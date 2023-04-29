#include <Core/CorePCH.h>

#include <Core/ResourceManager/Implementation/ResourceManagerState.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Profiling/Profiling.h>

wdResourceManagerWorkerDataLoad::wdResourceManagerWorkerDataLoad() = default;
wdResourceManagerWorkerDataLoad::~wdResourceManagerWorkerDataLoad() = default;

void wdResourceManagerWorkerDataLoad::Execute()
{
  WD_PROFILE_SCOPE("LoadResourceFromDisk");

  wdResource* pResourceToLoad = nullptr;
  wdResourceTypeLoader* pLoader = nullptr;
  wdUniquePtr<wdResourceTypeLoader> pCustomLoader;

  {
    WD_LOCK(wdResourceManager::s_ResourceMutex);

    if (wdResourceManager::s_pState->m_LoadingQueue.IsEmpty())
    {
      wdResourceManager::s_pState->m_bAllowLaunchDataLoadTask = true;
      return;
    }

    wdResourceManager::UpdateLoadingDeadlines();

    auto it = wdResourceManager::s_pState->m_LoadingQueue.PeekFront();
    pResourceToLoad = it.m_pResource;
    wdResourceManager::s_pState->m_LoadingQueue.PopFront();

    if (pResourceToLoad->m_Flags.IsSet(wdResourceFlags::HasCustomDataLoader))
    {
      pCustomLoader = std::move(wdResourceManager::s_pState->m_CustomLoaders[pResourceToLoad]);
      pLoader = pCustomLoader.Borrow();
      pResourceToLoad->m_Flags.Remove(wdResourceFlags::HasCustomDataLoader);
      pResourceToLoad->m_Flags.Add(wdResourceFlags::PreventFileReload);
    }
  }

  if (pLoader == nullptr)
    pLoader = wdResourceManager::GetResourceTypeLoader(pResourceToLoad->GetDynamicRTTI());

  if (pLoader == nullptr)
    pLoader = pResourceToLoad->GetDefaultResourceTypeLoader();

  WD_ASSERT_DEV(pLoader != nullptr, "No Loader function available for Resource Type '{0}'", pResourceToLoad->GetDynamicRTTI()->GetTypeName());

  wdResourceLoadData LoaderData = pLoader->OpenDataStream(pResourceToLoad);

  // we need this info later to do some work in a lock, all the directly following code is outside the lock
  const bool bResourceIsLoadedOnMainThread = pResourceToLoad->GetBaseResourceFlags().IsAnySet(wdResourceFlags::UpdateOnMainThread);

  wdSharedPtr<wdResourceManagerWorkerUpdateContent> pUpdateContentTask;
  wdTaskGroupID* pUpdateContentGroup = nullptr;

  WD_LOCK(wdResourceManager::s_ResourceMutex);

  // try to find an update content task that has finished and can be reused
  for (wdUInt32 i = 0; i < wdResourceManager::s_pState->m_WorkerTasksUpdateContent.GetCount(); ++i)
  {
    auto& td = wdResourceManager::s_pState->m_WorkerTasksUpdateContent[i];

    if (wdTaskSystem::IsTaskGroupFinished(td.m_GroupId))
    {
      pUpdateContentTask = td.m_pTask;
      pUpdateContentGroup = &td.m_GroupId;
      break;
    }
  }

  // if no such task could be found, we must allocate a new one
  if (pUpdateContentTask == nullptr)
  {
    wdStringBuilder s;
    s.Format("Resource Content Updater {0}", wdResourceManager::s_pState->m_WorkerTasksUpdateContent.GetCount());

    auto& td = wdResourceManager::s_pState->m_WorkerTasksUpdateContent.ExpandAndGetRef();
    td.m_pTask = WD_DEFAULT_NEW(wdResourceManagerWorkerUpdateContent);
    td.m_pTask->ConfigureTask(s, wdTaskNesting::Maybe);

    pUpdateContentTask = td.m_pTask;
    pUpdateContentGroup = &td.m_GroupId;
  }

  // always updated together with pUpdateContentTask
  WD_MSVC_ANALYSIS_ASSUME(pUpdateContentGroup != nullptr);

  // set up the data load task and launch it
  {
    pUpdateContentTask->m_LoaderData = LoaderData;
    pUpdateContentTask->m_pLoader = pLoader;
    pUpdateContentTask->m_pCustomLoader = std::move(pCustomLoader);
    pUpdateContentTask->m_pResourceToLoad = pResourceToLoad;

    // schedule the task to run, either on the main thread or on some other thread
    *pUpdateContentGroup = wdTaskSystem::StartSingleTask(
      pUpdateContentTask, bResourceIsLoadedOnMainThread ? wdTaskPriority::SomeFrameMainThread : wdTaskPriority::LateNextFrame);

    // restart the next loading task (this one is about to finish)
    wdResourceManager::s_pState->m_bAllowLaunchDataLoadTask = true;
    wdResourceManager::RunWorkerTask(nullptr);

    pCustomLoader.Clear();
  }
}


