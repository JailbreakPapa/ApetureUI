#pragma once

#include <Core/ResourceManager/Implementation/WorkerTasks.h>
#include <Core/ResourceManager/Resource.h>
#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/ResourceManager/ResourceTypeLoader.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Threading/LockedObject.h>
#include <Foundation/Types/UniquePtr.h>

class wdResourceManagerState;

/// \brief The central class for managing all types derived from wdResource
class WD_CORE_DLL wdResourceManager
{
  friend class wdResourceManagerState;
  static wdUniquePtr<wdResourceManagerState> s_pState;

  /// \name Events
  ///@{

public:
  /// Events on individual resources. Subscribe to this to get a notification for events happening on any resource.
  /// If you are only interested in events for a specific resource, subscribe on directly on that instance.
  static const wdEvent<const wdResourceEvent&, wdMutex>& GetResourceEvents();

  /// Events for the resource manager that affect broader things.
  static const wdEvent<const wdResourceManagerEvent&, wdMutex>& GetManagerEvents();

  /// \brief Goes through all existing resources and broadcasts the 'Exists' event.
  ///
  /// Used to announce all currently existing resources to interested event listeners (ie tools).
  static void BroadcastExistsEvent();

  ///@}
  /// \name Loading and creating resources
  ///@{

public:
  /// \brief Returns a handle to the requested resource. szResourceID must uniquely identify the resource, different spellings / casing
  /// will result in different resources.
  ///
  /// After the call to this function the resource definitely exists in memory. Upon access through BeginAcquireResource / wdResourceLock
  /// the resource will be loaded. If it is not possible to load the resource it will change to a 'missing' state. If the code accessing the
  /// resource cannot handle that case, the application will 'terminate' (that means crash).
  template <typename ResourceType>
  static wdTypedResourceHandle<ResourceType> LoadResource(wdStringView sResourceID);

  /// \brief Same as LoadResource(), but additionally allows to set a priority on the resource and a custom fallback resource for this
  /// instance.
  ///
  /// Pass in wdResourcePriority::Unchanged, if you only want to specify a custom fallback resource.
  /// If a resource priority is specified, the target resource will get that priority.
  /// If a valid fallback resource is specified, the resource will store that as its instance specific fallback resource. This will be used
  /// when trying to acquire the resource later.
  template <typename ResourceType>
  static wdTypedResourceHandle<ResourceType> LoadResource(wdStringView sResourceID, wdTypedResourceHandle<ResourceType> hLoadingFallback);


  /// \brief Same as LoadResource(), but instead of a template argument, the resource type to use is given as wdRTTI info. Returns a
  /// typeless handle due to the missing template argument.
  static wdTypelessResourceHandle LoadResourceByType(const wdRTTI* pResourceType, wdStringView sResourceID);

  /// \brief Checks whether any resource loading is in progress
  static bool IsAnyLoadingInProgress();

  /// \brief Generates a unique resource ID with the given prefix.
  ///
  /// Provide a prefix that is preferably not used anywhere else (i.e., closely related to your code).
  /// If the prefix is not also used to manually generate resource IDs, this function is guaranteed to return a unique resource ID.
  static wdString GenerateUniqueResourceID(wdStringView sResourceIDPrefix);

  /// \brief Creates a resource from a descriptor.
  ///
  /// \param szResourceID The unique ID by which the resource is identified. E.g. in GetExistingResource()
  /// \param descriptor A type specific descriptor that holds all the information to create the resource.
  /// \param szResourceDescription An optional description that might help during debugging. Often a human readable name or path is stored
  /// here, to make it easier to identify this resource.
  template <typename ResourceType, typename DescriptorType>
  static wdTypedResourceHandle<ResourceType> CreateResource(wdStringView sResourceID, DescriptorType&& descriptor, wdStringView sResourceDescription = nullptr);

