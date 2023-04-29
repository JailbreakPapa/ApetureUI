#include <Core/CorePCH.h>

#include <Core/ResourceManager/Implementation/ResourceManagerState.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/Profiling/Profiling.h>

wdTypelessResourceHandle wdResourceManager::LoadResourceByType(const wdRTTI* pResourceType, wdStringView sResourceID)
{
  // the mutex here is necessary to prevent a race between resource unloading and storing the pointer in the handle
  WD_LOCK(s_ResourceMutex);
  return wdTypelessResourceHandle(GetResource(pResourceType, sResourceID, true));
}

void wdResourceManager::InternalPreloadResource(wdResource* pResource, bool bHighestPriority)
{
  if (s_pState->m_bShutdown)
    return;

  WD_PROFILE_SCOPE("InternalPreloadResource");

  WD_LOCK(s_ResourceMutex);

  // if there is nothing else that could be loaded, just return right away
  if (pResource->GetLoadingState() == wdResourceState::Loaded && pResource->GetNumQualityLevelsLoadable() == 0)
  {
    // due to the threading this can happen for all resource types and is valid
    // WD_ASSERT_DEV(!IsQueuedForLoading(pResource), "Invalid flag on resource type '{0}'",
    // pResource->GetDynamicRTTI()->GetTypeName());
    return;
  }

  WD_ASSERT_DEV(!s_pState->m_bExportMode, "Resources should not be loaded in export mode");

  // if we are already loading this resource, early out
  if (IsQueuedForLoading(pResource))
  {
    // however, if it now has highest priority and is still in the loading queue (so not yet started)
    // move it to the front of the queue
    if (bHighestPriority)
    {
      // if it is not in the queue anymore, it has already been started by some thread
      if (RemoveFromLoadingQueue(pResource).Succeeded())
      {
        AddToLoadingQueue(pResource, bHighestPriority);
      }
    }

    return;
  }
  else
  {
    AddToLoadingQueue(pResource, bHighestPriority);

    if (bHighestPriority && wdTaskSystem::GetCurrentThreadWorkerType() == wdWorkerThreadType::FileAccess)
    {
      wdResourceManager::s_pState->m_bAllowLaunchDataLoadTask = true;
    }

    RunWorkerTask(pResource);
  }
}

void wdResourceManager::SetupWorkerTasks()
{
  if (!s_pState->m_bTaskNamesInitialized)
  {
    s_pState->m_bTaskNamesInitialized = true;
    wdStringBuilder s;

    {
      static const wdUInt32 InitialDataLoadTasks = 4;

      for (wdUInt32 i = 0; i < InitialDataLoadTasks; ++i)
      {
        s.Format("Resource Data Loader {0}", i);
        auto& data = s_pState->m_WorkerTasksDataLoad.ExpandAndGetRef();
        data.m_pTask = WD_DEFAULT_NEW(wdResourceManagerWorkerDataLoad);
        data.m_pTask->ConfigureTask(s, wdTaskNesting::Maybe);
      }
    }

    {
      static const wdUInt32 InitialUpdateContentTasks = 16;

      for (wdUInt32 i = 0; i < InitialUpdateContentTasks; ++i)
      {
        s.Format("Resource Content Updater {0}", i);
        auto& data = s_pState->m_WorkerTasksUpdateContent.ExpandAndGetRef();
        data.m_pTask = WD_DEFAULT_NEW(wdResourceManagerWorkerUpdateContent);
        data.m_pTask->ConfigureTask(s, wdTaskNesting::Maybe);
      }
    }
  }
}

void wdResourceManager::RunWorkerTask(wdResource* pResource)
{
  if (s_pState->m_bShutdown)
    return;

  WD_ASSERT_DEV(s_ResourceMutex.IsLocked(), "");

  SetupWorkerTasks();

  if (s_pState->m_bAllowLaunchDataLoadTask && !s_pState->m_LoadingQueue.IsEmpty())
  {
    s_pState->m_bAllowLaunchDataLoadTask = false;

    for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksDataLoad.GetCount(); ++i)
    {
      if (s_pState->m_WorkerTasksDataLoad[i].m_pTask->IsTaskFinished())
      {
        s_pState->m_WorkerTasksDataLoad[i].m_GroupId =
          wdTaskSystem::StartSingleTask(s_pState->m_WorkerTasksDataLoad[i].m_pTask, wdTaskPriority::FileAccess);
        return;
      }
    }

    // could not find any unused task -> need to create a new one
    {
      wdStringBuilder s;
      s.Format("Resource Data Loader {0}", s_pState->m_WorkerTasksDataLoad.GetCount());
      auto& data = s_pState->m_WorkerTasksDataLoad.ExpandAndGetRef();
      data.m_pTask = WD_DEFAULT_NEW(wdResourceManagerWorkerDataLoad);
      data.m_pTask->ConfigureTask(s, wdTaskNesting::Maybe);
      data.m_GroupId = wdTaskSystem::StartSingleTask(data.m_pTask, wdTaskPriority::FileAccess);
    }
  }
}

