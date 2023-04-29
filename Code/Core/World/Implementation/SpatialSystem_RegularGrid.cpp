#include <Core/CorePCH.h>

#include <Core/World/SpatialSystem_RegularGrid.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Profiling/Profiling.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/Time/Stopwatch.h>

wdCVarInt cvar_SpatialQueriesCachingThreshold("Spatial.Queries.CachingThreshold", 100, wdCVarFlags::Default, "Number of objects that are tested for a query before it is considered for caching");

namespace
{
  enum
  {
    MAX_CELL_INDEX = (1 << 20) - 1,
    CELL_INDEX_MASK = (1 << 21) - 1
  };

  WD_ALWAYS_INLINE wdSimdVec4f ToVec3(const wdSimdVec4i& v)
  {
    return v.ToFloat();
  }

  WD_ALWAYS_INLINE wdSimdVec4i ToVec3I32(const wdSimdVec4f& v)
  {
    wdSimdVec4f vf = v.Floor();
    return wdSimdVec4i::Truncate(vf);
  }

  WD_ALWAYS_INLINE wdUInt64 GetCellKey(wdInt32 x, wdInt32 y, wdInt32 z)
  {
    wdUInt64 sx = (x + MAX_CELL_INDEX) & CELL_INDEX_MASK;
    wdUInt64 sy = (y + MAX_CELL_INDEX) & CELL_INDEX_MASK;
    wdUInt64 sz = (z + MAX_CELL_INDEX) & CELL_INDEX_MASK;

    return (sx << 42) | (sy << 21) | sz;
  }

  WD_ALWAYS_INLINE wdSimdBBox ComputeCellBoundingBox(const wdSimdVec4i& vCellIndex, const wdSimdVec4i& vCellSize)
  {
    wdSimdVec4i overlapSize = vCellSize >> 2;
    wdSimdVec4i minPos = vCellIndex.CompMul(vCellSize);

    wdSimdVec4f bmin = ToVec3(minPos - overlapSize);
    wdSimdVec4f bmax = ToVec3(minPos + overlapSize + vCellSize);

    return wdSimdBBox(bmin, bmax);
  }

  WD_ALWAYS_INLINE bool FilterByCategory(wdUInt32 uiCategoryBitmask, wdUInt32 uiQueryBitmask)
  {
    return (uiCategoryBitmask & uiQueryBitmask) == 0;
  }

  WD_ALWAYS_INLINE bool FilterByTags(const wdTagSet& tags, const wdTagSet& includeTags, const wdTagSet& excludeTags)
  {
    if (!excludeTags.IsEmpty() && excludeTags.IsAnySet(tags))
      return true;

    if (!includeTags.IsEmpty() && !includeTags.IsAnySet(tags))
      return true;

    return false;
  }

  WD_ALWAYS_INLINE bool CanBeCached(wdSpatialData::Category category)
  {
    return wdSpatialData::GetCategoryFlags(category).IsSet(wdSpatialData::Flags::FrequentChanges) == false;
  }

  void TagsToString(const wdTagSet& tags, wdStringBuilder& out_sSb)
  {
    out_sSb.Append("{ ");

    bool first = true;
    for (auto it = tags.GetIterator(); it.IsValid(); ++it)
    {
      if (!first)
      {
        out_sSb.Append(", ");
        first = false;
      }
      out_sSb.Append(it->GetTagString().GetView());
    }

    out_sSb.Append(" }");
  }

  struct PlaneData
  {
    wdSimdVec4f m_x0x1x2x3;
    wdSimdVec4f m_y0y1y2y3;
    wdSimdVec4f m_z0z1z2z3;
    wdSimdVec4f m_w0w1w2w3;

    wdSimdVec4f m_x4x5x4x5;
    wdSimdVec4f m_y4y5y4y5;
    wdSimdVec4f m_z4z5z4z5;
    wdSimdVec4f m_w4w5w4w5;
  };

  WD_FORCE_INLINE bool SphereFrustumIntersect(const wdSimdBSphere& sphere, const PlaneData& planeData)
  {
    wdSimdVec4f pos_xxxx(sphere.m_CenterAndRadius.x());
    wdSimdVec4f pos_yyyy(sphere.m_CenterAndRadius.y());
    wdSimdVec4f pos_zzzz(sphere.m_CenterAndRadius.z());
    wdSimdVec4f pos_rrrr(sphere.m_CenterAndRadius.w());

    wdSimdVec4f dot_0123;
    dot_0123 = wdSimdVec4f::MulAdd(pos_xxxx, planeData.m_x0x1x2x3, planeData.m_w0w1w2w3);
    dot_0123 = wdSimdVec4f::MulAdd(pos_yyyy, planeData.m_y0y1y2y3, dot_0123);
    dot_0123 = wdSimdVec4f::MulAdd(pos_zzzz, planeData.m_z0z1z2z3, dot_0123);

    wdSimdVec4f dot_4545;
    dot_4545 = wdSimdVec4f::MulAdd(pos_xxxx, planeData.m_x4x5x4x5, planeData.m_w4w5w4w5);
    dot_4545 = wdSimdVec4f::MulAdd(pos_yyyy, planeData.m_y4y5y4y5, dot_4545);
    dot_4545 = wdSimdVec4f::MulAdd(pos_zzzz, planeData.m_z4z5z4z5, dot_4545);

    wdSimdVec4b cmp_0123 = dot_0123 > pos_rrrr;
    wdSimdVec4b cmp_4545 = dot_4545 > pos_rrrr;
    return (cmp_0123 || cmp_4545).NoneSet<4>();
  }

