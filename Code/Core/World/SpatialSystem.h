#pragma once

#include <Core/World/SpatialData.h>
#include <Foundation/Math/Frustum.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/SimdMath/SimdBBoxSphere.h>
#include <Foundation/Types/TagSet.h>

class WD_CORE_DLL wdSpatialSystem : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdSpatialSystem, wdReflectedClass);

public:
  wdSpatialSystem();
  ~wdSpatialSystem();

  virtual void StartNewFrame();

  /// \name Spatial Data Functions
  ///@{

  virtual wdSpatialDataHandle CreateSpatialData(const wdSimdBBoxSphere& bounds, wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags) = 0;
  virtual wdSpatialDataHandle CreateSpatialDataAlwaysVisible(wdGameObject* pObject, wdUInt32 uiCategoryBitmask, const wdTagSet& tags) = 0;

  virtual void DeleteSpatialData(const wdSpatialDataHandle& hData) = 0;

  virtual void UpdateSpatialDataBounds(const wdSpatialDataHandle& hData, const wdSimdBBoxSphere& bounds) = 0;
  virtual void UpdateSpatialDataObject(const wdSpatialDataHandle& hData, wdGameObject* pObject) = 0;

  ///@}
  /// \name Simple Queries
  ///@{

  typedef wdDelegate<wdVisitorExecution::Enum(wdGameObject*)> QueryCallback;

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  struct QueryStats
  {
    wdUInt32 m_uiTotalNumObjects = 0;  ///< The total number of spatial objects in this system.
    wdUInt32 m_uiNumObjectsTested = 0; ///< Number of objects tested for the query condition.
    wdUInt32 m_uiNumObjectsPassed = 0; ///< Number of objects that passed the query condition.
    wdTime m_TimeTaken;                ///< Time taken to execute the query
  };
#endif

  struct QueryParams
  {
    wdUInt32 m_uiCategoryBitmask = 0;
    wdTagSet m_IncludeTags;
    wdTagSet m_ExcludeTags;
#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
    QueryStats* m_pStats = nullptr;
#endif
  };

  virtual void FindObjectsInSphere(const wdBoundingSphere& sphere, const QueryParams& queryParams, wdDynamicArray<wdGameObject*>& out_objects) const;
  virtual void FindObjectsInSphere(const wdBoundingSphere& sphere, const QueryParams& queryParams, QueryCallback callback) const = 0;

  virtual void FindObjectsInBox(const wdBoundingBox& box, const QueryParams& queryParams, wdDynamicArray<wdGameObject*>& out_objects) const;
  virtual void FindObjectsInBox(const wdBoundingBox& box, const QueryParams& queryParams, QueryCallback callback) const = 0;

  ///@}
  /// \name Visibility Queries
  ///@{

  using IsOccludedFunc = wdDelegate<bool(const wdSimdBBox&)>;

  virtual void FindVisibleObjects(const wdFrustum& frustum, const QueryParams& queryParams, wdDynamicArray<const wdGameObject*>& out_objects, IsOccludedFunc isOccluded) const = 0;

  virtual wdUInt64 GetNumFramesSinceVisible(const wdSpatialDataHandle& hData) const = 0;

  ///@}

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  virtual void GetInternalStats(wdStringBuilder& ref_sSb) const;
#endif

protected:
  wdProxyAllocator m_Allocator;

  wdUInt64 m_uiFrameCounter = 0;
};
