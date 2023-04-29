#pragma once

#include <Foundation/Logging/Log.h>

template <typename ResourceType>
ResourceType* wdResourceManager::GetResource(wdStringView sResourceID, bool bIsReloadable)
{
  return static_cast<ResourceType*>(GetResource(wdGetStaticRTTI<ResourceType>(), sResourceID, bIsReloadable));
}

template <typename ResourceType>
wdTypedResourceHandle<ResourceType> wdResourceManager::LoadResource(wdStringView sResourceID)
{
  // the mutex here is necessary to prevent a race between resource unloading and storing the pointer in the handle
  WD_LOCK(s_ResourceMutex);
  return wdTypedResourceHandle<ResourceType>(GetResource<ResourceType>(sResourceID, true));
}

template <typename ResourceType>
wdTypedResourceHandle<ResourceType> wdResourceManager::LoadResource(wdStringView sResourceID, wdTypedResourceHandle<ResourceType> hLoadingFallback)
{
  wdTypedResourceHandle<ResourceType> hResource;
  {
    // the mutex here is necessary to prevent a race between resource unloading and storing the pointer in the handle
    WD_LOCK(s_ResourceMutex);
    hResource = wdTypedResourceHandle<ResourceType>(GetResource<ResourceType>(sResourceID, true));
  }

  if (hLoadingFallback.IsValid())
  {
    hResource.m_pResource->SetLoadingFallbackResource(hLoadingFallback);
  }

  return hResource;
}

template <typename ResourceType>
wdTypedResourceHandle<ResourceType> wdResourceManager::GetExistingResource(wdStringView sResourceID)
{
  wdResource* pResource = nullptr;

  const wdTempHashedString sResourceHash(sResourceID);

  WD_LOCK(s_ResourceMutex);

  const wdRTTI* pRtti = FindResourceTypeOverride(wdGetStaticRTTI<ResourceType>(), sResourceID);

  if (GetLoadedResources()[pRtti].m_Resources.TryGetValue(sResourceHash, pResource))
    return wdTypedResourceHandle<ResourceType>((ResourceType*)pResource);

  return wdTypedResourceHandle<ResourceType>();
}

template <typename ResourceType, typename DescriptorType>
wdTypedResourceHandle<ResourceType> wdResourceManager::CreateResource(wdStringView sResourceID, DescriptorType&& descriptor, wdStringView sResourceDescription)
{
  static_assert(std::is_rvalue_reference<DescriptorType&&>::value, "Please std::move the descriptor into this function");

  WD_LOG_BLOCK("wdResourceManager::CreateResource", sResourceID);

  WD_LOCK(s_ResourceMutex);

  wdTypedResourceHandle<ResourceType> hResource(GetResource<ResourceType>(sResourceID, false));

  ResourceType* pResource = BeginAcquireResource(hResource, wdResourceAcquireMode::PointerOnly);
  pResource->SetResourceDescription(sResourceDescription);
  pResource->m_Flags.Add(wdResourceFlags::IsCreatedResource);

  WD_ASSERT_DEV(pResource->GetLoadingState() == wdResourceState::Unloaded, "CreateResource was called on a resource that is already created");

  // If this does not compile, you either passed in the wrong descriptor type for the given resource type
  // or you forgot to std::move the descriptor when calling CreateResource
  {
    auto localDescriptor = std::move(descriptor);
    wdResourceLoadDesc ld = pResource->CreateResource(std::move(localDescriptor));
    pResource->VerifyAfterCreateResource(ld);
  }

  WD_ASSERT_DEV(pResource->GetLoadingState() != wdResourceState::Unloaded, "CreateResource did not set the loading state properly.");

  EndAcquireResource(pResource);

  return hResource;
}

template <typename ResourceType, typename DescriptorType>
wdTypedResourceHandle<ResourceType>
wdResourceManager::GetOrCreateResource(wdStringView sResourceID, DescriptorType&& descriptor, wdStringView sResourceDescription)
{
  WD_LOCK(s_ResourceMutex);
  wdTypedResourceHandle<ResourceType> hResource = GetExistingResource<ResourceType>(sResourceID);
  if (!hResource.IsValid())
  {
    hResource = CreateResource<ResourceType, DescriptorType>(sResourceID, std::move(descriptor), sResourceDescription);
  }

  return hResource;
}

WD_FORCE_INLINE wdResource* wdResourceManager::BeginAcquireResourcePointer(const wdRTTI* pType, const wdTypelessResourceHandle& hResource)
{
  WD_ASSERT_DEV(hResource.IsValid(), "Cannot acquire a resource through an invalid handle!");

  wdResource* pResource = (wdResource*)hResource.m_pResource;

  WD_ASSERT_DEBUG(pResource->GetDynamicRTTI()->IsDerivedFrom(pType),
    "The requested resource does not have the same type ('{0}') as the resource handle ('{1}').", pResource->GetDynamicRTTI()->GetTypeName(),
    pType->GetTypeName());

  // pResource->m_iLockCount.Increment();
  return pResource;
}