  WD_FORCE_INLINE wdUInt32 SphereFrustumIntersect(const wdSimdBSphere& sphereA, const wdSimdBSphere& sphereB, const PlaneData& planeData)
  {
    wdSimdVec4f posA_xxxx(sphereA.m_CenterAndRadius.x());
    wdSimdVec4f posA_yyyy(sphereA.m_CenterAndRadius.y());
    wdSimdVec4f posA_zzzz(sphereA.m_CenterAndRadius.z());
    wdSimdVec4f posA_rrrr(sphereA.m_CenterAndRadius.w());

    wdSimdVec4f dotA_0123;
    dotA_0123 = wdSimdVec4f::MulAdd(posA_xxxx, planeData.m_x0x1x2x3, planeData.m_w0w1w2w3);
    dotA_0123 = wdSimdVec4f::MulAdd(posA_yyyy, planeData.m_y0y1y2y3, dotA_0123);
    dotA_0123 = wdSimdVec4f::MulAdd(posA_zzzz, planeData.m_z0z1z2z3, dotA_0123);

    wdSimdVec4f posB_xxxx(sphereB.m_CenterAndRadius.x());
    wdSimdVec4f posB_yyyy(sphereB.m_CenterAndRadius.y());
    wdSimdVec4f posB_zzzz(sphereB.m_CenterAndRadius.z());
    wdSimdVec4f posB_rrrr(sphereB.m_CenterAndRadius.w());

    wdSimdVec4f dotB_0123;
    dotB_0123 = wdSimdVec4f::MulAdd(posB_xxxx, planeData.m_x0x1x2x3, planeData.m_w0w1w2w3);
    dotB_0123 = wdSimdVec4f::MulAdd(posB_yyyy, planeData.m_y0y1y2y3, dotB_0123);
    dotB_0123 = wdSimdVec4f::MulAdd(posB_zzzz, planeData.m_z0z1z2z3, dotB_0123);

    wdSimdVec4f posAB_xxxx = posA_xxxx.GetCombined<wdSwizzle::XXXX>(posB_xxxx);
    wdSimdVec4f posAB_yyyy = posA_yyyy.GetCombined<wdSwizzle::XXXX>(posB_yyyy);
    wdSimdVec4f posAB_zzzz = posA_zzzz.GetCombined<wdSwizzle::XXXX>(posB_zzzz);
    wdSimdVec4f posAB_rrrr = posA_rrrr.GetCombined<wdSwizzle::XXXX>(posB_rrrr);

    wdSimdVec4f dot_A45B45;
    dot_A45B45 = wdSimdVec4f::MulAdd(posAB_xxxx, planeData.m_x4x5x4x5, planeData.m_w4w5w4w5);
    dot_A45B45 = wdSimdVec4f::MulAdd(posAB_yyyy, planeData.m_y4y5y4y5, dot_A45B45);
    dot_A45B45 = wdSimdVec4f::MulAdd(posAB_zzzz, planeData.m_z4z5z4z5, dot_A45B45);

    wdSimdVec4b cmp_A0123 = dotA_0123 > posA_rrrr;
    wdSimdVec4b cmp_B0123 = dotB_0123 > posB_rrrr;
    wdSimdVec4b cmp_A45B45 = dot_A45B45 > posAB_rrrr;

    wdSimdVec4b cmp_A45 = cmp_A45B45.Get<wdSwizzle::XYXY>();
    wdSimdVec4b cmp_B45 = cmp_A45B45.Get<wdSwizzle::ZWZW>();

    wdUInt32 result = (cmp_A0123 || cmp_A45).NoneSet<4>() ? 1 : 0;
    result |= (cmp_B0123 || cmp_B45).NoneSet<4>() ? 2 : 0;

    return result;
  }
} // namespace

//////////////////////////////////////////////////////////////////////////

struct CellDataMapping
{
  WD_DECLARE_POD_TYPE();

  wdUInt32 m_uiCellIndex = wdInvalidIndex;
  wdUInt32 m_uiCellDataIndex = wdInvalidIndex;
};

struct wdSpatialSystem_RegularGrid::Cell
{
  Cell(wdAllocatorBase* pAlignedAlloctor, wdAllocatorBase* pAllocator)
    : m_BoundingSpheres(pAlignedAlloctor)
    , m_BoundingBoxHalfExtents(pAlignedAlloctor)
    , m_TagSets(pAllocator)
    , m_ObjectPointers(pAllocator)
    , m_DataIndices(pAllocator)
  {
  }

  WD_FORCE_INLINE wdUInt32 AddData(const wdSimdBBoxSphere& bounds, const wdTagSet& tags, wdGameObject* pObject, wdUInt64 uiLastVisibleFrame, wdUInt32 uiDataIndex)
  {
    m_BoundingSpheres.PushBack(bounds.GetSphere());
    m_BoundingBoxHalfExtents.PushBack(bounds.m_BoxHalfExtents);
    m_TagSets.PushBack(tags);
    m_ObjectPointers.PushBack(pObject);
    m_LastVisibleFrames.PushBack(uiLastVisibleFrame);
    m_DataIndices.PushBack(uiDataIndex);

    return m_BoundingSpheres.GetCount() - 1;
  }

  // Returns the data index of the moved data
  WD_FORCE_INLINE wdUInt32 RemoveData(wdUInt32 uiCellDataIndex)
  {
    wdUInt32 uiMovedDataIndex = m_DataIndices.PeekBack();

    m_BoundingSpheres.RemoveAtAndSwap(uiCellDataIndex);
    m_BoundingBoxHalfExtents.RemoveAtAndSwap(uiCellDataIndex);
    m_TagSets.RemoveAtAndSwap(uiCellDataIndex);
    m_ObjectPointers.RemoveAtAndSwap(uiCellDataIndex);
    m_LastVisibleFrames.RemoveAtAndSwap(uiCellDataIndex);
    m_DataIndices.RemoveAtAndSwap(uiCellDataIndex);
    WD_ASSERT_DEBUG(m_DataIndices.GetCount() == uiCellDataIndex || m_DataIndices[uiCellDataIndex] == uiMovedDataIndex, "Implementation error");

    return uiMovedDataIndex;
  }

  WD_ALWAYS_INLINE wdBoundingBox GetBoundingBox() const { return wdSimdConversion::ToBBoxSphere(m_Bounds).GetBox(); }

  wdSimdBBoxSphere m_Bounds;

  wdDynamicArray<wdSimdBSphere> m_BoundingSpheres;
  wdDynamicArray<wdSimdVec4f> m_BoundingBoxHalfExtents;
  wdDynamicArray<wdTagSet> m_TagSets;
  wdDynamicArray<wdGameObject*> m_ObjectPointers;
  mutable wdDynamicArray<wdUInt64> m_LastVisibleFrames; // multi-threaded access is ok, since all threads will set the same value
  wdDynamicArray<wdUInt32> m_DataIndices;
};

//////////////////////////////////////////////////////////////////////////

struct CellKeyHashHelper
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(wdUInt64 value)
  {
    // return wdUInt32(value * 2654435761U);
    return wdHashHelper<wdUInt64>::Hash(value);
  }

  WD_ALWAYS_INLINE static bool Equal(wdUInt64 a, wdUInt64 b) { return a == b; }
};

//////////////////////////////////////////////////////////////////////////

struct wdSpatialSystem_RegularGrid::Grid
{
  Grid(wdSpatialSystem_RegularGrid& ref_system, wdSpatialData::Category category)
    : m_System(ref_system)
    , m_Cells(&ref_system.m_Allocator)
    , m_CellKeyToCellIndex(&ref_system.m_Allocator)
    , m_Category(category)
    , m_bCanBeCached(CanBeCached(category))
  {
    wdSimdBBox overflowBox;
    overflowBox.SetCenterAndHalfExtents(wdSimdVec4f::ZeroVector(), wdSimdVec4f((float)(ref_system.m_vCellSize.x() * MAX_CELL_INDEX)));

    auto pOverflowCell = WD_NEW(&m_System.m_AlignedAllocator, Cell, &m_System.m_AlignedAllocator, &m_System.m_Allocator);
    pOverflowCell->m_Bounds = overflowBox;

    m_Cells.PushBack(pOverflowCell);
  }

