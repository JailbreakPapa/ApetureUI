#include <Core/CorePCH.h>

#include <Core/ResourceManager/Implementation/ResourceManagerState.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Communication/GlobalEvent.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Profiling/Profiling.h>

/// \todo Do not unload resources while they are acquired
/// \todo Resource Type Memory Thresholds
/// \todo Preload does not load all quality levels

/// Infos to Display:
///   Ref Count (max)
///   Fallback: Type / Instance
///   Loading Time

/// Resource Flags:
/// Category / Group (Texture Sets)

/// Resource Loader
///   Requires No File Access -> on non-File Thread

wdUniquePtr<wdResourceManagerState> wdResourceManager::s_pState;
wdMutex wdResourceManager::s_ResourceMutex;

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, ResourceManager)

  BEGIN_SUBSYSTEM_DEPENDENCIES
  "Foundation"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    wdResourceManager::OnCoreStartup();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    wdResourceManager::OnCoreShutdown();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
    wdResourceManager::OnEngineShutdown();
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on


wdResourceTypeLoader* wdResourceManager::GetResourceTypeLoader(const wdRTTI* pRTTI)
{
  return s_pState->m_ResourceTypeLoader[pRTTI];
}

wdMap<const wdRTTI*, wdResourceTypeLoader*>& wdResourceManager::GetResourceTypeLoaders()
{
  return s_pState->m_ResourceTypeLoader;
}

void wdResourceManager::AddResourceCleanupCallback(ResourceCleanupCB cb)
{
  WD_ASSERT_DEV(cb.IsComparable(), "Delegates with captures are not allowed");

  for (wdUInt32 i = 0; i < s_pState->m_ResourceCleanupCallbacks.GetCount(); ++i)
  {
    if (s_pState->m_ResourceCleanupCallbacks[i].IsEqualIfComparable(cb))
      return;
  }

  s_pState->m_ResourceCleanupCallbacks.PushBack(cb);
}

void wdResourceManager::ClearResourceCleanupCallback(ResourceCleanupCB cb)
{
  for (wdUInt32 i = 0; i < s_pState->m_ResourceCleanupCallbacks.GetCount(); ++i)
  {
    if (s_pState->m_ResourceCleanupCallbacks[i].IsEqualIfComparable(cb))
    {
      s_pState->m_ResourceCleanupCallbacks.RemoveAtAndSwap(i);
      return;
    }
  }
}

void wdResourceManager::ExecuteAllResourceCleanupCallbacks()
{
  if (s_pState == nullptr)
  {
    // In case resource manager wasn't initialized, nothing to do
    return;
  }

  wdDynamicArray<ResourceCleanupCB> callbacks = s_pState->m_ResourceCleanupCallbacks;
  s_pState->m_ResourceCleanupCallbacks.Clear();

  for (auto& cb : callbacks)
  {
    cb();
  }

  WD_ASSERT_DEV(s_pState->m_ResourceCleanupCallbacks.IsEmpty(), "During resource cleanup, new resource cleanup callbacks were registered.");
}

wdMap<const wdRTTI*, wdResourcePriority>& wdResourceManager::GetResourceTypePriorities()
{
  return s_pState->m_ResourceTypePriorities;
}

void wdResourceManager::BroadcastResourceEvent(const wdResourceEvent& e)
{
  WD_LOCK(s_ResourceMutex);

  // broadcast it through the resource to everyone directly interested in that specific resource
  e.m_pResource->m_ResourceEvents.Broadcast(e);

  // and then broadcast it to everyone else through the general event
  s_pState->m_ResourceEvents.Broadcast(e);
}

void wdResourceManager::RegisterResourceForAssetType(wdStringView sAssetTypeName, const wdRTTI* pResourceType)
{
  wdStringBuilder s = sAssetTypeName;
  s.ToLower();

  s_pState->m_AssetToResourceType[s] = pResourceType;
}

const wdRTTI* wdResourceManager::FindResourceForAssetType(wdStringView sAssetTypeName)
{
  wdStringBuilder s = sAssetTypeName;
  s.ToLower();

  return s_pState->m_AssetToResourceType.GetValueOrDefault(s, nullptr);
}

