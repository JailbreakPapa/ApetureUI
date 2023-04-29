#include <Core/CorePCH.h>

#include <Core/ResourceManager/Resource.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Profiling/Profiling.h>
#include <Foundation/System/StackTracer.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdResource, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

WD_CORE_DLL void IncreaseResourceRefCount(wdResource* pResource, const void* pOwner)
{
#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
  {
    WD_LOCK(pResource->m_HandleStackTraceMutex);

    auto& info = pResource->m_HandleStackTraces[pOwner];

    wdArrayPtr<void*> ptr(info.m_Ptrs);

    info.m_uiNumPtrs = wdStackTracer::GetStackTrace(ptr);
  }
#endif

  pResource->m_iReferenceCount.Increment();
}

WD_CORE_DLL void DecreaseResourceRefCount(wdResource* pResource, const void* pOwner)
{
#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
  {
    WD_LOCK(pResource->m_HandleStackTraceMutex);

    if (!pResource->m_HandleStackTraces.Remove(pOwner, nullptr))
    {
      WD_REPORT_FAILURE("No associated stack-trace!");
    }
  }
#endif

  pResource->m_iReferenceCount.Decrement();
}

#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
WD_CORE_DLL void MigrateResourceRefCount(wdResource* pResource, const void* pOldOwner, const void* pNewOwner)
{
  WD_LOCK(pResource->m_HandleStackTraceMutex);

  // allocate / resize the hash-table first to ensure the iterator stays valid
  auto& newInfo = pResource->m_HandleStackTraces[pNewOwner];

  auto it = pResource->m_HandleStackTraces.Find(pOldOwner);
  if (!it.IsValid())
  {
    WD_REPORT_FAILURE("No associated stack-trace!");
  }
  else
  {
    newInfo = it.Value();
    pResource->m_HandleStackTraces.Remove(it);
  }
}
#endif

wdResource::~wdResource()
{
  WD_ASSERT_DEV(!wdResourceManager::IsQueuedForLoading(this), "Cannot deallocate a resource while it is still qeued for loading");
}

wdResource::wdResource(DoUpdate ResourceUpdateThread, wdUInt8 uiQualityLevelsLoadable)
{
  m_Flags.AddOrRemove(wdResourceFlags::UpdateOnMainThread, ResourceUpdateThread == DoUpdate::OnMainThread);

  m_uiQualityLevelsLoadable = uiQualityLevelsLoadable;
}

#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
static void LogStackTrace(const char* szText)
{
  wdLog::Info(szText);
};
#endif

void wdResource::PrintHandleStackTraces()
{
#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)

  WD_LOCK(m_HandleStackTraceMutex);

  WD_LOG_BLOCK("Resource Handle Stack Traces");

  for (auto& it : m_HandleStackTraces)
  {
    WD_LOG_BLOCK("Handle Trace");

    wdStackTracer::ResolveStackTrace(wdArrayPtr<void*>(it.Value().m_Ptrs, it.Value().m_uiNumPtrs), LogStackTrace);
  }

#else

  wdLog::Warning("Compile with WD_RESOURCEHANDLE_STACK_TRACES set to WD_ON to enable support for resource handle stack traces.");

#endif
}

void wdResource::SetResourceDescription(wdStringView sDescription)
{
  m_sResourceDescription = sDescription;
}

void wdResource::SetUniqueID(wdStringView sUniqueID, bool bIsReloadable)
{
  m_sUniqueID = sUniqueID;
  m_uiUniqueIDHash = wdHashingUtils::StringHash(sUniqueID);
  SetIsReloadable(bIsReloadable);

  wdResourceEvent e;
  e.m_pResource = this;
  e.m_Type = wdResourceEvent::Type::ResourceCreated;
  wdResourceManager::BroadcastResourceEvent(e);
}

void wdResource::CallUnloadData(Unload WhatToUnload)
{
  WD_LOG_BLOCK("wdResource::UnloadData", GetResourceID().GetData());

  wdResourceEvent e;
  e.m_pResource = this;
  e.m_Type = wdResourceEvent::Type::ResourceContentUnloading;
  wdResourceManager::BroadcastResourceEvent(e);

  wdResourceLoadDesc ld = UnloadData(WhatToUnload);

  WD_ASSERT_DEV(ld.m_State != wdResourceState::Invalid, "UnloadData() did not return a valid resource load state");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsDiscardable != 0xFF, "UnloadData() did not fill out m_uiQualityLevelsDiscardable correctly");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsLoadable != 0xFF, "UnloadData() did not fill out m_uiQualityLevelsLoadable correctly");

  m_LoadingState = ld.m_State;
  m_uiQualityLevelsDiscardable = ld.m_uiQualityLevelsDiscardable;
  m_uiQualityLevelsLoadable = ld.m_uiQualityLevelsLoadable;
}

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
thread_local const wdResource* g_pCurrentlyUpdatingContent = nullptr;

const wdResource* wdResource::GetCurrentlyUpdatingContent()
{
  return g_pCurrentlyUpdatingContent;
}
#endif