  wdUInt32 GetOrCreateCell(const wdSimdBBoxSphere& bounds)
  {
    wdSimdVec4i cellIndex = ToVec3I32(bounds.m_CenterAndRadius * m_System.m_fInvCellSize);
    wdSimdBBox cellBox = ComputeCellBoundingBox(cellIndex, m_System.m_vCellSize);

    if (cellBox.Contains(bounds.GetBox()))
    {
      wdUInt64 cellKey = GetCellKey(cellIndex.x(), cellIndex.y(), cellIndex.z());

      wdUInt32 uiCellIndex = 0;
      if (m_CellKeyToCellIndex.TryGetValue(cellKey, uiCellIndex))
      {
        return uiCellIndex;
      }

      uiCellIndex = m_Cells.GetCount();
      m_CellKeyToCellIndex.Insert(cellKey, uiCellIndex);

      auto pNewCell = WD_NEW(&m_System.m_AlignedAllocator, Cell, &m_System.m_AlignedAllocator, &m_System.m_Allocator);
      pNewCell->m_Bounds = cellBox;

      m_Cells.PushBack(pNewCell);

      return uiCellIndex;
    }
    else
    {
      return m_uiOverflowCellIndex;
    }
  }

  void AddSpatialData(const wdSimdBBoxSphere& bounds, const wdTagSet& tags, wdGameObject* pObject, wdUInt64 uiLastVisibleFrame, const wdSpatialDataHandle& hData)
  {
    wdUInt32 uiDataIndex = hData.GetInternalID().m_InstanceIndex;

    wdUInt32 uiCellIndex = GetOrCreateCell(bounds);
    wdUInt32 uiCellDataIndex = m_Cells[uiCellIndex]->AddData(bounds, tags, pObject, uiLastVisibleFrame, uiDataIndex);

    m_CellDataMappings.EnsureCount(uiDataIndex + 1);
    WD_ASSERT_DEBUG(m_CellDataMappings[uiDataIndex].m_uiCellIndex == wdInvalidIndex, "data has already been added to a cell");
    m_CellDataMappings[uiDataIndex] = {uiCellIndex, uiCellDataIndex};
  }

  void RemoveSpatialData(const wdSpatialDataHandle& hData)
  {
    wdUInt32 uiDataIndex = hData.GetInternalID().m_InstanceIndex;

    auto& mapping = m_CellDataMappings[uiDataIndex];
    wdUInt32 uiMovedDataIndex = m_Cells[mapping.m_uiCellIndex]->RemoveData(mapping.m_uiCellDataIndex);
    if (uiMovedDataIndex != uiDataIndex)
    {
      m_CellDataMappings[uiMovedDataIndex].m_uiCellDataIndex = mapping.m_uiCellDataIndex;
    }

    mapping = {};
  }

  bool MigrateSpatialDataFromOtherGrid(wdUInt32 uiDataIndex, const Grid& other)
  {
    // Data has already been added
    if (uiDataIndex < m_CellDataMappings.GetCount() && m_CellDataMappings[uiDataIndex].m_uiCellIndex != wdInvalidIndex)
      return false;

    auto& mapping = other.m_CellDataMappings[uiDataIndex];
    if (mapping.m_uiCellIndex == wdInvalidIndex)
      return false;

    auto& pOtherCell = other.m_Cells[mapping.m_uiCellIndex];

    const wdTagSet& tags = pOtherCell->m_TagSets[mapping.m_uiCellDataIndex];
    if (FilterByTags(tags, m_IncludeTags, m_ExcludeTags))
      return false;

    wdSimdBBoxSphere bounds;
    bounds.m_CenterAndRadius = pOtherCell->m_BoundingSpheres[mapping.m_uiCellDataIndex].m_CenterAndRadius;
    bounds.m_BoxHalfExtents = pOtherCell->m_BoundingBoxHalfExtents[mapping.m_uiCellDataIndex];
    wdGameObject* objectPointer = pOtherCell->m_ObjectPointers[mapping.m_uiCellDataIndex];
    wdUInt64 uiLastVisibleFrame = pOtherCell->m_LastVisibleFrames[mapping.m_uiCellDataIndex];

    WD_ASSERT_DEBUG(pOtherCell->m_DataIndices[mapping.m_uiCellDataIndex] == uiDataIndex, "Implementation error");
    wdSpatialDataHandle hData = wdSpatialDataHandle(wdSpatialDataId(uiDataIndex, 1));

    AddSpatialData(bounds, tags, objectPointer, uiLastVisibleFrame, hData);
    return true;
  }

  WD_ALWAYS_INLINE bool CachingCompleted() const { return m_uiLastMigrationIndex == wdInvalidIndex; }

  template <typename Functor>
  WD_FORCE_INLINE void ForEachCellInBox(const wdSimdBBox& box, Functor func) const
  {
    wdSimdVec4i minIndex = ToVec3I32((box.m_Min - m_System.m_vOverlapSize) * m_System.m_fInvCellSize);
    wdSimdVec4i maxIndex = ToVec3I32((box.m_Max + m_System.m_vOverlapSize) * m_System.m_fInvCellSize);

    WD_ASSERT_DEBUG((minIndex.Abs() < wdSimdVec4i(MAX_CELL_INDEX)).AllSet<3>(), "Position is too big");
    WD_ASSERT_DEBUG((maxIndex.Abs() < wdSimdVec4i(MAX_CELL_INDEX)).AllSet<3>(), "Position is too big");

    const wdInt32 iMinX = minIndex.x();
    const wdInt32 iMinY = minIndex.y();
    const wdInt32 iMinZ = minIndex.z();

    const wdSimdVec4i diff = maxIndex - minIndex + wdSimdVec4i(1);
    const wdInt32 iDiffX = diff.x();
    const wdInt32 iDiffY = diff.y();
    const wdInt32 iDiffZ = diff.z();
    const wdInt32 iNumIterations = iDiffX * iDiffY * iDiffZ;

    for (wdInt32 i = 0; i < iNumIterations; ++i)
    {
      wdInt32 index = i;
      wdInt32 z = i / (iDiffX * iDiffY);
      index -= z * iDiffX * iDiffY;
      wdInt32 y = index / iDiffX;
      wdInt32 x = index - (y * iDiffX);

      x += iMinX;
      y += iMinY;
      z += iMinZ;

      wdUInt64 cellKey = GetCellKey(x, y, z);
      wdUInt32 cellIndex = 0;
      if (m_CellKeyToCellIndex.TryGetValue(cellKey, cellIndex))
      {
        const Cell& constCell = *m_Cells[cellIndex];
        if (func(constCell) == wdVisitorExecution::Stop)
          return;
      }
    }

    const Cell& overflowCell = *m_Cells[m_uiOverflowCellIndex];
    func(overflowCell);
  }

  wdSpatialSystem_RegularGrid& m_System;
  wdDynamicArray<wdUniquePtr<Cell>> m_Cells;

  wdHashTable<wdUInt64, wdUInt32, CellKeyHashHelper> m_CellKeyToCellIndex;
  static constexpr wdUInt32 m_uiOverflowCellIndex = 0;