void wdResourceManager::ReverseBubbleSortStep(wdDeque<LoadingInfo>& data)
{
  // Yep, it's really bubble sort!
  // This will move the entry with the smallest value to the front and move all other values closer to their correct position,
  // which is exactly what we need for the priority queue.
  // We do this once a frame, which gives us nice iterative sorting, with relatively deterministic performance characteristics.

  WD_ASSERT_DEBUG(s_ResourceMutex.IsLocked(), "Calling code must acquire s_ResourceMutex");

  const wdUInt32 uiCount = data.GetCount();

  for (wdUInt32 i = uiCount; i > 1; --i)
  {
    const wdUInt32 idx2 = i - 1;
    const wdUInt32 idx1 = i - 2;

    if (data[idx1].m_fPriority > data[idx1].m_fPriority)
    {
      wdMath::Swap(data[idx1], data[idx2]);
    }
  }
}

void wdResourceManager::UpdateLoadingDeadlines()
{
  if (s_pState->m_LoadingQueue.IsEmpty())
    return;

  WD_ASSERT_DEBUG(s_ResourceMutex.IsLocked(), "Calling code must acquire s_ResourceMutex");

  WD_PROFILE_SCOPE("UpdateLoadingDeadlines");

  const wdUInt32 uiCount = s_pState->m_LoadingQueue.GetCount();
  s_pState->m_uiLastResourcePriorityUpdateIdx = wdMath::Min(s_pState->m_uiLastResourcePriorityUpdateIdx, uiCount);

  wdUInt32 uiUpdateCount = wdMath::Min(50u, uiCount - s_pState->m_uiLastResourcePriorityUpdateIdx);

  if (uiUpdateCount == 0)
  {
    s_pState->m_uiLastResourcePriorityUpdateIdx = 0;
    uiUpdateCount = wdMath::Min(50u, uiCount - s_pState->m_uiLastResourcePriorityUpdateIdx);
  }

  if (uiUpdateCount > 0)
  {
    {
      WD_PROFILE_SCOPE("EvalLoadingDeadlines");

      const wdTime tNow = wdTime::Now();

      for (wdUInt32 i = 0; i < uiUpdateCount; ++i)
      {
        auto& element = s_pState->m_LoadingQueue[s_pState->m_uiLastResourcePriorityUpdateIdx];
        element.m_fPriority = element.m_pResource->GetLoadingPriority(tNow);
        ++s_pState->m_uiLastResourcePriorityUpdateIdx;
      }
    }

    {
      WD_PROFILE_SCOPE("SortLoadingDeadlines");
      ReverseBubbleSortStep(s_pState->m_LoadingQueue);
    }
  }
}

void wdResourceManager::PreloadResource(wdResource* pResource)
{
  InternalPreloadResource(pResource, false);
}

void wdResourceManager::PreloadResource(const wdTypelessResourceHandle& hResource)
{
  WD_ASSERT_DEV(hResource.IsValid(), "Cannot acquire a resource through an invalid handle!");

  wdResource* pResource = hResource.m_pResource;
  PreloadResource(hResource.m_pResource);
}

wdResourceState wdResourceManager::GetLoadingState(const wdTypelessResourceHandle& hResource)
{
  if (hResource.m_pResource == nullptr)
    return wdResourceState::Invalid;

  return hResource.m_pResource->GetLoadingState();
}