void wdResourceManager::ForceNoFallbackAcquisition(wdUInt32 uiNumFrames /*= 0xFFFFFFFF*/)
{
  s_pState->m_uiForceNoFallbackAcquisition = wdMath::Max(s_pState->m_uiForceNoFallbackAcquisition, uiNumFrames);
}

wdUInt32 wdResourceManager::FreeAllUnusedResources()
{
  WD_LOG_BLOCK("wdResourceManager::FreeAllUnusedResources");

  WD_PROFILE_SCOPE("FreeAllUnusedResources");

  if (s_pState == nullptr)
  {
    // In case resource manager wasn't initialized, no resources to unload
    return 0;
  }

  const bool bFreeAllUnused = true;

  wdUInt32 uiUnloaded = 0;
  bool bUnloadedAny = false;
  bool bAnyFailed = false;

  do
  {
    {
      WD_LOCK(s_ResourceMutex);

      bUnloadedAny = false;

      for (auto itType = s_pState->m_LoadedResources.GetIterator(); itType.IsValid(); ++itType)
      {
        const wdRTTI* pRtti = itType.Key();
        LoadedResources& lr = itType.Value();

        for (auto it = lr.m_Resources.GetIterator(); it.IsValid(); /* empty */)
        {
          wdResource* pReference = it.Value();

          if (pReference->m_iReferenceCount == 0)
          {
            bUnloadedAny = true; // make sure to try again, even if DeallocateResource() fails; need to release our lock for that to prevent dead-locks

            if (DeallocateResource(pReference).Succeeded())
            {
              ++uiUnloaded;

              it = lr.m_Resources.Remove(it);
              continue;
            }
            else
            {
              bAnyFailed = true;
            }
          }

          ++it;
        }
      }
    }

    if (bAnyFailed)
    {
      // When this happens, it is possible that the resource that failed to be deleted
      // is dependent on a task that needs to be executed on THIS thread (main thread).
      // Therefore, help executing some tasks here, to unblock the task system.

      bAnyFailed = false;

      wdInt32 iHelpExecTasksRounds = 1;
      wdTaskSystem::WaitForCondition([&iHelpExecTasksRounds]() { return iHelpExecTasksRounds-- <= 0; });
    }

  } while (bFreeAllUnused && bUnloadedAny);

  return uiUnloaded;
}

wdUInt32 wdResourceManager::FreeUnusedResources(wdTime timeout, wdTime lastAcquireThreshold)
{
  if (timeout.IsZeroOrNegative())
    return 0;

  WD_LOCK(s_ResourceMutex);
  WD_LOG_BLOCK("wdResourceManager::FreeUnusedResources");
  WD_PROFILE_SCOPE("FreeUnusedResources");

  auto itResourceType = s_pState->m_LoadedResources.Find(s_pState->m_pFreeUnusedLastType);
  if (!itResourceType.IsValid())
  {
    itResourceType = s_pState->m_LoadedResources.GetIterator();
  }

  if (!itResourceType.IsValid())
    return 0;

  auto itResourceID = itResourceType.Value().m_Resources.Find(s_pState->m_sFreeUnusedLastResourceID);
  if (!itResourceID.IsValid())
  {
    itResourceID = itResourceType.Value().m_Resources.GetIterator();
  }

  const wdTime tStart = wdTime::Now();

  wdUInt32 uiDeallocatedCount = 0;

  wdStringBuilder sResourceName;

  const wdRTTI* pLastTypeCheck = nullptr;

  // stop once we wasted enough time
  while (wdTime::Now() - tStart < timeout)
  {
    if (!itResourceID.IsValid())
    {
      // reached the end of this resource type
      // advance to the next resource type
      ++itResourceType;

      if (!itResourceType.IsValid())
      {
        // if we reached the end, reset everything and stop

        s_pState->m_pFreeUnusedLastType = nullptr;
        s_pState->m_sFreeUnusedLastResourceID = wdTempHashedString();
        return uiDeallocatedCount;
      }


      // reset resource ID to the beginning of this type and start over
      itResourceID = itResourceType.Value().m_Resources.GetIterator();
      continue;
    }

    s_pState->m_pFreeUnusedLastType = itResourceType.Key();
    s_pState->m_sFreeUnusedLastResourceID = itResourceID.Key();

    if (pLastTypeCheck != itResourceType.Key())
    {
      pLastTypeCheck = itResourceType.Key();

      if (GetResourceTypeInfo(pLastTypeCheck).m_bIncrementalUnload == false)
      {
        itResourceID = itResourceType.Value().m_Resources.GetEndIterator();
        continue;
      }
    }

    wdResource* pResource = itResourceID.Value();

    if ((pResource->GetReferenceCount() == 0) && (tStart - pResource->GetLastAcquireTime() > lastAcquireThreshold))
    {
      sResourceName = pResource->GetResourceID();

      if (DeallocateResource(pResource).Succeeded())
      {
        wdLog::Debug("Freed '{}'", wdArgSensitive(sResourceName, "ResourceID"));

        ++uiDeallocatedCount;
        itResourceID = itResourceType.Value().m_Resources.Remove(itResourceID);
        continue;
      }
    }

    ++itResourceID;
  }

  return uiDeallocatedCount;
}