  /// \brief Returns a handle to the resource with the given ID if it exists or creates it from a descriptor.
  ///
  /// \param szResourceID The unique ID by which the resource is identified. E.g. in GetExistingResource()
  /// \param descriptor A type specific descriptor that holds all the information to create the resource.
  /// \param szResourceDescription An optional description that might help during debugging. Often a human readable name or path is stored here, to make it easier to identify this resource.
  template <typename ResourceType, typename DescriptorType>
  static wdTypedResourceHandle<ResourceType> GetOrCreateResource(wdStringView sResourceID, DescriptorType&& descriptor, wdStringView sResourceDescription = nullptr);

  /// \brief Returns a handle to the resource with the given ID. If the resource does not exist, the handle is invalid.
  ///
  /// Use this if a resource needs to be created procedurally (with CreateResource()), but might already have been created.
  /// If the returned handle is invalid, then just go through the resource creation step.
  template <typename ResourceType>
  static wdTypedResourceHandle<ResourceType> GetExistingResource(wdStringView sResourceID);

  /// \brief Same as GetExistingResourceByType() but allows to specify the resource type as an wdRTTI.
  static wdTypelessResourceHandle GetExistingResourceByType(const wdRTTI* pResourceType, wdStringView sResourceID);

  template <typename ResourceType>
  static wdTypedResourceHandle<ResourceType> GetExistingResourceOrCreateAsync(wdStringView sResourceID, wdUniquePtr<wdResourceTypeLoader>&& pLoader, wdTypedResourceHandle<ResourceType> hLoadingFallback = {})
  {
    wdTypelessResourceHandle hTypeless = GetExistingResourceOrCreateAsync(wdGetStaticRTTI<ResourceType>(), sResourceID, std::move(pLoader));

    auto hTyped = wdTypedResourceHandle<ResourceType>((ResourceType*)hTypeless.m_pResource);

    if (hLoadingFallback.IsValid())
    {
      ((ResourceType*)hTypeless.m_pResource)->SetLoadingFallbackResource(hLoadingFallback);
    }

    return hTyped;
  }

  static wdTypelessResourceHandle GetExistingResourceOrCreateAsync(const wdRTTI* pResourceType, wdStringView sResourceID, wdUniquePtr<wdResourceTypeLoader>&& pLoader);

  /// \brief Triggers loading of the given resource. tShouldBeAvailableIn specifies how long the resource is not yet needed, thus allowing
  /// other resources to be loaded first. This is only a hint and there are no guarantees when the resource is available.
  static void PreloadResource(const wdTypelessResourceHandle& hResource);

  /// \brief Similar to locking a resource with 'BlockTillLoaded' acquire mode, but can be done with a typeless handle and does not return a result.
  static void ForceLoadResourceNow(const wdTypelessResourceHandle& hResource);

  /// \brief Returns the current loading state of the given resource.
  static wdResourceState GetLoadingState(const wdTypelessResourceHandle& hResource);

  ///@}
  /// \name Reloading resources
  ///@{

public:
  /// \brief Goes through all resources and makes sure they are reloaded, if they have changed. If bForce is true, all resources
  /// are updated, even if there is no indication that they have changed.
  static wdUInt32 ReloadAllResources(bool bForce);

  /// \brief Goes through all resources of the given type and makes sure they are reloaded, if they have changed. If bForce is true,
  /// resources are updated, even if there is no indication that they have changed.
  template <typename ResourceType>
  static wdUInt32 ReloadResourcesOfType(bool bForce);

  /// \brief Goes through all resources of the given type and makes sure they are reloaded, if they have changed. If bForce is true,
  /// resources are updated, even if there is no indication that they have changed.
  static wdUInt32 ReloadResourcesOfType(const wdRTTI* pType, bool bForce);

  /// \brief Reloads only the one specific resource. If bForce is true, it is updated, even if there is no indication that it has changed.
  template <typename ResourceType>
  static bool ReloadResource(const wdTypedResourceHandle<ResourceType>& hResource, bool bForce);

  /// \brief Reloads only the one specific resource. If bForce is true, it is updated, even if there is no indication that it has changed.
  static bool ReloadResource(const wdRTTI* pType, const wdTypelessResourceHandle& hResource, bool bForce);