  wdDynamicArray<CellDataMapping> m_CellDataMappings;

  const wdSpatialData::Category m_Category;
  const bool m_bCanBeCached;

  wdTagSet m_IncludeTags;
  wdTagSet m_ExcludeTags;

  wdUInt32 m_uiLastMigrationIndex = 0;
};

//////////////////////////////////////////////////////////////////////////

struct wdSpatialSystem_RegularGrid::Stats
{
  wdUInt32 m_uiNumObjectsTested = 0;
  wdUInt32 m_uiNumObjectsPassed = 0;
  wdUInt32 m_uiNumObjectsFiltered = 0;
};

//////////////////////////////////////////////////////////////////////////

namespace wdInternal
{
  struct QueryHelper
  {
    template <typename T>
    struct ShapeQueryData
    {
      T m_Shape;
      wdSpatialSystem::QueryCallback m_Callback;
    };

    template <typename T, bool UseTagsFilter>
    static wdVisitorExecution::Enum ShapeQueryCallback(const wdSpatialSystem_RegularGrid::Cell& cell, const wdSpatialSystem::QueryParams& queryParams, wdSpatialSystem_RegularGrid::Stats& ref_stats, void* pUserData)
    {
      auto pQueryData = static_cast<const ShapeQueryData<T>*>(pUserData);
      T shape = pQueryData->m_Shape;

      wdSimdBBox cellBox = cell.m_Bounds.GetBox();
      if (!cellBox.Overlaps(shape))
        return wdVisitorExecution::Continue;

      auto boundingSpheres = cell.m_BoundingSpheres.GetData();
      auto tagSets = cell.m_TagSets.GetData();
      auto objectPointers = cell.m_ObjectPointers.GetData();

      const wdUInt32 numSpheres = cell.m_BoundingSpheres.GetCount();
      ref_stats.m_uiNumObjectsTested += numSpheres;

      for (wdUInt32 i = 0; i < numSpheres; ++i)
      {
        if (!shape.Overlaps(boundingSpheres[i]))
          continue;

        if constexpr (UseTagsFilter)
        {
          if (FilterByTags(tagSets[i], queryParams.m_IncludeTags, queryParams.m_ExcludeTags))
          {
            ref_stats.m_uiNumObjectsFiltered++;
            continue;
          }
        }

        ref_stats.m_uiNumObjectsPassed++;

        if (pQueryData->m_Callback(objectPointers[i]) == wdVisitorExecution::Stop)
          return wdVisitorExecution::Stop;
      }

      return wdVisitorExecution::Continue;
    }

    struct FrustumQueryData
    {
      PlaneData m_PlaneData;
      wdDynamicArray<const wdGameObject*>* m_pOutObjects;
      wdUInt64 m_uiFrameCounter;
      wdSpatialSystem::IsOccludedFunc m_IsOccludedCB;
    };

    template <bool UseTagsFilter, bool UseOcclusionCallback>
    static wdVisitorExecution::Enum FrustumQueryCallback(const wdSpatialSystem_RegularGrid::Cell& cell, const wdSpatialSystem::QueryParams& queryParams, wdSpatialSystem_RegularGrid::Stats& ref_stats, void* pUserData)
    {
      auto pQueryData = static_cast<FrustumQueryData*>(pUserData);
      PlaneData planeData = pQueryData->m_PlaneData;

      wdSimdBSphere cellSphere = cell.m_Bounds.GetSphere();
      if (!SphereFrustumIntersect(cellSphere, planeData))
        return wdVisitorExecution::Continue;

      if constexpr (UseOcclusionCallback)
      {
        if (pQueryData->m_IsOccludedCB(cell.m_Bounds.GetBox()))
        {
          return wdVisitorExecution::Continue;
        }
      }

      wdSimdBBox bbox;
      auto boundingSpheres = cell.m_BoundingSpheres.GetData();
      auto boundingBoxHalfExtents = cell.m_BoundingBoxHalfExtents.GetData();
      auto tagSets = cell.m_TagSets.GetData();
      auto objectPointers = cell.m_ObjectPointers.GetData();
      auto lastVisibleFrames = cell.m_LastVisibleFrames.GetData();

      const wdUInt32 numSpheres = cell.m_BoundingSpheres.GetCount();
      ref_stats.m_uiNumObjectsTested += numSpheres;

      wdUInt32 currentIndex = 0;

      while (currentIndex < numSpheres)
      {
        if (numSpheres - currentIndex >= 32)
        {
          wdUInt32 mask = 0;

          for (wdUInt32 i = 0; i < 32; i += 2)
          {
            auto& objectSphereA = boundingSpheres[currentIndex + i + 0];
            auto& objectSphereB = boundingSpheres[currentIndex + i + 1];

            mask |= SphereFrustumIntersect(objectSphereA, objectSphereB, planeData) << i;
          }

          while (mask > 0)
          {
            wdUInt32 i = wdMath::FirstBitLow(mask) + currentIndex;
            mask &= mask - 1;

            if constexpr (UseTagsFilter)
            {
              if (FilterByTags(tagSets[i], queryParams.m_IncludeTags, queryParams.m_ExcludeTags))
              {
                ref_stats.m_uiNumObjectsFiltered++;
                continue;
              }
            }

            if constexpr (UseOcclusionCallback)
            {
              bbox.SetCenterAndHalfExtents(boundingSpheres[i].GetCenter(), boundingBoxHalfExtents[i]);
              if (pQueryData->m_IsOccludedCB(bbox))
              {
                continue;
              }
            }

            lastVisibleFrames[i] = pQueryData->m_uiFrameCounter;
            pQueryData->m_pOutObjects->PushBack(objectPointers[i]);

            ref_stats.m_uiNumObjectsPassed++;
          }

          currentIndex += 32;
        }
        else
        {
          wdUInt32 i = currentIndex;
          ++currentIndex;

          if (!SphereFrustumIntersect(boundingSpheres[i], planeData))
            continue;

          if constexpr (UseTagsFilter)
          {
            if (FilterByTags(tagSets[i], queryParams.m_IncludeTags, queryParams.m_ExcludeTags))
            {
              ref_stats.m_uiNumObjectsFiltered++;
              continue;
            }
          }

          if constexpr (UseOcclusionCallback)
          {
            bbox.SetCenterAndHalfExtents(boundingSpheres[i].GetCenter(), boundingBoxHalfExtents[i]);

            if (pQueryData->m_IsOccludedCB(bbox))
            {
              continue;
            }
          }

          lastVisibleFrames[i] = pQueryData->m_uiFrameCounter;
          pQueryData->m_pOutObjects->PushBack(objectPointers[i]);

          ref_stats.m_uiNumObjectsPassed++;
        }
      }

      return wdVisitorExecution::Continue;
    }
  };
} // namespace wdInternal

//////////////////////////////////////////////////////////////////////////

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdSpatialSystem_RegularGrid, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;