void wdResourceManager::SetAutoFreeUnused(wdTime timeout, wdTime lastAcquireThreshold)
{
  s_pState->m_AutoFreeUnusedTimeout = timeout;
  s_pState->m_AutoFreeUnusedThreshold = lastAcquireThreshold;
}

void wdResourceManager::AllowResourceTypeAcquireDuringUpdateContent(const wdRTTI* pTypeBeingUpdated, const wdRTTI* pTypeItWantsToAcquire)
{
  auto& info = s_pState->m_TypeInfo[pTypeBeingUpdated];

  WD_ASSERT_DEV(info.m_bAllowNestedAcquireCached == false, "AllowResourceTypeAcquireDuringUpdateContent for type '{}' must be called before the resource info has been requested.", pTypeBeingUpdated->GetTypeName());

  if (info.m_NestedTypes.IndexOf(pTypeItWantsToAcquire) == wdInvalidIndex)
  {
    info.m_NestedTypes.PushBack(pTypeItWantsToAcquire);
  }
}

bool wdResourceManager::IsResourceTypeAcquireDuringUpdateContentAllowed(const wdRTTI* pTypeBeingUpdated, const wdRTTI* pTypeItWantsToAcquire)
{
  WD_ASSERT_DEBUG(s_ResourceMutex.IsLocked(), "");

  auto& info = s_pState->m_TypeInfo[pTypeBeingUpdated];

  if (!info.m_bAllowNestedAcquireCached)
  {
    info.m_bAllowNestedAcquireCached = true;

    wdSet<const wdRTTI*> visited;
    wdSet<const wdRTTI*> todo;
    wdSet<const wdRTTI*> deps;

    for (const wdRTTI* pRtti : info.m_NestedTypes)
    {
      wdHybridArray<const wdRTTI*, 16> derived;
      wdRTTI::GetAllTypesDerivedFrom(pRtti, derived, false);

      for (const wdRTTI* pDerived : derived)
      {
        todo.Insert(pDerived);
      }
    }

    while (!todo.IsEmpty())
    {
      auto it = todo.GetIterator();
      const wdRTTI* pRtti = it.Key();
      todo.Remove(it);

      if (visited.Contains(pRtti))
        continue;

      visited.Insert(pRtti);
      deps.Insert(pRtti);

      for (const wdRTTI* pNestedRtti : s_pState->m_TypeInfo[pRtti].m_NestedTypes)
      {
        if (!visited.Contains(pNestedRtti))
        {
          wdHybridArray<const wdRTTI*, 16> derived;
          wdRTTI::GetAllTypesDerivedFrom(pNestedRtti, derived, false);

          for (const wdRTTI* pDerived : derived)
          {
            todo.Insert(pDerived);
          }
        }
      }
    }

    info.m_NestedTypes.Clear();
    for (const wdRTTI* pRtti : deps)
    {
      info.m_NestedTypes.PushBack(pRtti);
    }
    info.m_NestedTypes.Sort();
  }

  return info.m_NestedTypes.IndexOf(pTypeItWantsToAcquire) != wdInvalidIndex;
}

