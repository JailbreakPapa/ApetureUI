#pragma once

#include <Core/CoreInternal.h>
WD_CORE_INTERNAL_HEADER

#include <Core/ResourceManager/ResourceManager.h>

class wdResourceManagerState
{
private:
  friend class wdResource;
  friend class wdResourceManager;
  friend class wdResourceManagerWorkerDataLoad;
  friend class wdResourceManagerWorkerUpdateContent;
  friend class wdResourceHandleReadContext;

  /// \name Events
  ///@{

  wdEvent<const wdResourceEvent&, wdMutex> m_ResourceEvents;
  wdEvent<const wdResourceManagerEvent&, wdMutex> m_ManagerEvents;

  ///@}
  /// \name Resource Fallbacks
  ///@{

  wdDynamicArray<wdResourceManager::ResourceCleanupCB> m_ResourceCleanupCallbacks;

  ///@}
  /// \name Resource Priorities
  ///@{

  wdMap<const wdRTTI*, wdResourcePriority> m_ResourceTypePriorities;

  ///@}

  struct TaskDataUpdateContent
  {
    wdSharedPtr<wdResourceManagerWorkerUpdateContent> m_pTask;
    wdTaskGroupID m_GroupId;
  };

  struct TaskDataDataLoad
  {
    wdSharedPtr<wdResourceManagerWorkerDataLoad> m_pTask;
    wdTaskGroupID m_GroupId;
  };

  bool m_bTaskNamesInitialized = false;
  bool m_bBroadcastExistsEvent = false;
  wdUInt32 m_uiForceNoFallbackAcquisition = 0;

  // resources in this queue are waiting for a task to load them
  wdDeque<wdResourceManager::LoadingInfo> m_LoadingQueue;

  wdHashTable<const wdRTTI*, wdResourceManager::LoadedResources> m_LoadedResources;

  bool m_bAllowLaunchDataLoadTask = true;
  bool m_bShutdown = false;

  wdHybridArray<TaskDataUpdateContent, 24> m_WorkerTasksUpdateContent;
  wdHybridArray<TaskDataDataLoad, 8> m_WorkerTasksDataLoad;

  wdTime m_LastFrameUpdate;
  wdUInt32 m_uiLastResourcePriorityUpdateIdx = 0;

  wdDynamicArray<wdResource*> m_LoadedResourceOfTypeTempContainer;
  wdHashTable<wdTempHashedString, const wdRTTI*> m_ResourcesToUnloadOnMainThread;

  const wdRTTI* m_pFreeUnusedLastType = nullptr;
  wdTempHashedString m_sFreeUnusedLastResourceID;

  // Type Loaders

  wdMap<const wdRTTI*, wdResourceTypeLoader*> m_ResourceTypeLoader;
  wdResourceLoaderFromFile m_FileResourceLoader;
  wdResourceTypeLoader* m_pDefaultResourceLoader = &m_FileResourceLoader;
  wdMap<wdResource*, wdUniquePtr<wdResourceTypeLoader>> m_CustomLoaders;


  // Override / derived resources

  wdMap<const wdRTTI*, wdHybridArray<wdResourceManager::DerivedTypeInfo, 4>> m_DerivedTypeInfos;


  // Named resources

  wdHashTable<wdTempHashedString, wdHashedString> m_NamedResources;

  // Asset system interaction

  wdMap<wdString, const wdRTTI*> m_AssetToResourceType;


  // Export mode

  bool m_bExportMode = false;
  wdUInt32 m_uiNextResourceID = 0;

  // Resource Unloading
  wdTime m_AutoFreeUnusedTimeout = wdTime::Zero();
  wdTime m_AutoFreeUnusedThreshold = wdTime::Zero();

  wdMap<const wdRTTI*, wdResourceManager::ResourceTypeInfo> m_TypeInfo;
};