  /// \brief Calls ReloadResource() on the given resource, but makes sure that the reload happens with the given custom loader.
  ///
  /// Use this e.g. with a wdResourceLoaderFromMemory to replace an existing resource with new data that was created on-the-fly.
  /// Using this function will set the 'PreventFileReload' flag on the resource and thus prevent further reload actions.
  ///
  /// \sa RestoreResource()
  static void UpdateResourceWithCustomLoader(const wdTypelessResourceHandle& hResource, wdUniquePtr<wdResourceTypeLoader>&& pLoader);

  /// \brief Removes the 'PreventFileReload' flag and forces a reload on the resource.
  ///
  /// \sa UpdateResourceWithCustomLoader()
  static void RestoreResource(const wdTypelessResourceHandle& hResource);

  ///@}
  /// \name Acquiring resources
  ///@{

public:
  /// \brief Acquires a resource pointer from a handle. Prefer to use wdResourceLock, which wraps BeginAcquireResource / EndAcquireResource
  ///
  /// \param hResource The resource to acquire
  /// \param mode The desired way to acquire the resource. See wdResourceAcquireMode for details.
  /// \param hLoadingFallback A custom fallback resource that should be returned if hResource is not yet available. Allows to use domain
  /// specific knowledge to get a better fallback.
  /// \param Priority Allows to adjust the priority of the resource. This will affect how fast
  /// the resource is loaded, in case it is not yet available.
  /// \param out_AcquireResult Returns how successful the acquisition was. See wdResourceAcquireResult for details.
  template <typename ResourceType>
  static ResourceType* BeginAcquireResource(const wdTypedResourceHandle<ResourceType>& hResource, wdResourceAcquireMode mode,
    const wdTypedResourceHandle<ResourceType>& hLoadingFallback = wdTypedResourceHandle<ResourceType>(),
    wdResourceAcquireResult* out_pAcquireResult = nullptr);

  /// \brief Same as BeginAcquireResource but only for the base resource pointer.
  static wdResource* BeginAcquireResourcePointer(const wdRTTI* pType, const wdTypelessResourceHandle& hResource);

  /// \brief Needs to be called in concert with BeginAcquireResource() after accessing a resource has been finished. Prefer to use
  /// wdResourceLock instead.
  template <typename ResourceType>
  static void EndAcquireResource(ResourceType* pResource);

  /// \brief Same as EndAcquireResource but without the template parameter. See also BeginAcquireResourcePointer.
  static void EndAcquireResourcePointer(wdResource* pResource);

  /// \brief Forces the resource manager to treat wdResourceAcquireMode::AllowLoadingFallback as wdResourceAcquireMode::BlockTillLoaded on
  /// BeginAcquireResource.
  static void ForceNoFallbackAcquisition(wdUInt32 uiNumFrames = 0xFFFFFFFF);

  /// \brief If the returned number is greater 0 the resource manager treats wdResourceAcquireMode::AllowLoadingFallback as
  /// wdResourceAcquireMode::BlockTillLoaded on BeginAcquireResource.
  static wdUInt32 GetForceNoFallbackAcquisition();

  /// \brief Retrieves an array of pointers to resources of the indicated type which
  /// are loaded at the moment. Destroy the returned object as soon as possible as it
  /// holds the entire resource manager locked.
  template <typename ResourceType>
  static wdLockedObject<wdMutex, wdDynamicArray<wdResource*>> GetAllResourcesOfType();

  ///@}
  /// \name Unloading resources
  ///@{

public:
  /// \brief Deallocates all resources whose refcount has reached 0. Returns the number of deleted resources.
  static wdUInt32 FreeAllUnusedResources();

  /// \brief Deallocates resources whose refcount has reached 0. Returns the number of deleted resources.
  static wdUInt32 FreeUnusedResources(wdTime timeout, wdTime lastAcquireThreshold);

  /// \brief If timeout is not zero, FreeUnusedResources() is called once every frame with the given parameters.
  static void SetAutoFreeUnused(wdTime timeout, wdTime lastAcquireThreshold);

  /// \brief If set to 'false' resources of the given type will not be incrementally unloaded in the background, when they are not referenced anymore.
  template <typename ResourceType>
  static void SetIncrementalUnloadForResourceType(bool bActive);