wdResult wdResourceManager::DeallocateResource(wdResource* pResource)
{
  //WD_ASSERT_DEBUG(pResource->m_iLockCount == 0, "Resource '{0}' has a refcount of zero, but is still in an acquired state.", pResource->GetResourceID());

  if (RemoveFromLoadingQueue(pResource).Failed())
  {
    // cannot deallocate resources that are currently queued for loading,
    // especially when they are already picked up by a task
    return WD_FAILURE;
  }

  pResource->CallUnloadData(wdResource::Unload::AllQualityLevels);

  WD_ASSERT_DEBUG(pResource->GetLoadingState() <= wdResourceState::LoadedResourceMissing, "Resource '{0}' should be in an unloaded state now.", pResource->GetResourceID());

  // broadcast that we are going to delete the resource
  {
    wdResourceEvent e;
    e.m_pResource = pResource;
    e.m_Type = wdResourceEvent::Type::ResourceDeleted;
    wdResourceManager::BroadcastResourceEvent(e);
  }

  // delete the resource via the RTTI provided allocator
  pResource->GetDynamicRTTI()->GetAllocator()->Deallocate(pResource);

  return WD_SUCCESS;
}

// To allow triggering this event without a link dependency
// Used by Fileserve, to trigger this event, even though Fileserve should not have a link dependency on Core
WD_ON_GLOBAL_EVENT(wdResourceManager_ReloadAllResources)
{
  wdResourceManager::ReloadAllResources(false);
}
void wdResourceManager::ResetAllResources()
{
  WD_LOCK(s_ResourceMutex);
  WD_LOG_BLOCK("wdResourceManager::ReloadAllResources");

  for (auto itType = s_pState->m_LoadedResources.GetIterator(); itType.IsValid(); ++itType)
  {
    for (auto it = itType.Value().m_Resources.GetIterator(); it.IsValid(); ++it)
    {
      wdResource* pResource = it.Value();
      pResource->ResetResource();
    }
  }
}

void wdResourceManager::PerFrameUpdate()
{
  WD_PROFILE_SCOPE("wdResourceManagerUpdate");

  s_pState->m_LastFrameUpdate = wdTime::Now();

  if (s_pState->m_bBroadcastExistsEvent)
  {
    WD_LOCK(s_ResourceMutex);

    s_pState->m_bBroadcastExistsEvent = false;

    for (auto itType = s_pState->m_LoadedResources.GetIterator(); itType.IsValid(); ++itType)
    {
      for (auto it = itType.Value().m_Resources.GetIterator(); it.IsValid(); ++it)
      {
        wdResourceEvent e;
        e.m_Type = wdResourceEvent::Type::ResourceExists;
        e.m_pResource = it.Value();

        wdResourceManager::BroadcastResourceEvent(e);
      }
    }
  }

  {
    WD_LOCK(s_ResourceMutex);

    for (auto it = s_pState->m_ResourcesToUnloadOnMainThread.GetIterator(); it.IsValid(); it.Next())
    {
      // Identify the container of loaded resource for the type of resource we want to unload.
      LoadedResources loadedResourcesForType;
      if (s_pState->m_LoadedResources.TryGetValue(it.Value(), loadedResourcesForType) == false)
      {
        continue;
      }

      // See, if the resource we want to unload still exists.
      wdResource* resourceToUnload = nullptr;

      if (loadedResourcesForType.m_Resources.TryGetValue(it.Key(), resourceToUnload) == false)
      {
        continue;
      }

      WD_ASSERT_DEV(resourceToUnload != nullptr, "Found a resource above, should not be nullptr.");

      // If the resource was still loaded, we are going to unload it now.
      resourceToUnload->CallUnloadData(wdResource::Unload::AllQualityLevels);

      WD_ASSERT_DEV(resourceToUnload->GetLoadingState() <= wdResourceState::LoadedResourceMissing, "Resource '{0}' should be in an unloaded state now.", resourceToUnload->GetResourceID());
    }

    s_pState->m_ResourcesToUnloadOnMainThread.Clear();
  }

  if (s_pState->m_AutoFreeUnusedTimeout.IsPositive())
  {
    FreeUnusedResources(s_pState->m_AutoFreeUnusedTimeout, s_pState->m_AutoFreeUnusedThreshold);
  }
}

