#pragma once

#include <Core/World/SpatialSystem.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/SimdMath/SimdVec4i.h>
#include <Foundation/Types/UniquePtr.h>

namespace wdInternal
{
  struct QueryHelper;
}

class WD_CORE_DLL wdSpatialSystem_RegularGrid : public wdSpatialSystem
{
  WD_ADD_DYNAMIC_REFLECTION(wdSpatialSystem_RegularGrid, wdSpatialSystem);

public:
  wdSpatialSystem_RegularGrid(wdUInt32 uiCellSize = 128);
  ~wdSpatialSystem_RegularGrid();

  /// \brief Returns the bounding box of the cell associated with the given spatial data. Useful for debug visualizations.
  wdResult GetCellBoxForSpatialData(const wdSpatialDataHandle& hData, wdBoundingBox& out_boundingBox) const;

  /// \brief Returns bounding boxes of all existing cells.
  void GetAllCellBoxes(wdDynamicArray<wdBoundingBox>& out_boundingBoxes, wdSpatialData::Category filterCategory = wdInvalidSpatialDataCategory) const;

private:
  friend wdInternal::QueryHelper;

  // wdSpatialSystem implementation
  virtual void StartNewFrame() override;

  wdSpatialDataHandle CreateSpatialData(const wdSimdBBoxSphere& bounds, wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags) override;
  wdSpatialDataHandle CreateSpatialDataAlwaysVisible(wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags) override;

  void DeleteSpatialData(const wdSpatialDataHandle& hData) override;

  void UpdateSpatialDataBounds(const wdSpatialDataHandle& hData, const wdSimdBBoxSphere& bounds) override;
  void UpdateSpatialDataObject(const wdSpatialDataHandle& hData, wdGameObject* pObject) override;

  void FindObjectsInSphere(const wdBoundingSphere& sphere, const QueryParams& queryParams, QueryCallback callback) const override;
  void FindObjectsInBox(const wdBoundingBox& box, const QueryParams& queryParams, QueryCallback callback) const override;

  void FindVisibleObjects(const wdFrustum& frustum, const QueryParams& queryParams, wdDynamicArray<const wdGameObject*>& out_Objects, wdSpatialSystem::IsOccludedFunc IsOccluded) const override;

  wdUInt64 GetNumFramesSinceVisible(const wdSpatialDataHandle& hData) const override;

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  virtual void GetInternalStats(wdStringBuilder& sb) const override;
#endif

  wdProxyAllocator m_AlignedAllocator;

  wdSimdVec4i m_vCellSize;
  wdSimdVec4f m_vOverlapSize;
  wdSimdFloat m_fInvCellSize;

  enum
  {
    MAX_NUM_GRIDS = 63,
    MAX_NUM_REGULAR_GRIDS = (sizeof(wdSpatialData::Category::m_uiValue) * 8),
    MAX_NUM_CACHED_GRIDS = MAX_NUM_GRIDS - MAX_NUM_REGULAR_GRIDS
  };

  struct Cell;
  struct Grid;
  wdDynamicArray<wdUniquePtr<Grid>> m_Grids;
  wdUInt32 m_uiFirstCachedGridIndex = MAX_NUM_GRIDS;

  struct Data
  {
    WD_DECLARE_POD_TYPE();

    wdUInt64 m_uiGridBitmask : MAX_NUM_GRIDS;
    wdUInt64 m_uiAlwaysVisible : 1;
  };

  wdIdTable<wdSpatialDataId, Data, wdLocalAllocatorWrapper> m_DataTable;

  bool IsAlwaysVisibleData(const Data& data) const;

  wdSpatialDataHandle AddSpatialDataToGrids(const wdSimdBBoxSphere& bounds, wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags, bool bAlwaysVisible);

  template <typename Functor>
  void ForEachGrid(const Data& data, const wdSpatialDataHandle& hData, Functor func) const;

  struct Stats;
  using CellCallback = wdDelegate<wdVisitorExecution::Enum(const Cell&, const QueryParams&, Stats&, void*)>;
  void ForEachCellInBoxInMatchingGrids(const wdSimdBBox& box, const QueryParams& queryParams, CellCallback noFilterCallback, CellCallback filterByTagsCallback, void* pUserData) const;

  struct CacheCandidate
  {
    wdTagSet m_IncludeTags;
    wdTagSet m_ExcludeTags;
    wdSpatialData::Category m_Category;
    float m_fQueryCount = 0.0f;
    float m_fFilteredRatio = 0.0f;
    wdUInt32 m_uiGridIndex = wdInvalidIndex;
  };

  mutable wdDynamicArray<CacheCandidate> m_CacheCandidates;
  mutable wdMutex m_CacheCandidatesMutex;

  struct SortedCacheCandidate
  {
    wdUInt32 m_uiIndex = 0;
    float m_fScore = 0;

    bool operator<(const SortedCacheCandidate& other) const
    {
      if (m_fScore != other.m_fScore)
        return m_fScore > other.m_fScore; // higher score comes first

      return m_uiIndex < other.m_uiIndex;
    }
  };

  wdDynamicArray<SortedCacheCandidate> m_SortedCacheCandidates;

  void MigrateCachedGrid(wdUInt32 uiCandidateIndex);
  void MigrateSpatialData(wdUInt32 uiTargetGridIndex, wdUInt32 uiSourceGridIndex);

  void RemoveCachedGrid(wdUInt32 uiCandidateIndex);
  void RemoveAllCachedGrids();

  void UpdateCacheCandidate(const wdTagSet& includeTags, const wdTagSet& excludeTags, wdSpatialData::Category category, float filteredRatio) const;
};