  template <typename TypeBeingUpdated, typename TypeItWantsToAcquire>
  static void AllowResourceTypeAcquireDuringUpdateContent()
  {
    AllowResourceTypeAcquireDuringUpdateContent(wdGetStaticRTTI<TypeBeingUpdated>(), wdGetStaticRTTI<TypeItWantsToAcquire>());
  }

  static void AllowResourceTypeAcquireDuringUpdateContent(const wdRTTI* pTypeBeingUpdated, const wdRTTI* pTypeItWantsToAcquire);

  static bool IsResourceTypeAcquireDuringUpdateContentAllowed(const wdRTTI* pTypeBeingUpdated, const wdRTTI* pTypeItWantsToAcquire);

private:
  static wdResult DeallocateResource(wdResource* pResource);

  ///@}
  /// \name Miscellaneous
  ///@{

public:
  /// \brief Returns the resource manager mutex. Allows to lock the manager on a thread when multiple operations need to be done in
  /// sequence.
  static wdMutex& GetMutex() { return s_ResourceMutex; }

  /// \brief Must be called once per frame for some bookkeeping.
  static void PerFrameUpdate();

  /// \brief Makes sure that no further resource loading will take place.
  static void EngineAboutToShutdown();

  /// \brief Calls wdResource::ResetResource() on all resources.
  ///
  /// This is mostly for usage in tools to reset resource whose state can be modified at runtime, to reset them to their original state.
  static void ResetAllResources();

  /// \brief Calls wdResource::UpdateContent() to fill the resource with 'low resolution' data
  ///
  /// This will early out, if the resource has gotten low-res data before.
  /// The resource itself may ignore the data, if it has already gotten low/high res data before.
  ///
  /// The typical use case is, that some other piece of code stores a low-res version of a resource to be able to get
  /// a resource into a usable state. For instance, a material may store low resolution texture data for every texture that it references.
  /// Then when 'loading' the textures, it can pass this low-res data to the textures, such that rendering can give decent results right
  /// away. If the textures have already been loaded before, or some other material already had low-res data, the call exits quickly.
  static void SetResourceLowResData(const wdTypelessResourceHandle& hResource, wdStreamReader* pStream);

  ///@}
  /// \name Type specific loaders
  ///@{

public:
  /// \brief Sets the resource loader to use when no type specific resource loader is available.
  static void SetDefaultResourceLoader(wdResourceTypeLoader* pDefaultLoader);

  /// \brief Returns the resource loader to use when no type specific resource loader is available.
  static wdResourceTypeLoader* GetDefaultResourceLoader();

  /// \brief Sets the resource loader to use for the given resource type.
  ///
  /// \note This is bound to one specific type. Derived types do not inherit the type loader.
  template <typename ResourceType>
  static void SetResourceTypeLoader(wdResourceTypeLoader* pCreator);

  ///@}
  /// \name Named resources
  ///@{

public:
  /// \brief Registers a 'named' resource. When a resource is looked up using \a szLookupName, the lookup will be redirected to \a
  /// szRedirectionResource.
  ///
  /// This can be used to register a resource under an easier to use name. For example one can register "MenuBackground" as the name for "{
  /// E50DCC85-D375-4999-9CFE-42F1377FAC85 }". If the lookup name already exists, it will be overwritten.
  static void RegisterNamedResource(wdStringView sLookupName, wdStringView sRedirectionResource);

  /// \brief Removes a previously registered name from the redirection table.
  static void UnregisterNamedResource(wdStringView sLookupName);


  ///@}
  /// \name Asset system interaction
  ///@{

public:
  /// \brief Registers which resource type to use to load an asset with the given type name
  static void RegisterResourceForAssetType(wdStringView sAssetTypeName, const wdRTTI* pResourceType);

  /// \brief Returns the resource type that was registered to handle the given asset type for loading. nullptr if no resource type was
  /// registered for this asset type.
  static const wdRTTI* FindResourceForAssetType(wdStringView sAssetTypeName);

  ///@}
  /// \name Export mode
  ///@{

public:
  /// \brief Enables export mode. In this mode the resource manager will assert when it actually tries to load a resource.
  /// This can be useful when exporting resource handles but the actual resource content is not needed.
  static void EnableExportMode(bool bEnable);