const wdEvent<const wdResourceEvent&, wdMutex>& wdResourceManager::GetResourceEvents()
{
  return s_pState->m_ResourceEvents;
}

const wdEvent<const wdResourceManagerEvent&, wdMutex>& wdResourceManager::GetManagerEvents()
{
  return s_pState->m_ManagerEvents;
}

void wdResourceManager::BroadcastExistsEvent()
{
  s_pState->m_bBroadcastExistsEvent = true;
}

void wdResourceManager::PluginEventHandler(const wdPluginEvent& e)
{
  switch (e.m_EventType)
  {
    case wdPluginEvent::AfterStartupShutdown:
    {
      // unload all resources until there are no more that can be unloaded
      // this is to prevent having resources allocated that came from a dynamic plugin
      FreeAllUnusedResources();
    }
    break;

    default:
      break;
  }
}

void wdResourceManager::OnCoreStartup()
{
  s_pState = WD_DEFAULT_NEW(wdResourceManagerState);

  WD_LOCK(s_ResourceMutex);
  s_pState->m_bAllowLaunchDataLoadTask = true;
  s_pState->m_bShutdown = false;

  wdPlugin::Events().AddEventHandler(PluginEventHandler);
}

void wdResourceManager::EngineAboutToShutdown()
{
  {
    WD_LOCK(s_ResourceMutex);

    if (s_pState == nullptr)
    {
      // In case resource manager wasn't initialized, nothing to do
      return;
    }

    s_pState->m_bAllowLaunchDataLoadTask = false; // prevent a new one from starting
    s_pState->m_bShutdown = true;
  }

  for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksDataLoad.GetCount(); ++i)
  {
    wdTaskSystem::CancelTask(s_pState->m_WorkerTasksDataLoad[i].m_pTask).IgnoreResult();
  }

  for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksUpdateContent.GetCount(); ++i)
  {
    wdTaskSystem::CancelTask(s_pState->m_WorkerTasksUpdateContent[i].m_pTask).IgnoreResult();
  }

  {
    WD_LOCK(s_ResourceMutex);

    for (auto entry : s_pState->m_LoadingQueue)
    {
      entry.m_pResource->m_Flags.Remove(wdResourceFlags::IsQueuedForLoading);
    }

    s_pState->m_LoadingQueue.Clear();

    // Since we just canceled all loading tasks above and cleared the loading queue,
    // some resources may still be flagged as 'loading', but can never get loaded.
    // That can deadlock the 'FreeAllUnused' function, because it won't delete 'loading' resources.
    // Therefore we need to make sure no resource has the IsQueuedForLoading flag set anymore.
    for (auto itTypes : s_pState->m_LoadedResources)
    {
      for (auto itRes : itTypes.Value().m_Resources)
      {
        wdResource* pRes = itRes.Value();

        if (pRes->GetBaseResourceFlags().IsSet(wdResourceFlags::IsQueuedForLoading))
        {
          pRes->m_Flags.Remove(wdResourceFlags::IsQueuedForLoading);
        }
      }
    }
  }
}

bool wdResourceManager::IsAnyLoadingInProgress()
{
  WD_LOCK(s_ResourceMutex);

  if (s_pState->m_LoadingQueue.GetCount() > 0)
  {
    return true;
  }

  for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksDataLoad.GetCount(); ++i)
  {
    if (!s_pState->m_WorkerTasksDataLoad[i].m_pTask->IsTaskFinished())
    {
      return true;
    }
  }

  for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksUpdateContent.GetCount(); ++i)
  {
    if (!s_pState->m_WorkerTasksUpdateContent[i].m_pTask->IsTaskFinished())
    {
      return true;
    }
  }
  return false;
}