//////////////////////////////////////////////////////////////////////////

wdResourceManagerWorkerUpdateContent::wdResourceManagerWorkerUpdateContent() = default;
wdResourceManagerWorkerUpdateContent::~wdResourceManagerWorkerUpdateContent() = default;

void wdResourceManagerWorkerUpdateContent::Execute()
{
  if (!m_LoaderData.m_sResourceDescription.IsEmpty())
    m_pResourceToLoad->SetResourceDescription(m_LoaderData.m_sResourceDescription);

  m_pResourceToLoad->CallUpdateContent(m_LoaderData.m_pDataStream);

  if (m_pResourceToLoad->m_uiQualityLevelsLoadable > 0)
  {
    // if the resource can have more details loaded, put it into the preload queue right away again
    wdResourceManager::PreloadResource(m_pResourceToLoad);
  }

  // update the file modification date, if available
  if (m_LoaderData.m_LoadedFileModificationDate.IsValid())
    m_pResourceToLoad->m_LoadedFileModificationTime = m_LoaderData.m_LoadedFileModificationDate;

  WD_ASSERT_DEV(m_pResourceToLoad->GetLoadingState() != wdResourceState::Unloaded, "The resource should have changed its loading state.");

  // Update Memory Usage
  {
    wdResource::MemoryUsage MemUsage;
    MemUsage.m_uiMemoryCPU = 0xFFFFFFFF;
    MemUsage.m_uiMemoryGPU = 0xFFFFFFFF;
    m_pResourceToLoad->UpdateMemoryUsage(MemUsage);

    WD_ASSERT_DEV(
      MemUsage.m_uiMemoryCPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its CPU memory usage", m_pResourceToLoad->GetResourceID());
    WD_ASSERT_DEV(
      MemUsage.m_uiMemoryGPU != 0xFFFFFFFF, "Resource '{0}' did not properly update its GPU memory usage", m_pResourceToLoad->GetResourceID());

    m_pResourceToLoad->m_MemoryUsage = MemUsage;
  }

  m_pLoader->CloseDataStream(m_pResourceToLoad, m_LoaderData);

  {
    WD_LOCK(wdResourceManager::s_ResourceMutex);
    WD_ASSERT_DEV(wdResourceManager::IsQueuedForLoading(m_pResourceToLoad), "Multi-threaded access detected");
    m_pResourceToLoad->m_Flags.Remove(wdResourceFlags::IsQueuedForLoading);
    m_pResourceToLoad->m_LastAcquire = wdResourceManager::GetLastFrameUpdate();
  }

  m_pLoader = nullptr;
  m_pResourceToLoad = nullptr;
}


WD_STATICLINK_FILE(Core, Core_ResourceManager_Implementation_WorkerTasks);
