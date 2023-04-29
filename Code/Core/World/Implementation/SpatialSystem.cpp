#include <Core/CorePCH.h>

#include <Core/World/SpatialSystem.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdSpatialSystem, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdSpatialSystem::wdSpatialSystem()
  : m_Allocator("Spatial System", wdFoundation::GetDefaultAllocator())
{
}

wdSpatialSystem::~wdSpatialSystem() = default;

void wdSpatialSystem::StartNewFrame()
{
  ++m_uiFrameCounter;
}

void wdSpatialSystem::FindObjectsInSphere(const wdBoundingSphere& sphere, const QueryParams& queryParams, wdDynamicArray<wdGameObject*>& out_objects) const
{
  out_objects.Clear();

  FindObjectsInSphere(
    sphere, queryParams,
    [&](wdGameObject* pObject) {
      out_objects.PushBack(pObject);

      return wdVisitorExecution::Continue;
    });
}

void wdSpatialSystem::FindObjectsInBox(const wdBoundingBox& box, const QueryParams& queryParams, wdDynamicArray<wdGameObject*>& out_objects) const
{
  out_objects.Clear();

  FindObjectsInBox(
    box, queryParams,
    [&](wdGameObject* pObject) {
      out_objects.PushBack(pObject);

      return wdVisitorExecution::Continue;
    });
}

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
void wdSpatialSystem::GetInternalStats(wdStringBuilder& ref_sSb) const
{
  ref_sSb.Clear();
}
#endif

WD_STATICLINK_FILE(Core, Core_World_Implementation_SpatialSystem);