void wdResourceManager::OnEngineShutdown()
{
  wdResourceManagerEvent e;
  e.m_Type = wdResourceManagerEvent::Type::ManagerShuttingDown;

  // in case of a crash inside the event broadcast or ExecuteAllResourceCleanupCallbacks():
  // you might have a resource type added through a dynamic plugin that has already been unloaded,
  // but the event handler is still referenced
  // to fix this, call wdResource::CleanupDynamicPluginReferences() on that resource type during engine shutdown (see wdStartup)
  s_pState->m_ManagerEvents.Broadcast(e);

  ExecuteAllResourceCleanupCallbacks();

  EngineAboutToShutdown();

  // unload all resources until there are no more that can be unloaded
  FreeAllUnusedResources();
}

void wdResourceManager::OnCoreShutdown()
{
  OnEngineShutdown();

  WD_LOG_BLOCK("Referenced Resources");

  for (auto itType = s_pState->m_LoadedResources.GetIterator(); itType.IsValid(); ++itType)
  {
    const wdRTTI* pRtti = itType.Key();
    LoadedResources& lr = itType.Value();

    if (!lr.m_Resources.IsEmpty())
    {
      WD_LOG_BLOCK("Type", pRtti->GetTypeName());

      wdLog::Error("{0} resource of type '{1}' are still referenced.", lr.m_Resources.GetCount(), pRtti->GetTypeName());

      for (auto it = lr.m_Resources.GetIterator(); it.IsValid(); ++it)
      {
        wdResource* pReference = it.Value();

        wdLog::Info("RC = {0}, ID = '{1}'", pReference->GetReferenceCount(), wdArgSensitive(pReference->GetResourceID(), "ResourceID"));

#if WD_ENABLED(WD_RESOURCEHANDLE_STACK_TRACES)
        pReference->PrintHandleStackTraces();
#endif
      }
    }
  }

  wdPlugin::Events().RemoveEventHandler(PluginEventHandler);

  s_pState.Clear();
}

wdResource* wdResourceManager::GetResource(const wdRTTI* pRtti, wdStringView sResourceID, bool bIsReloadable)
{
  if (sResourceID.IsEmpty())
    return nullptr;

  WD_ASSERT_DEV(s_ResourceMutex.IsLocked(), "Calling code must lock the mutex until the resource pointer is stored in a handle");

  // redirect requested type to override type, if available
  pRtti = FindResourceTypeOverride(pRtti, sResourceID);

  WD_ASSERT_DEBUG(pRtti != nullptr, "There is no RTTI information available for the given resource type '{0}'", WD_STRINGIZE(ResourceType));
  WD_ASSERT_DEBUG(pRtti->GetAllocator() != nullptr && pRtti->GetAllocator()->CanAllocate(), "There is no RTTI allocator available for the given resource type '{0}'", WD_STRINGIZE(ResourceType));

  wdResource* pResource = nullptr;
  wdTempHashedString sHashedResourceID(sResourceID);

  wdHashedString* redirection;
  if (s_pState->m_NamedResources.TryGetValue(sHashedResourceID, redirection))
  {
    sHashedResourceID = *redirection;
    sResourceID = redirection->GetView();
  }

  LoadedResources& lr = s_pState->m_LoadedResources[pRtti];

  if (lr.m_Resources.TryGetValue(sHashedResourceID, pResource))
    return pResource;

  wdResource* pNewResource = pRtti->GetAllocator()->Allocate<wdResource>();
  pNewResource->m_Priority = s_pState->m_ResourceTypePriorities.GetValueOrDefault(pRtti, wdResourcePriority::Medium);
  pNewResource->SetUniqueID(sResourceID, bIsReloadable);
  pNewResource->m_Flags.AddOrRemove(wdResourceFlags::ResourceHasTypeFallback, pNewResource->HasResourceTypeLoadingFallback());

  lr.m_Resources.Insert(sHashedResourceID, pNewResource);

  return pNewResource;
}

void wdResourceManager::RegisterResourceOverrideType(const wdRTTI* pDerivedTypeToUse, wdDelegate<bool(const wdStringBuilder&)> overrideDecider)
{
  const wdRTTI* pParentType = pDerivedTypeToUse->GetParentType();
  while (pParentType != nullptr && pParentType != wdGetStaticRTTI<wdResource>())
  {
    auto& info = s_pState->m_DerivedTypeInfos[pParentType].ExpandAndGetRef();
    info.m_pDerivedType = pDerivedTypeToUse;
    info.m_Decider = overrideDecider;

    pParentType = pParentType->GetParentType();
  }
}