wdSpatialSystem_RegularGrid::wdSpatialSystem_RegularGrid(wdUInt32 uiCellSize /*= 128*/)
  : m_AlignedAllocator("Spatial System Aligned", wdFoundation::GetAlignedAllocator())
  , m_Grids(&m_Allocator)
  , m_DataTable(&m_Allocator)
  , m_vCellSize(uiCellSize)
  , m_vOverlapSize(uiCellSize / 4.0f)
  , m_fInvCellSize(1.0f / uiCellSize)
{
  WD_CHECK_AT_COMPILETIME(sizeof(Data) == 8);

  m_Grids.SetCount(MAX_NUM_GRIDS);

  cvar_SpatialQueriesCachingThreshold.m_CVarEvents.AddEventHandler([&](const wdCVarEvent& e) {
    if (e.m_EventType == wdCVarEvent::ValueChanged)
    {
      RemoveAllCachedGrids();
    } });
}

wdSpatialSystem_RegularGrid::~wdSpatialSystem_RegularGrid() = default;

wdResult wdSpatialSystem_RegularGrid::GetCellBoxForSpatialData(const wdSpatialDataHandle& hData, wdBoundingBox& out_boundingBox) const
{
  Data* pData = nullptr;
  if (!m_DataTable.TryGetValue(hData.GetInternalID(), pData))
    return WD_FAILURE;

  ForEachGrid(*pData, hData,
    [&](Grid& ref_grid, const CellDataMapping& mapping) {
      auto& pCell = ref_grid.m_Cells[mapping.m_uiCellIndex];

      out_boundingBox = pCell->GetBoundingBox();
      return wdVisitorExecution::Stop;
    });

  return WD_SUCCESS;
}

template <>
struct wdHashHelper<wdBoundingBox>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(const wdBoundingBox& value) { return wdHashingUtils::xxHash32(&value, sizeof(wdBoundingBox)); }

  WD_ALWAYS_INLINE static bool Equal(const wdBoundingBox& a, const wdBoundingBox& b) { return a == b; }
};

void wdSpatialSystem_RegularGrid::GetAllCellBoxes(wdDynamicArray<wdBoundingBox>& out_boundingBoxes, wdSpatialData::Category filterCategory /*= wdInvalidSpatialDataCategory*/) const
{
  if (filterCategory != wdInvalidSpatialDataCategory)
  {
    wdUInt32 uiGridIndex = filterCategory.m_uiValue;
    auto& pGrid = m_Grids[uiGridIndex];
    if (pGrid != nullptr)
    {
      for (auto& pCell : pGrid->m_Cells)
      {
        out_boundingBoxes.ExpandAndGetRef() = pCell->GetBoundingBox();
      }
    }
  }
  else
  {
    wdHashSet<wdBoundingBox> boundingBoxes;

    for (auto& pGrid : m_Grids)
    {
      if (pGrid != nullptr)
      {
        for (auto& pCell : pGrid->m_Cells)
        {
          boundingBoxes.Insert(pCell->GetBoundingBox());
        }
      }
    }

    for (auto boundingBox : boundingBoxes)
    {
      out_boundingBoxes.PushBack(boundingBox);
    }
  }
}

void wdSpatialSystem_RegularGrid::StartNewFrame()
{
  SUPER::StartNewFrame();

  m_SortedCacheCandidates.Clear();

  {
    WD_LOCK(m_CacheCandidatesMutex);

    for (wdUInt32 i = 0; i < m_CacheCandidates.GetCount(); ++i)
    {
      auto& cacheCandidate = m_CacheCandidates[i];

      const float fScore = cacheCandidate.m_fQueryCount + cacheCandidate.m_fFilteredRatio * 100.0f;
      m_SortedCacheCandidates.PushBack({i, fScore});

      // Query has to be issued at least once every 10 frames to keep a stable value
      cacheCandidate.m_fQueryCount = wdMath::Max(cacheCandidate.m_fQueryCount - 0.1f, 0.0f);
    }
  }

  m_SortedCacheCandidates.Sort();

  // First remove all cached grids that don't make it into the top MAX_NUM_CACHED_GRIDS to make space for new grids
  if (m_SortedCacheCandidates.GetCount() > MAX_NUM_CACHED_GRIDS)
  {
    for (wdUInt32 i = MAX_NUM_CACHED_GRIDS; i < m_SortedCacheCandidates.GetCount(); ++i)
    {
      RemoveCachedGrid(m_SortedCacheCandidates[i].m_uiIndex);
    }
  }

  // Then take the MAX_NUM_CACHED_GRIDS candidates with the highest score and migrate the data
  for (wdUInt32 i = 0; i < wdMath::Min<wdUInt32>(m_SortedCacheCandidates.GetCount(), MAX_NUM_CACHED_GRIDS); ++i)
  {
    MigrateCachedGrid(m_SortedCacheCandidates[i].m_uiIndex);
  }
}

wdSpatialDataHandle wdSpatialSystem_RegularGrid::CreateSpatialData(const wdSimdBBoxSphere& bounds, wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags)
{
  if (uiCategoryBitmask == 0)
    return wdSpatialDataHandle();

  return AddSpatialDataToGrids(bounds, pObject, uiCategoryBitmask, tags, false);
}

wdSpatialDataHandle wdSpatialSystem_RegularGrid::CreateSpatialDataAlwaysVisible(wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags)
{
  if (uiCategoryBitmask == 0)
    return wdSpatialDataHandle();

  wdSimdBBox hugeBox;
  hugeBox.SetCenterAndHalfExtents(wdSimdVec4f::ZeroVector(), wdSimdVec4f((float)(m_vCellSize.x() * MAX_CELL_INDEX)));

  return AddSpatialDataToGrids(hugeBox, pObject, uiCategoryBitmask, tags, true);
}

void wdSpatialSystem_RegularGrid::DeleteSpatialData(const wdSpatialDataHandle& hData)
{
  Data oldData;
  WD_VERIFY(m_DataTable.Remove(hData.GetInternalID(), &oldData), "Invalid spatial data handle");

  ForEachGrid(oldData, hData,
    [&](Grid& ref_grid, const CellDataMapping& mapping) {
      ref_grid.RemoveSpatialData(hData);
      return wdVisitorExecution::Continue;
    });
}