template <typename ResourceType>
ResourceType* wdResourceManager::BeginAcquireResource(const wdTypedResourceHandle<ResourceType>& hResource, wdResourceAcquireMode mode,
  const wdTypedResourceHandle<ResourceType>& hFallbackResource, wdResourceAcquireResult* out_pAcquireResult /*= nullptr*/)
{
  WD_ASSERT_DEV(hResource.IsValid(), "Cannot acquire a resource through an invalid handle!");

#if WD_ENABLED(WD_COMPILE_FOR_DEVELOPMENT)
  const wdResource* pCurrentlyUpdatingContent = wdResource::GetCurrentlyUpdatingContent();
  if (pCurrentlyUpdatingContent != nullptr)
  {
    WD_LOCK(s_ResourceMutex);
    WD_ASSERT_DEV(mode == wdResourceAcquireMode::PointerOnly || IsResourceTypeAcquireDuringUpdateContentAllowed(pCurrentlyUpdatingContent->GetDynamicRTTI(), wdGetStaticRTTI<ResourceType>()),
      "Trying to acquire a resource of type '{0}' during '{1}::UpdateContent()'. This has to be enabled by calling "
      "wdResourceManager::AllowResourceTypeAcquireDuringUpdateContent<{1}, {0}>(); at engine startup, for example in "
      "wdGameApplication::Init_SetupDefaultResources().",
      wdGetStaticRTTI<ResourceType>()->GetTypeName(), pCurrentlyUpdatingContent->GetDynamicRTTI()->GetTypeName());
  }
#endif

  ResourceType* pResource = (ResourceType*)hResource.m_hTypeless.m_pResource;

  // WD_ASSERT_DEV(pResource->m_iLockCount < 20, "You probably forgot somewhere to call 'EndAcquireResource' in sync with 'BeginAcquireResource'.");
  WD_ASSERT_DEBUG(pResource->GetDynamicRTTI()->template IsDerivedFrom<ResourceType>(),
    "The requested resource does not have the same type ('{0}') as the resource handle ('{1}').", pResource->GetDynamicRTTI()->GetTypeName(),
    wdGetStaticRTTI<ResourceType>()->GetTypeName());

  if (mode == wdResourceAcquireMode::AllowLoadingFallback && GetForceNoFallbackAcquisition() > 0)
  {
    mode = wdResourceAcquireMode::BlockTillLoaded;
  }

  if (mode == wdResourceAcquireMode::PointerOnly)
  {
    if (out_pAcquireResult)
      *out_pAcquireResult = wdResourceAcquireResult::Final;

    // pResource->m_iLockCount.Increment();
    return pResource;
  }

  // only set the last accessed time stamp, if it is actually needed, pointer-only access might not mean that the resource is used
  // productively
  pResource->m_LastAcquire = GetLastFrameUpdate();

  if (pResource->GetLoadingState() != wdResourceState::LoadedResourceMissing)
  {
    if (pResource->GetLoadingState() != wdResourceState::Loaded)
    {
      // if BlockTillLoaded is specified, it will prepended to the preload array, thus will be loaded immediately
      InternalPreloadResource(pResource, mode >= wdResourceAcquireMode::BlockTillLoaded);

      if (mode == wdResourceAcquireMode::AllowLoadingFallback &&
          (pResource->m_hLoadingFallback.IsValid() || hFallbackResource.IsValid() || GetResourceTypeLoadingFallback<ResourceType>().IsValid()))
      {
        // return the fallback resource for now, if there is one
        if (out_pAcquireResult)
          *out_pAcquireResult = wdResourceAcquireResult::LoadingFallback;

        // Fallback order is as follows:
        //  1) Prefer any resource specific fallback resource
        //  2) If not available, use the fallback that is given to BeginAcquireResource, as that is at least specific to the situation
        //  3) If nothing else is available, take the fallback for the whole resource type

        if (pResource->m_hLoadingFallback.IsValid())
          return (ResourceType*)BeginAcquireResource(pResource->m_hLoadingFallback, wdResourceAcquireMode::BlockTillLoaded);
        else if (hFallbackResource.IsValid())
          return (ResourceType*)BeginAcquireResource(hFallbackResource, wdResourceAcquireMode::BlockTillLoaded);
        else
          return (ResourceType*)BeginAcquireResource(GetResourceTypeLoadingFallback<ResourceType>(), wdResourceAcquireMode::BlockTillLoaded);
      }

      EnsureResourceLoadingState(pResource, wdResourceState::Loaded);
    }
    else
    {
      // as long as there are more quality levels available, schedule the resource for more loading
      // accessing IsQueuedForLoading without a lock here is save because InternalPreloadResource() will lock and early out if necessary
      // and accidentally skipping InternalPreloadResource() is no problem
      if (IsQueuedForLoading(pResource) == false && pResource->GetNumQualityLevelsLoadable() > 0)
        InternalPreloadResource(pResource, false);
    }
  }

  if (pResource->GetLoadingState() == wdResourceState::LoadedResourceMissing)
  {
    // When you get a crash with a stack overflow in this code path, then the resource to be used as the
    // 'missing resource' replacement might be missing itself.

    if (wdResourceManager::GetResourceTypeMissingFallback<ResourceType>().IsValid())
    {
      if (out_pAcquireResult)
        *out_pAcquireResult = wdResourceAcquireResult::MissingFallback;

      return (ResourceType*)BeginAcquireResource(
        wdResourceManager::GetResourceTypeMissingFallback<ResourceType>(), wdResourceAcquireMode::BlockTillLoaded);
    }

    if (mode != wdResourceAcquireMode::AllowLoadingFallback_NeverFail && mode != wdResourceAcquireMode::BlockTillLoaded_NeverFail)
    {
      WD_REPORT_FAILURE("The resource '{0}' of type '{1}' is missing and no fallback is available", pResource->GetResourceID(),
        wdGetStaticRTTI<ResourceType>()->GetTypeName());
    }

    if (out_pAcquireResult)
      *out_pAcquireResult = wdResourceAcquireResult::None;

    return nullptr;
  }

  if (out_pAcquireResult)
    *out_pAcquireResult = wdResourceAcquireResult::Final;

  // pResource->m_iLockCount.Increment();
  return pResource;
}