void wdResourceManager::UnregisterResourceOverrideType(const wdRTTI* pDerivedTypeToUse)
{
  const wdRTTI* pParentType = pDerivedTypeToUse->GetParentType();
  while (pParentType != nullptr && pParentType != wdGetStaticRTTI<wdResource>())
  {
    auto it = s_pState->m_DerivedTypeInfos.Find(pParentType);
    pParentType = pParentType->GetParentType();

    if (!it.IsValid())
      break;

    auto& infos = it.Value();

    for (wdUInt32 i = infos.GetCount(); i > 0; --i)
    {
      if (infos[i - 1].m_pDerivedType == pDerivedTypeToUse)
        infos.RemoveAtAndSwap(i - 1);
    }
  }
}

const wdRTTI* wdResourceManager::FindResourceTypeOverride(const wdRTTI* pRtti, wdStringView sResourceID)
{
  auto it = s_pState->m_DerivedTypeInfos.Find(pRtti);

  if (!it.IsValid())
    return pRtti;

  wdStringBuilder sRedirectedPath;
  wdFileSystem::ResolveAssetRedirection(sResourceID, sRedirectedPath);

  while (it.IsValid())
  {
    for (const auto& info : it.Value())
    {
      if (info.m_Decider(sRedirectedPath))
      {
        pRtti = info.m_pDerivedType;
        it = s_pState->m_DerivedTypeInfos.Find(pRtti);
        continue;
      }
    }

    break;
  }

  return pRtti;
}

wdString wdResourceManager::GenerateUniqueResourceID(wdStringView sResourceIDPrefix)
{
  wdStringBuilder resourceID;
  resourceID.Format("{}-{}", sResourceIDPrefix, s_pState->m_uiNextResourceID++);
  return resourceID;
}

wdTypelessResourceHandle wdResourceManager::GetExistingResourceByType(const wdRTTI* pResourceType, wdStringView sResourceID)
{
  wdResource* pResource = nullptr;

  const wdTempHashedString sResourceHash(sResourceID);

  WD_LOCK(s_ResourceMutex);

  const wdRTTI* pRtti = FindResourceTypeOverride(pResourceType, sResourceID);

  if (s_pState->m_LoadedResources[pRtti].m_Resources.TryGetValue(sResourceHash, pResource))
    return wdTypelessResourceHandle(pResource);

  return wdTypelessResourceHandle();
}

wdTypelessResourceHandle wdResourceManager::GetExistingResourceOrCreateAsync(const wdRTTI* pResourceType, wdStringView sResourceID, wdUniquePtr<wdResourceTypeLoader>&& pLoader)
{
  WD_LOCK(s_ResourceMutex);

  wdTypelessResourceHandle hResource = GetExistingResourceByType(pResourceType, sResourceID);

  if (hResource.IsValid())
    return hResource;

  hResource = GetResource(pResourceType, sResourceID, false);
  wdResource* pResource = hResource.m_pResource;

  pResource->m_Flags.Add(wdResourceFlags::HasCustomDataLoader | wdResourceFlags::IsCreatedResource);
  s_pState->m_CustomLoaders[pResource] = std::move(pLoader);

  return hResource;
}

void wdResourceManager::ForceLoadResourceNow(const wdTypelessResourceHandle& hResource)
{
  WD_ASSERT_DEV(hResource.IsValid(), "Cannot access an invalid resource");

  wdResource* pResource = hResource.m_pResource;

  if (pResource->GetLoadingState() != wdResourceState::LoadedResourceMissing && pResource->GetLoadingState() != wdResourceState::Loaded)
  {
    InternalPreloadResource(pResource, true);

    EnsureResourceLoadingState(hResource.m_pResource, wdResourceState::Loaded);
  }
}

void wdResourceManager::RegisterNamedResource(wdStringView sLookupName, wdStringView sRedirectionResource)
{
  WD_LOCK(s_ResourceMutex);

  wdTempHashedString lookup(sLookupName);

  wdHashedString redirection;
  redirection.Assign(sRedirectionResource);

  s_pState->m_NamedResources[lookup] = redirection;
}