  /// \brief Returns whether export mode is active.
  static bool IsExportModeEnabled();

  /// \brief Creates a resource handle for the given resource ID. This method can only be used if export mode is enabled.
  /// Internally it will create a resource but does not load the content. This way it can be ensured that the resource handle is always only
  /// the size of a pointer.
  template <typename ResourceType>
  static wdTypedResourceHandle<ResourceType> GetResourceHandleForExport(wdStringView sResourceID);


  ///@}
  /// \name Resource Type Overrides
  ///@{

public:
  /// \brief Registers a resource type to be used instead of any of it's base classes, when loading specific data
  ///
  /// When resource B is derived from A it can be registered to be instantiated when loading data, even if the code specifies to use a
  /// resource of type A.
  /// Whenever LoadResource<A>() is executed, the registered callback \a OverrideDecider is run to figure out whether B should be
  /// instantiated instead. If OverrideDecider returns true, B is used.
  ///
  /// OverrideDecider is given the resource ID after it has been resolved by the wdFileSystem. So it has to be able to make its decision
  /// from the file path, name or extension.
  /// The override is registered for all base classes of \a pDerivedTypeToUse, in case the derivation hierarchy is longer.
  ///
  /// Without calling this at startup, a derived resource type has to be manually requested in code.
  static void RegisterResourceOverrideType(const wdRTTI* pDerivedTypeToUse, wdDelegate<bool(const wdStringBuilder&)> overrideDecider);

  /// \brief Unregisters \a pDerivedTypeToUse as an override resource
  ///
  /// \sa RegisterResourceOverrideType()
  static void UnregisterResourceOverrideType(const wdRTTI* pDerivedTypeToUse);

  ///@}
  /// \name Resource Fallbacks
  ///@{

public:
  /// \brief Specifies which resource to use as a loading fallback for the given type, while a resource is not yet loaded.
  template <typename RESOURCE_TYPE>
  static void SetResourceTypeLoadingFallback(const wdTypedResourceHandle<RESOURCE_TYPE>& hResource)
  {
    RESOURCE_TYPE::SetResourceTypeLoadingFallback(hResource);
  }

  /// \sa SetResourceTypeLoadingFallback()
  template <typename RESOURCE_TYPE>
  static inline const wdTypedResourceHandle<RESOURCE_TYPE>& GetResourceTypeLoadingFallback()
  {
    return RESOURCE_TYPE::GetResourceTypeLoadingFallback();
  }

  /// \brief Specifies which resource to use as a missing fallback for the given type, when a resource cannot be loaded.
  ///
  /// \note If no missing fallback is specified, trying to load a resource that does not exist will assert at runtime.
  template <typename RESOURCE_TYPE>
  static void SetResourceTypeMissingFallback(const wdTypedResourceHandle<RESOURCE_TYPE>& hResource)
  {
    RESOURCE_TYPE::SetResourceTypeMissingFallback(hResource);
  }

  /// \sa SetResourceTypeMissingFallback()
  template <typename RESOURCE_TYPE>
  static inline const wdTypedResourceHandle<RESOURCE_TYPE>& GetResourceTypeMissingFallback()
  {
    return RESOURCE_TYPE::GetResourceTypeMissingFallback();
  }

  using ResourceCleanupCB = wdDelegate<void()>;

  /// \brief [internal] Used by wdResource to register a cleanup function to be called at resource manager shutdown.
  static void AddResourceCleanupCallback(ResourceCleanupCB cb);

  /// \sa AddResourceCleanupCallback()
  static void ClearResourceCleanupCallback(ResourceCleanupCB cb);

  /// \brief This will clear ALL resources that were registered as 'missing' or 'loading' fallback resources. This is called early during
  /// system shutdown to clean up resources.
  static void ExecuteAllResourceCleanupCallbacks();