void wdSpatialSystem_RegularGrid::UpdateSpatialDataBounds(const wdSpatialDataHandle& hData, const wdSimdBBoxSphere& bounds)
{
  Data* pData = nullptr;
  WD_VERIFY(m_DataTable.TryGetValue(hData.GetInternalID(), pData), "Invalid spatial data handle");

  // No need to update bounds for always visible data
  if (IsAlwaysVisibleData(*pData))
    return;

  ForEachGrid(*pData, hData,
    [&](Grid& ref_grid, const CellDataMapping& mapping) {
      auto& pOldCell = ref_grid.m_Cells[mapping.m_uiCellIndex];

      if (pOldCell->m_Bounds.GetBox().Contains(bounds.GetBox()))
      {
        pOldCell->m_BoundingSpheres[mapping.m_uiCellDataIndex] = bounds.GetSphere();
        pOldCell->m_BoundingBoxHalfExtents[mapping.m_uiCellDataIndex] = bounds.m_BoxHalfExtents;
      }
      else
      {
        const wdTagSet tags = pOldCell->m_TagSets[mapping.m_uiCellDataIndex];
        wdGameObject* objectPointer = pOldCell->m_ObjectPointers[mapping.m_uiCellDataIndex];
        wdUInt64 uiLastVisibleFrame = pOldCell->m_LastVisibleFrames[mapping.m_uiCellDataIndex];

        ref_grid.RemoveSpatialData(hData);

        ref_grid.AddSpatialData(bounds, tags, objectPointer, uiLastVisibleFrame, hData);
      }

      return wdVisitorExecution::Continue;
    });
}

void wdSpatialSystem_RegularGrid::UpdateSpatialDataObject(const wdSpatialDataHandle& hData, wdGameObject* pObject)
{
  Data* pData = nullptr;
  WD_VERIFY(m_DataTable.TryGetValue(hData.GetInternalID(), pData), "Invalid spatial data handle");

  ForEachGrid(*pData, hData,
    [&](Grid& ref_grid, const CellDataMapping& mapping) {
      auto& pCell = ref_grid.m_Cells[mapping.m_uiCellIndex];
      pCell->m_ObjectPointers[mapping.m_uiCellDataIndex] = pObject;
      return wdVisitorExecution::Continue;
    });
}

void wdSpatialSystem_RegularGrid::FindObjectsInSphere(const wdBoundingSphere& sphere, const QueryParams& queryParams, QueryCallback callback) const
{
  WD_PROFILE_SCOPE("FindObjectsInSphere");

  wdSimdBSphere simdSphere(wdSimdConversion::ToVec3(sphere.m_vCenter), sphere.m_fRadius);
  wdSimdBBox simdBox;
  simdBox.SetCenterAndHalfExtents(simdSphere.m_CenterAndRadius, simdSphere.m_CenterAndRadius.Get<wdSwizzle::WWWW>());

  wdInternal::QueryHelper::ShapeQueryData<wdSimdBSphere> queryData = {simdSphere, callback};

  ForEachCellInBoxInMatchingGrids(simdBox, queryParams,
    &wdInternal::QueryHelper::ShapeQueryCallback<wdSimdBSphere, false>,
    &wdInternal::QueryHelper::ShapeQueryCallback<wdSimdBSphere, true>,
    &queryData);
}

void wdSpatialSystem_RegularGrid::FindObjectsInBox(const wdBoundingBox& box, const QueryParams& queryParams, QueryCallback callback) const
{
  WD_PROFILE_SCOPE("FindObjectsInBox");

  wdSimdBBox simdBox(wdSimdConversion::ToVec3(box.m_vMin), wdSimdConversion::ToVec3(box.m_vMax));

  wdInternal::QueryHelper::ShapeQueryData<wdSimdBBox> queryData = {simdBox, callback};

  ForEachCellInBoxInMatchingGrids(simdBox, queryParams,
    &wdInternal::QueryHelper::ShapeQueryCallback<wdSimdBBox, false>,
    &wdInternal::QueryHelper::ShapeQueryCallback<wdSimdBBox, true>,
    &queryData);
}

void wdSpatialSystem_RegularGrid::FindVisibleObjects(const wdFrustum& frustum, const QueryParams& queryParams, wdDynamicArray<const wdGameObject*>& out_Objects, wdSpatialSystem::IsOccludedFunc IsOccluded) const
{
  WD_PROFILE_SCOPE("FindVisibleObjects");

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  wdStopwatch timer;
#endif

  wdVec3 cornerPoints[8];
  frustum.ComputeCornerPoints(cornerPoints);

  wdSimdVec4f simdCornerPoints[8];
  for (wdUInt32 i = 0; i < 8; ++i)
  {
    simdCornerPoints[i] = wdSimdConversion::ToVec3(cornerPoints[i]);
  }

  wdSimdBBox simdBox;
  simdBox.SetFromPoints(simdCornerPoints, 8);

  wdInternal::QueryHelper::FrustumQueryData queryData;
  {
    // Compiler is too stupid to properly unroll a constant loop so we do it by hand
    wdSimdVec4f plane0 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(0).m_vNormal.x)));
    wdSimdVec4f plane1 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(1).m_vNormal.x)));
    wdSimdVec4f plane2 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(2).m_vNormal.x)));
    wdSimdVec4f plane3 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(3).m_vNormal.x)));
    wdSimdVec4f plane4 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(4).m_vNormal.x)));
    wdSimdVec4f plane5 = wdSimdConversion::ToVec4(*reinterpret_cast<const wdVec4*>(&(frustum.GetPlane(5).m_vNormal.x)));

    wdSimdMat4f helperMat;
    helperMat.SetRows(plane0, plane1, plane2, plane3);

    queryData.m_PlaneData.m_x0x1x2x3 = helperMat.m_col0;
    queryData.m_PlaneData.m_y0y1y2y3 = helperMat.m_col1;
    queryData.m_PlaneData.m_z0z1z2z3 = helperMat.m_col2;
    queryData.m_PlaneData.m_w0w1w2w3 = helperMat.m_col3;

    helperMat.SetRows(plane4, plane5, plane4, plane5);

    queryData.m_PlaneData.m_x4x5x4x5 = helperMat.m_col0;
    queryData.m_PlaneData.m_y4y5y4y5 = helperMat.m_col1;
    queryData.m_PlaneData.m_z4z5z4z5 = helperMat.m_col2;
    queryData.m_PlaneData.m_w4w5w4w5 = helperMat.m_col3;

    queryData.m_pOutObjects = &out_Objects;
    queryData.m_uiFrameCounter = m_uiFrameCounter;

    queryData.m_IsOccludedCB = IsOccluded;
  }

  if (IsOccluded.IsValid())
  {
    ForEachCellInBoxInMatchingGrids(simdBox, queryParams,
      &wdInternal::QueryHelper::FrustumQueryCallback<false, true>,
      &wdInternal::QueryHelper::FrustumQueryCallback<true, true>,
      &queryData);
  }
  else
  {
    ForEachCellInBoxInMatchingGrids(simdBox, queryParams,
      &wdInternal::QueryHelper::FrustumQueryCallback<false, false>,
      &wdInternal::QueryHelper::FrustumQueryCallback<true, false>,
      &queryData);
  }

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  if (queryParams.m_pStats != nullptr)
  {
    queryParams.m_pStats->m_TimeTaken = timer.GetRunningTotal();
  }
#endif
}