template <typename ResourceType>
void wdResourceManager::EndAcquireResource(ResourceType* pResource)
{
  // WD_ASSERT_DEV(pResource->m_iLockCount > 0, "The resource lock counter is incorrect: {0}", (wdInt32)pResource->m_iLockCount);
  // pResource->m_iLockCount.Decrement();
}

WD_FORCE_INLINE void wdResourceManager::EndAcquireResourcePointer(wdResource* pResource)
{
  // WD_ASSERT_DEV(pResource->m_iLockCount > 0, "The resource lock counter is incorrect: {0}", (wdInt32)pResource->m_iLockCount);
  // pResource->m_iLockCount.Decrement();
}

template <typename ResourceType>
wdLockedObject<wdMutex, wdDynamicArray<wdResource*>> wdResourceManager::GetAllResourcesOfType()
{
  const wdRTTI* pBaseType = wdGetStaticRTTI<ResourceType>();

  auto& container = GetLoadedResourceOfTypeTempContainer();

  // We use a static container here to ensure its life-time is extended beyond
  // calls to this function as the locked object does not own the passed-in object
  // and thus does not extend the data life-time. It is safe to do this, as the
  // locked object holding the container ensures the container will not be
  // accessed concurrently.
  wdLockedObject<wdMutex, wdDynamicArray<wdResource*>> loadedResourcesLock(s_ResourceMutex, &container);

  container.Clear();

  for (auto itType = GetLoadedResources().GetIterator(); itType.IsValid(); itType.Next())
  {
    const wdRTTI* pDerivedType = itType.Key();

    if (pDerivedType->IsDerivedFrom(pBaseType))
    {
      const LoadedResources& lr = GetLoadedResources()[pDerivedType];

      container.Reserve(container.GetCount() + lr.m_Resources.GetCount());

      for (auto itResource : lr.m_Resources)
      {
        container.PushBack(itResource.Value());
      }
    }
  }

  return loadedResourcesLock;
}

template <typename ResourceType>
bool wdResourceManager::ReloadResource(const wdTypedResourceHandle<ResourceType>& hResource, bool bForce)
{
  ResourceType* pResource = BeginAcquireResource(hResource, wdResourceAcquireMode::PointerOnly);

  bool res = ReloadResource(pResource, bForce);

  EndAcquireResource(pResource);

  return res;
}

WD_FORCE_INLINE bool wdResourceManager::ReloadResource(const wdRTTI* pType, const wdTypelessResourceHandle& hResource, bool bForce)
{
  wdResource* pResource = BeginAcquireResourcePointer(pType, hResource);

  bool res = ReloadResource(pResource, bForce);

  EndAcquireResourcePointer(pResource);

  return res;
}

template <typename ResourceType>
wdUInt32 wdResourceManager::ReloadResourcesOfType(bool bForce)
{
  return ReloadResourcesOfType(wdGetStaticRTTI<ResourceType>(), bForce);
}

template <typename ResourceType>
void wdResourceManager::SetResourceTypeLoader(wdResourceTypeLoader* pCreator)
{
  WD_LOCK(s_ResourceMutex);

  GetResourceTypeLoaders()[wdGetStaticRTTI<ResourceType>()] = pCreator;
}

template <typename ResourceType>
wdTypedResourceHandle<ResourceType> wdResourceManager::GetResourceHandleForExport(wdStringView sResourceID)
{
  WD_ASSERT_DEV(IsExportModeEnabled(), "Export mode needs to be enabled");

  return LoadResource<ResourceType>(sResourceID);
}

template <typename ResourceType>
void wdResourceManager::SetIncrementalUnloadForResourceType(bool bActive)
{
  GetResourceTypeInfo(wdGetStaticRTTI<ResourceType>()).m_bIncrementalUnload = bActive;
}