  ///@}
  /// \name Resource Priorities
  ///@{

public:
  /// \brief Specifies which resource to use as a loading fallback for the given type, while a resource is not yet loaded.
  template <typename RESOURCE_TYPE>
  static void SetResourceTypeDefaultPriority(wdResourcePriority priority)
  {
    GetResourceTypePriorities()[wdGetStaticRTTI<RESOURCE_TYPE>()] = priority;
  }

private:
  static wdMap<const wdRTTI*, wdResourcePriority>& GetResourceTypePriorities();
  ///@}

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

private:
  friend class wdResource;
  friend class wdResourceManagerWorkerDataLoad;
  friend class wdResourceManagerWorkerUpdateContent;
  friend class wdResourceHandleReadContext;

  // Events
private:
  static void BroadcastResourceEvent(const wdResourceEvent& e);

  // Miscellaneous
private:
  static wdMutex s_ResourceMutex;

  // Startup / shutdown
private:
  WD_MAKE_SUBSYSTEM_STARTUP_FRIEND(Core, ResourceManager);
  static void OnEngineShutdown();
  static void OnCoreShutdown();
  static void OnCoreStartup();
  static void PluginEventHandler(const wdPluginEvent& e);

  // Loading / reloading / creating resources
private:
  struct LoadedResources
  {
    wdHashTable<wdTempHashedString, wdResource*> m_Resources;
  };

  struct LoadingInfo
  {
    float m_fPriority = 0;
    wdResource* m_pResource = nullptr;

    WD_ALWAYS_INLINE bool operator==(const LoadingInfo& rhs) const { return m_pResource == rhs.m_pResource; }
    WD_ALWAYS_INLINE bool operator<(const LoadingInfo& rhs) const { return m_fPriority < rhs.m_fPriority; }
  };
  static void EnsureResourceLoadingState(wdResource* pResource, const wdResourceState RequestedState);
  static void PreloadResource(wdResource* pResource);
  static void InternalPreloadResource(wdResource* pResource, bool bHighestPriority);

  template <typename ResourceType>
  static ResourceType* GetResource(wdStringView sResourceID, bool bIsReloadable);
  static wdResource* GetResource(const wdRTTI* pRtti, wdStringView sResourceID, bool bIsReloadable);
  static void RunWorkerTask(wdResource* pResource);
  static void UpdateLoadingDeadlines();
  static void ReverseBubbleSortStep(wdDeque<LoadingInfo>& data);
  static bool ReloadResource(wdResource* pResource, bool bForce);

  static void SetupWorkerTasks();
  static wdTime GetLastFrameUpdate();
  static wdHashTable<const wdRTTI*, LoadedResources>& GetLoadedResources();
  static wdDynamicArray<wdResource*>& GetLoadedResourceOfTypeTempContainer();

  WD_ALWAYS_INLINE static bool IsQueuedForLoading(wdResource* pResource) { return pResource->m_Flags.IsSet(wdResourceFlags::IsQueuedForLoading); }
  [[nodiscard]] static wdResult RemoveFromLoadingQueue(wdResource* pResource);
  static void AddToLoadingQueue(wdResource* pResource, bool bHighPriority);

  struct ResourceTypeInfo
  {
    bool m_bIncrementalUnload = true;
    bool m_bAllowNestedAcquireCached = false;

    wdHybridArray<const wdRTTI*, 8> m_NestedTypes;
  };

  static ResourceTypeInfo& GetResourceTypeInfo(const wdRTTI* pRtti);

  // Type loaders
private:
  static wdResourceTypeLoader* GetResourceTypeLoader(const wdRTTI* pRTTI);

  static wdMap<const wdRTTI*, wdResourceTypeLoader*>& GetResourceTypeLoaders();

  // Override / derived resources
private:
  struct DerivedTypeInfo
  {
    const wdRTTI* m_pDerivedType = nullptr;
    wdDelegate<bool(const wdStringBuilder&)> m_Decider;
  };

  /// \brief Checks whether there is a type override for pRtti given szResourceID and returns that
  static const wdRTTI* FindResourceTypeOverride(const wdRTTI* pRtti, wdStringView sResourceID);
};

#include <Core/ResourceManager/Implementation/ResourceLock.h>
#include <Core/ResourceManager/Implementation/ResourceManager_inl.h>