wdUInt64 wdSpatialSystem_RegularGrid::GetNumFramesSinceVisible(const wdSpatialDataHandle& hData) const
{
  Data* pData = nullptr;
  WD_VERIFY(m_DataTable.TryGetValue(hData.GetInternalID(), pData), "Invalid spatial data handle");

  if (IsAlwaysVisibleData(*pData))
    return 0;

  wdUInt64 uiLastFrameVisible = 0;
  ForEachGrid(*pData, hData,
    [&](const Grid& grid, const CellDataMapping& mapping) {
      auto& pCell = grid.m_Cells[mapping.m_uiCellIndex];
      uiLastFrameVisible = wdMath::Max(uiLastFrameVisible, pCell->m_LastVisibleFrames[mapping.m_uiCellDataIndex]);
      return wdVisitorExecution::Continue;
    });

  return (m_uiFrameCounter > uiLastFrameVisible) ? m_uiFrameCounter - uiLastFrameVisible : 0;
}

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
void wdSpatialSystem_RegularGrid::GetInternalStats(wdStringBuilder& sb) const
{
  sb = "Cache Candidates:\n";

  WD_LOCK(m_CacheCandidatesMutex);

  for (auto& sortedCandidate : m_SortedCacheCandidates)
  {
    auto& candidate = m_CacheCandidates[sortedCandidate.m_uiIndex];

    sb.AppendFormat(" \nCategory: {}\nInclude Tags: ", candidate.m_Category.m_uiValue);
    TagsToString(candidate.m_IncludeTags, sb);
    sb.Append("\nExclude Tags: ");
    TagsToString(candidate.m_ExcludeTags, sb);
    sb.AppendFormat("\nScore: {}", wdArgF(sortedCandidate.m_fScore, 2));

    const wdUInt32 uiGridIndex = candidate.m_uiGridIndex;
    if (uiGridIndex != wdInvalidIndex)
    {
      auto& pGrid = m_Grids[uiGridIndex];
      if (pGrid->CachingCompleted())
      {
        sb.Append("\nReady to use!\n");
      }
      else
      {
        const wdUInt32 uiNumObjectsMigrated = pGrid->m_uiLastMigrationIndex;
        sb.AppendFormat("\nMigration Status: {}%%\n", wdArgF(float(uiNumObjectsMigrated) / m_DataTable.GetCount() * 100.0f, 2));
      }
    }
  }
}
#endif

WD_ALWAYS_INLINE bool wdSpatialSystem_RegularGrid::IsAlwaysVisibleData(const Data& data) const
{
  return data.m_uiAlwaysVisible != 0;
}

wdSpatialDataHandle wdSpatialSystem_RegularGrid::AddSpatialDataToGrids(const wdSimdBBoxSphere& bounds, wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags, bool bAlwaysVisible)
{
  Data data;
  data.m_uiGridBitmask = uiCategoryBitmask;
  data.m_uiAlwaysVisible = bAlwaysVisible ? 1 : 0;

  // find matching cached grids and add them to data.m_uiGridBitmask
  for (wdUInt32 uiCachedGridIndex = m_uiFirstCachedGridIndex; uiCachedGridIndex < m_Grids.GetCount(); ++uiCachedGridIndex)
  {
    auto& pGrid = m_Grids[uiCachedGridIndex];
    if (pGrid == nullptr)
      continue;

    if ((pGrid->m_Category.GetBitmask() & uiCategoryBitmask) == 0 ||
        FilterByTags(tags, pGrid->m_IncludeTags, pGrid->m_ExcludeTags))
      continue;

    data.m_uiGridBitmask |= WD_BIT(uiCachedGridIndex);
  }

  auto hData = wdSpatialDataHandle(m_DataTable.Insert(data));

  wdUInt64 uiGridBitmask = data.m_uiGridBitmask;
  while (uiGridBitmask > 0)
  {
    wdUInt32 uiGridIndex = wdMath::FirstBitLow(uiGridBitmask);
    uiGridBitmask &= uiGridBitmask - 1;

    auto& pGrid = m_Grids[uiGridIndex];
    if (pGrid == nullptr)
    {
      pGrid = WD_NEW(&m_Allocator, Grid, *this, wdSpatialData::Category(uiGridIndex));
    }

    pGrid->AddSpatialData(bounds, tags, pObject, m_uiFrameCounter, hData);
  }

  return hData;
}

template <typename Functor>
WD_FORCE_INLINE void wdSpatialSystem_RegularGrid::ForEachGrid(const Data& data, const wdSpatialDataHandle& hData, Functor func) const
{
  wdUInt64 uiGridBitmask = data.m_uiGridBitmask;
  wdUInt32 uiDataIndex = hData.GetInternalID().m_InstanceIndex;

  while (uiGridBitmask > 0)
  {
    wdUInt32 uiGridIndex = wdMath::FirstBitLow(uiGridBitmask);
    uiGridBitmask &= uiGridBitmask - 1;

    auto& grid = *m_Grids[uiGridIndex];
    auto& mapping = grid.m_CellDataMappings[uiDataIndex];

    if (func(grid, mapping) == wdVisitorExecution::Stop)
      break;
  }
}

void wdSpatialSystem_RegularGrid::ForEachCellInBoxInMatchingGrids(const wdSimdBBox& box, const QueryParams& queryParams, CellCallback noFilterCallback, CellCallback filterByTagsCallback, void* pUserData) const
{
#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  if (queryParams.m_pStats != nullptr)
  {
    queryParams.m_pStats->m_uiTotalNumObjects = m_DataTable.GetCount();
  }
#endif

  wdUInt32 uiGridBitmask = queryParams.m_uiCategoryBitmask;

  // search for cached grids that match the exact query params first
  for (wdUInt32 uiCachedGridIndex = m_uiFirstCachedGridIndex; uiCachedGridIndex < m_Grids.GetCount(); ++uiCachedGridIndex)
  {
    auto& pGrid = m_Grids[uiCachedGridIndex];
    if (pGrid == nullptr || pGrid->CachingCompleted() == false)
      continue;

    if ((pGrid->m_Category.GetBitmask() & uiGridBitmask) == 0 ||
        pGrid->m_IncludeTags != queryParams.m_IncludeTags ||
        pGrid->m_ExcludeTags != queryParams.m_ExcludeTags)
      continue;

    uiGridBitmask &= ~pGrid->m_Category.GetBitmask();

    Stats stats;
    pGrid->ForEachCellInBox(box,
      [&](const Cell& cell) {
        return noFilterCallback(cell, queryParams, stats, pUserData);
      });

    UpdateCacheCandidate(queryParams.m_IncludeTags, queryParams.m_ExcludeTags, pGrid->m_Category, 0.0f);

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
    if (queryParams.m_pStats != nullptr)
    {
      queryParams.m_pStats->m_uiNumObjectsTested += stats.m_uiNumObjectsTested;
      queryParams.m_pStats->m_uiNumObjectsPassed += stats.m_uiNumObjectsPassed;
    }
#endif
  }

  // then search for the rest
  const bool useTagsFilter = queryParams.m_IncludeTags.IsEmpty() == false || queryParams.m_ExcludeTags.IsEmpty() == false;
  CellCallback cellCallback = useTagsFilter ? filterByTagsCallback : noFilterCallback;

  while (uiGridBitmask > 0)
  {
    wdUInt32 uiGridIndex = wdMath::FirstBitLow(uiGridBitmask);
    uiGridBitmask &= uiGridBitmask - 1;

    auto& pGrid = m_Grids[uiGridIndex];
    if (pGrid == nullptr)
      continue;

    Stats stats;
    pGrid->ForEachCellInBox(box,
      [&](const Cell& cell) {
        return cellCallback(cell, queryParams, stats, pUserData);
      });

    if (pGrid->m_bCanBeCached && useTagsFilter)
    {
      const wdUInt32 totalNumObjectsAfterSpatialTest = stats.m_uiNumObjectsFiltered + stats.m_uiNumObjectsPassed;
      const wdUInt32 cacheThreshold = wdUInt32(wdMath::Max(cvar_SpatialQueriesCachingThreshold.GetValue(), 1));

      // 1.0 => all objects filtered, 0.0 => no object filtered by tags
      const float filteredRatio = float(double(stats.m_uiNumObjectsFiltered) / totalNumObjectsAfterSpatialTest);

      // Doesn't make sense to cache if there are only few objects in total or only few objects have been filtered
      if (totalNumObjectsAfterSpatialTest > cacheThreshold && filteredRatio > 0.1f)
      {
        UpdateCacheCandidate(queryParams.m_IncludeTags, queryParams.m_ExcludeTags, pGrid->m_Category, filteredRatio);
      }
    }

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
    if (queryParams.m_pStats != nullptr)
    {
      queryParams.m_pStats->m_uiNumObjectsTested += stats.m_uiNumObjectsTested;
      queryParams.m_pStats->m_uiNumObjectsPassed += stats.m_uiNumObjectsPassed;
    }
#endif
  }
}