wdResult wdResourceManager::RemoveFromLoadingQueue(wdResource* pResource)
{
  WD_ASSERT_DEV(s_ResourceMutex.IsLocked(), "Resource mutex must be locked");

  if (!IsQueuedForLoading(pResource))
    return WD_SUCCESS;

  LoadingInfo li;
  li.m_pResource = pResource;

  if (s_pState->m_LoadingQueue.RemoveAndSwap(li))
  {
    pResource->m_Flags.Remove(wdResourceFlags::IsQueuedForLoading);
    return WD_SUCCESS;
  }

  return WD_FAILURE;
}

void wdResourceManager::AddToLoadingQueue(wdResource* pResource, bool bHighestPriority)
{
  WD_ASSERT_DEV(s_ResourceMutex.IsLocked(), "Resource mutex must be locked");
  WD_ASSERT_DEV(IsQueuedForLoading(pResource) == false, "Resource is already in the loading queue");

  pResource->m_Flags.Add(wdResourceFlags::IsQueuedForLoading);

  LoadingInfo li;
  li.m_pResource = pResource;

  if (bHighestPriority)
  {
    pResource->SetPriority(wdResourcePriority::Critical);
    li.m_fPriority = 0.0f;
    s_pState->m_LoadingQueue.PushFront(li);
  }
  else
  {
    li.m_fPriority = pResource->GetLoadingPriority(s_pState->m_LastFrameUpdate);
    s_pState->m_LoadingQueue.PushBack(li);
  }
}

bool wdResourceManager::ReloadResource(wdResource* pResource, bool bForce)
{
  WD_LOCK(s_ResourceMutex);

  if (!pResource->m_Flags.IsAnySet(wdResourceFlags::IsReloadable))
    return false;

  if (!bForce && pResource->m_Flags.IsAnySet(wdResourceFlags::PreventFileReload))
    return false;

  wdResourceTypeLoader* pLoader = wdResourceManager::GetResourceTypeLoader(pResource->GetDynamicRTTI());

  if (pLoader == nullptr)
    pLoader = pResource->GetDefaultResourceTypeLoader();

  if (pLoader == nullptr)
    return false;

  // no need to reload resources that are not loaded so far
  if (pResource->GetLoadingState() == wdResourceState::Unloaded)
    return false;

  bool bAllowPreloading = true;

  // if the resource is already in the loading queue we can just keep it there
  if (IsQueuedForLoading(pResource))
  {
    bAllowPreloading = false;

    LoadingInfo li;
    li.m_pResource = pResource;

    if (s_pState->m_LoadingQueue.IndexOf(li) == wdInvalidIndex)
    {
      // the resource is marked as 'loading' but it is not in the queue anymore
      // that means some task is already working on loading it
      // therefore we should not touch it (especially unload it), it might end up in an inconsistent state

      wdLog::Dev(
        "Resource '{0}' is not being reloaded, because it is currently being loaded", wdArgSensitive(pResource->GetResourceID(), "ResourceID"));
      return false;
    }
  }

  // if bForce, skip the outdated check
  if (!bForce)
  {
    if (!pLoader->IsResourceOutdated(pResource))
      return false;

    if (pResource->GetLoadingState() == wdResourceState::LoadedResourceMissing)
    {
      wdLog::Dev("Resource '{0}' is missing and will be tried to be reloaded ('{1}')", wdArgSensitive(pResource->GetResourceID(), "ResourceID"),
        wdArgSensitive(pResource->GetResourceDescription(), "ResourceDesc"));
    }
    else
    {
      wdLog::Dev("Resource '{0}' is outdated and will be reloaded ('{1}')", wdArgSensitive(pResource->GetResourceID(), "ResourceID"),
        wdArgSensitive(pResource->GetResourceDescription(), "ResourceDesc"));
    }
  }

  if (pResource->GetBaseResourceFlags().IsSet(wdResourceFlags::UpdateOnMainThread) == false || wdThreadUtils::IsMainThread())
  {
    // make sure existing data is purged
    pResource->CallUnloadData(wdResource::Unload::AllQualityLevels);

    WD_ASSERT_DEV(pResource->GetLoadingState() <= wdResourceState::LoadedResourceMissing, "Resource '{0}' should be in an unloaded state now.",
      pResource->GetResourceID());
  }
  else
  {
    s_pState->m_ResourcesToUnloadOnMainThread.Insert(wdTempHashedString(pResource->GetResourceID().GetData()), pResource->GetDynamicRTTI());
  }

  if (bAllowPreloading)
  {
    const wdTime tNow = s_pState->m_LastFrameUpdate;

    // resources that have been in use recently will be put into the preload queue immediately
    // everything else will be loaded on demand
    if (pResource->GetLastAcquireTime() >= tNow - wdTime::Seconds(30.0))
    {
      PreloadResource(pResource);
    }
  }

  return true;
}