void wdResourceManager::UnregisterNamedResource(wdStringView sLookupName)
{
  WD_LOCK(s_ResourceMutex);

  wdTempHashedString hash(sLookupName);
  s_pState->m_NamedResources.Remove(hash);
}

void wdResourceManager::SetResourceLowResData(const wdTypelessResourceHandle& hResource, wdStreamReader* pStream)
{
  wdResource* pResource = hResource.m_pResource;

  if (pResource->GetBaseResourceFlags().IsSet(wdResourceFlags::HasLowResData))
    return;

  if (!pResource->GetBaseResourceFlags().IsSet(wdResourceFlags::IsReloadable))
    return;

  WD_LOCK(s_ResourceMutex);

  // set this, even if we don't end up using the data (because some thread is already loading the full thing)
  pResource->m_Flags.Add(wdResourceFlags::HasLowResData);

  if (IsQueuedForLoading(pResource))
  {
    // if we cannot find it in the queue anymore, some thread already started loading it
    // in this case, do not try to modify it
    if (RemoveFromLoadingQueue(pResource).Failed())
      return;
  }

  pResource->CallUpdateContent(pStream);

  WD_ASSERT_DEV(pResource->GetLoadingState() != wdResourceState::Unloaded, "The resource should have changed its loading state.");

  // Update Memory Usage
  {
    wdResource::MemoryUsage MemUsage;
    MemUsage.m_uiMemoryCPU = 0xFFFFFFFF;
    MemUsage.m_uiMemoryGPU = 0xFFFFFFFF;
    pResource->UpdateMemoryUsage(MemUsage);

    WD_ASSERT_DEV(MemUsage.m_uiMemoryCPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its CPU memory usage", pResource->GetResourceID());
    WD_ASSERT_DEV(MemUsage.m_uiMemoryGPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its GPU memory usage", pResource->GetResourceID());

    pResource->m_MemoryUsage = MemUsage;
  }
}

wdResourceTypeLoader* wdResourceManager::GetDefaultResourceLoader()
{
  return s_pState->m_pDefaultResourceLoader;
}

void wdResourceManager::EnableExportMode(bool bEnable)
{
  WD_ASSERT_DEV(s_pState != nullptr, "wdStartup::StartupCoreSystems() must be called before using the wdResourceManager.");

  s_pState->m_bExportMode = bEnable;
}

bool wdResourceManager::IsExportModeEnabled()
{
  WD_ASSERT_DEV(s_pState != nullptr, "wdStartup::StartupCoreSystems() must be called before using the wdResourceManager.");

  return s_pState->m_bExportMode;
}

void wdResourceManager::RestoreResource(const wdTypelessResourceHandle& hResource)
{
  WD_ASSERT_DEV(hResource.IsValid(), "Cannot access an invalid resource");

  wdResource* pResource = hResource.m_pResource;
  pResource->m_Flags.Remove(wdResourceFlags::PreventFileReload);

  ReloadResource(pResource, true);
}

wdUInt32 wdResourceManager::GetForceNoFallbackAcquisition()
{
  return s_pState->m_uiForceNoFallbackAcquisition;
}

wdTime wdResourceManager::GetLastFrameUpdate()
{
  return s_pState->m_LastFrameUpdate;
}

wdHashTable<const wdRTTI*, wdResourceManager::LoadedResources>& wdResourceManager::GetLoadedResources()
{
  return s_pState->m_LoadedResources;
}

wdDynamicArray<wdResource*>& wdResourceManager::GetLoadedResourceOfTypeTempContainer()
{
  return s_pState->m_LoadedResourceOfTypeTempContainer;
}

void wdResourceManager::SetDefaultResourceLoader(wdResourceTypeLoader* pDefaultLoader)
{
  WD_LOCK(s_ResourceMutex);

  s_pState->m_pDefaultResourceLoader = pDefaultLoader;
}

wdResourceManager::ResourceTypeInfo& wdResourceManager::GetResourceTypeInfo(const wdRTTI* pRtti)
{
  return s_pState->m_TypeInfo[pRtti];
}

WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_ResourceManager);