void wdResource::CallUpdateContent(wdStreamReader* Stream)
{
  WD_PROFILE_SCOPE("CallUpdateContent");

  WD_LOG_BLOCK("wdResource::UpdateContent", GetResourceID().GetData());

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  const wdResource* pPreviouslyUpdatingContent = g_pCurrentlyUpdatingContent;
  g_pCurrentlyUpdatingContent = this;
  wdResourceLoadDesc ld = UpdateContent(Stream);
  g_pCurrentlyUpdatingContent = pPreviouslyUpdatingContent;
#else
  wdResourceLoadDesc ld = UpdateContent(Stream);
#endif

  WD_ASSERT_DEV(ld.m_State != wdResourceState::Invalid, "UpdateContent() did not return a valid resource load state");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsDiscardable != 0xFF, "UpdateContent() did not fill out m_uiQualityLevelsDiscardable correctly");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsLoadable != 0xFF, "UpdateContent() did not fill out m_uiQualityLevelsLoadable correctly");

  if (ld.m_State == wdResourceState::LoadedResourceMissing)
  {
    ReportResourceIsMissing();
  }

  IncResourceChangeCounter();

  m_uiQualityLevelsDiscardable = ld.m_uiQualityLevelsDiscardable;
  m_uiQualityLevelsLoadable = ld.m_uiQualityLevelsLoadable;
  m_LoadingState = ld.m_State;

  wdResourceEvent e;
  e.m_pResource = this;
  e.m_Type = wdResourceEvent::Type::ResourceContentUpdated;
  wdResourceManager::BroadcastResourceEvent(e);

  wdLog::Debug("Updated {0} - '{1}'", GetDynamicRTTI()->GetTypeName(), wdArgSensitive(GetResourceDescription(), "ResourceDesc"));
}

float wdResource::GetLoadingPriority(wdTime now) const
{
  if (m_Priority == wdResourcePriority::Critical)
    return 0.0f;

  // low priority values mean it gets loaded earlier
  float fPriority = static_cast<float>(m_Priority) * 10.0f;

  if (GetLoadingState() == wdResourceState::Loaded)
  {
    // already loaded -> more penalty
    fPriority += 30.0f;

    // the more it could discard, the less important it is to load more of it
    fPriority += GetNumQualityLevelsDiscardable() * 10.0f;
  }
  else
  {
    const wdBitflags<wdResourceFlags> flags = GetBaseResourceFlags();

    if (flags.IsAnySet(wdResourceFlags::ResourceHasFallback))
    {
      // if the resource has a very specific fallback, it is least important to be get loaded
      fPriority += 20.0f;
    }
    else if (flags.IsAnySet(wdResourceFlags::ResourceHasTypeFallback))
    {
      // if it has at least a type fallback, it is less important to get loaded
      fPriority += 10.0f;
    }
  }

  // everything acquired in the last N seconds gets a higher priority
  // by getting the lowest penalty
  const float secondsSinceAcquire = (float)(now - GetLastAcquireTime()).GetSeconds();
  const float fTimePriority = wdMath::Min(10.0f, secondsSinceAcquire);

  return fPriority + fTimePriority;
}

void wdResource::SetPriority(wdResourcePriority priority)
{
  if (m_Priority == priority)
    return;

  m_Priority = priority;

  wdResourceEvent e;
  e.m_pResource = this;
  e.m_Type = wdResourceEvent::Type::ResourcePriorityChanged;
  wdResourceManager::BroadcastResourceEvent(e);
}

wdResourceTypeLoader* wdResource::GetDefaultResourceTypeLoader() const
{
  return wdResourceManager::GetDefaultResourceLoader();
}

void wdResource::ReportResourceIsMissing()
{
  wdLog::SeriousWarning("Missing Resource of Type '{2}': '{0}' ('{1}')", wdArgSensitive(GetResourceID(), "ResourceID"),
    wdArgSensitive(m_sResourceDescription, "ResourceDesc"), GetDynamicRTTI()->GetTypeName());
}

void wdResource::VerifyAfterCreateResource(const wdResourceLoadDesc& ld)
{
  WD_ASSERT_DEV(ld.m_State != wdResourceState::Invalid, "CreateResource() did not return a valid resource load state");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsDiscardable != 0xFF, "CreateResource() did not fill out m_uiQualityLevelsDiscardable correctly");
  WD_ASSERT_DEV(ld.m_uiQualityLevelsLoadable != 0xFF, "CreateResource() did not fill out m_uiQualityLevelsLoadable correctly");

  IncResourceChangeCounter();

  m_LoadingState = ld.m_State;
  m_uiQualityLevelsDiscardable = ld.m_uiQualityLevelsDiscardable;
  m_uiQualityLevelsLoadable = ld.m_uiQualityLevelsLoadable;

  /* Update Memory Usage*/
  {
    wdResource::MemoryUsage MemUsage;
    MemUsage.m_uiMemoryCPU = 0xFFFFFFFF;
    MemUsage.m_uiMemoryGPU = 0xFFFFFFFF;
    UpdateMemoryUsage(MemUsage);

    WD_ASSERT_DEV(MemUsage.m_uiMemoryCPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its CPU memory usage", GetResourceID());
    WD_ASSERT_DEV(MemUsage.m_uiMemoryGPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its GPU memory usage", GetResourceID());

    m_MemoryUsage = MemUsage;
  }

  wdResourceEvent e;
  e.m_pResource = this;
  e.m_Type = wdResourceEvent::Type::ResourceContentUpdated;
  wdResourceManager::BroadcastResourceEvent(e);

  wdLog::Debug("Created {0} - '{1}' ", GetDynamicRTTI()->GetTypeName(), wdArgSensitive(GetResourceDescription(), "ResourceDesc"));
}

WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_Resource);