wdUInt32 wdResourceManager::ReloadResourcesOfType(const wdRTTI* pType, bool bForce)
{
  WD_LOCK(s_ResourceMutex);
  WD_LOG_BLOCK("wdResourceManager::ReloadResourcesOfType", pType->GetTypeName());

  wdUInt32 count = 0;

  LoadedResources& lr = s_pState->m_LoadedResources[pType];

  for (auto it = lr.m_Resources.GetIterator(); it.IsValid(); ++it)
  {
    if (ReloadResource(it.Value(), bForce))
      ++count;
  }

  return count;
}

wdUInt32 wdResourceManager::ReloadAllResources(bool bForce)
{
  WD_PROFILE_SCOPE("ReloadAllResources");

  WD_LOCK(s_ResourceMutex);
  WD_LOG_BLOCK("wdResourceManager::ReloadAllResources");

  wdUInt32 count = 0;

  for (auto itType = s_pState->m_LoadedResources.GetIterator(); itType.IsValid(); ++itType)
  {
    for (auto it = itType.Value().m_Resources.GetIterator(); it.IsValid(); ++it)
    {
      if (ReloadResource(it.Value(), bForce))
        ++count;
    }
  }

  if (count > 0)
  {
    wdResourceManagerEvent e;
    e.m_Type = wdResourceManagerEvent::Type::ReloadAllResources;

    s_pState->m_ManagerEvents.Broadcast(e);
  }

  return count;
}

void wdResourceManager::UpdateResourceWithCustomLoader(const wdTypelessResourceHandle& hResource, wdUniquePtr<wdResourceTypeLoader>&& pLoader)
{
  WD_LOCK(s_ResourceMutex);

  hResource.m_pResource->m_Flags.Add(wdResourceFlags::HasCustomDataLoader);
  s_pState->m_CustomLoaders[hResource.m_pResource] = std::move(pLoader);
  // if there was already a custom loader set, but it got no action yet, it is deleted here and replaced with the newer loader

  ReloadResource(hResource.m_pResource, true);
};

void wdResourceManager::EnsureResourceLoadingState(wdResource* pResourceToLoad, const wdResourceState RequestedState)
{
  const wdRTTI* pOwnRtti = pResourceToLoad->GetDynamicRTTI();

  // help loading until the requested resource is available
  while ((wdInt32)pResourceToLoad->GetLoadingState() < (wdInt32)RequestedState &&
         (pResourceToLoad->GetLoadingState() != wdResourceState::LoadedResourceMissing))
  {
    wdTaskGroupID tgid;

    {
      WD_LOCK(s_ResourceMutex);

      for (wdUInt32 i = 0; i < s_pState->m_WorkerTasksUpdateContent.GetCount(); ++i)
      {
        const wdResource* pQueuedResource = s_pState->m_WorkerTasksUpdateContent[i].m_pTask->m_pResourceToLoad;

        if (pQueuedResource != nullptr && pQueuedResource != pResourceToLoad && !s_pState->m_WorkerTasksUpdateContent[i].m_pTask->IsTaskFinished())
        {
          if (!IsResourceTypeAcquireDuringUpdateContentAllowed(pQueuedResource->GetDynamicRTTI(), pOwnRtti))
          {
            tgid = s_pState->m_WorkerTasksUpdateContent[i].m_GroupId;
            break;
          }
        }
      }
    }

    if (tgid.IsValid())
    {
      wdTaskSystem::WaitForGroup(tgid);
    }
    else
    {
      // do not use wdThreadUtils::YieldTimeSlice here, otherwise the thread is not tagged as 'blocked' in the TaskSystem
      wdTaskSystem::WaitForCondition([=]() -> bool {
        return (wdInt32)pResourceToLoad->GetLoadingState() >= (wdInt32)RequestedState ||
               (pResourceToLoad->GetLoadingState() == wdResourceState::LoadedResourceMissing);
      });
    }
  }
}


WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_ResourceLoading);