void wdSpatialSystem_RegularGrid::MigrateCachedGrid(wdUInt32 uiCandidateIndex)
{
  wdUInt32 uiTargetGridIndex = wdInvalidIndex;
  wdUInt32 uiSourceGridIndex = wdInvalidIndex;

  {
    WD_LOCK(m_CacheCandidatesMutex);

    auto& cacheCandidate = m_CacheCandidates[uiCandidateIndex];
    uiTargetGridIndex = cacheCandidate.m_uiGridIndex;
    uiSourceGridIndex = cacheCandidate.m_Category.m_uiValue;

    if (uiTargetGridIndex == wdInvalidIndex)
    {
      for (wdUInt32 i = m_Grids.GetCount() - 1; i >= MAX_NUM_REGULAR_GRIDS; --i)
      {
        if (m_Grids[i] == nullptr)
        {
          uiTargetGridIndex = i;
          break;
        }
      }

      WD_ASSERT_DEBUG(uiTargetGridIndex != wdInvalidIndex, "No free cached grid");
      cacheCandidate.m_uiGridIndex = uiTargetGridIndex;

      auto pGrid = WD_NEW(&m_Allocator, Grid, *this, cacheCandidate.m_Category);
      pGrid->m_IncludeTags = cacheCandidate.m_IncludeTags;
      pGrid->m_ExcludeTags = cacheCandidate.m_ExcludeTags;

      m_Grids[uiTargetGridIndex] = pGrid;

      m_uiFirstCachedGridIndex = wdMath::Min(m_uiFirstCachedGridIndex, uiTargetGridIndex);
    }
  }

  MigrateSpatialData(uiTargetGridIndex, uiSourceGridIndex);
}

void wdSpatialSystem_RegularGrid::MigrateSpatialData(wdUInt32 uiTargetGridIndex, wdUInt32 uiSourceGridIndex)
{
  auto& pTargetGrid = m_Grids[uiTargetGridIndex];
  if (pTargetGrid->CachingCompleted())
    return;

  auto& pSourceGrid = m_Grids[uiSourceGridIndex];

  constexpr wdUInt32 uiNumObjectsPerStep = 64;
  wdUInt32& uiLastMigrationIndex = pTargetGrid->m_uiLastMigrationIndex;
  const wdUInt32 uiSourceCount = pSourceGrid->m_CellDataMappings.GetCount();
  const wdUInt32 uiEndIndex = wdMath::Min(uiLastMigrationIndex + uiNumObjectsPerStep, uiSourceCount);

  for (wdUInt32 i = uiLastMigrationIndex; i < uiEndIndex; ++i)
  {
    if (pTargetGrid->MigrateSpatialDataFromOtherGrid(i, *pSourceGrid))
    {
      m_DataTable.GetValueUnchecked(i).m_uiGridBitmask |= WD_BIT(uiTargetGridIndex);
    }
  }

  uiLastMigrationIndex = (uiEndIndex == uiSourceCount) ? wdInvalidIndex : uiEndIndex;
}

void wdSpatialSystem_RegularGrid::RemoveCachedGrid(wdUInt32 uiCandidateIndex)
{
  wdUInt32 uiGridIndex;

  {
    WD_LOCK(m_CacheCandidatesMutex);

    auto& cacheCandidate = m_CacheCandidates[uiCandidateIndex];
    uiGridIndex = cacheCandidate.m_uiGridIndex;

    if (uiGridIndex == wdInvalidIndex)
      return;

    cacheCandidate.m_fQueryCount = 0.0f;
    cacheCandidate.m_fFilteredRatio = 0.0f;
    cacheCandidate.m_uiGridIndex = wdInvalidIndex;
  }

  m_Grids[uiGridIndex] = nullptr;
}

void wdSpatialSystem_RegularGrid::RemoveAllCachedGrids()
{
  WD_LOCK(m_CacheCandidatesMutex);

  for (wdUInt32 i = 0; i < m_CacheCandidates.GetCount(); ++i)
  {
    RemoveCachedGrid(i);
  }
}

void wdSpatialSystem_RegularGrid::UpdateCacheCandidate(const wdTagSet& includeTags, const wdTagSet& excludeTags, wdSpatialData::Category category, float filteredRatio) const
{
  WD_LOCK(m_CacheCandidatesMutex);

  CacheCandidate* pCacheCandiate = nullptr;
  for (auto& cacheCandidate : m_CacheCandidates)
  {
    if (cacheCandidate.m_Category == category &&
        cacheCandidate.m_IncludeTags == includeTags &&
        cacheCandidate.m_ExcludeTags == excludeTags)
    {
      pCacheCandiate = &cacheCandidate;
      break;
    }
  }

  if (pCacheCandiate != nullptr)
  {
    pCacheCandiate->m_fQueryCount = wdMath::Min(pCacheCandiate->m_fQueryCount + 1.0f, 100.0f);
    pCacheCandiate->m_fFilteredRatio = wdMath::Max(pCacheCandiate->m_fFilteredRatio, filteredRatio);
  }
  else
  {
    m_CacheCandidates.PushBack({includeTags, excludeTags, category, 1, filteredRatio});
  }
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_SpatialSystem_RegularGrid);
