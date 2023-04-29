#include <Core/CorePCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Core/Collection/CollectionResource.h>
#include <Foundation/Profiling/Profiling.h>

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdCollectionResource, 1, wdRTTIDefaultAllocator<wdCollectionResource>)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_RESOURCE_IMPLEMENT_COMMON_CODE(wdCollectionResource);

wdCollectionResource::wdCollectionResource()
  : wdResource(DoUpdate::OnAnyThread, 1)
{
}

bool wdCollectionResource::PreloadResources(wdUInt32 uiNumResourcesToPreload)
{
  WD_LOCK(m_PreloadMutex);
  WD_PROFILE_SCOPE("Inject Resources to Preload");

  if (m_PreloadedResources.GetCount() == m_Collection.m_Resources.GetCount())
  {
    // All resources have already been queued so there is no need
    // to redo the work. Clearing the array would in fact potentially
    // trigger one of the resources to be unloaded, undoing the work
    // that was already done to preload the collection.
    return false;
  }

  m_PreloadedResources.Reserve(m_Collection.m_Resources.GetCount());

  const wdUInt32 remainingResources = m_Collection.m_Resources.GetCount() - m_PreloadedResources.GetCount();
  const wdUInt32 end = wdMath::Min(remainingResources, uiNumResourcesToPreload) + m_PreloadedResources.GetCount();
  for (wdUInt32 i = m_PreloadedResources.GetCount(); i < end; ++i)
  {
    const wdCollectionEntry& e = m_Collection.m_Resources[i];
    wdTypelessResourceHandle hTypeless;

    if (!e.m_sAssetTypeName.IsEmpty())
    {
      if (const wdRTTI* pRtti = wdResourceManager::FindResourceForAssetType(e.m_sAssetTypeName))
      {
        hTypeless = wdResourceManager::LoadResourceByType(pRtti, e.m_sResourceID);
      }
      else
      {
        wdLog::Error("There was no valid RTTI available for assets with type name '{}'. Could not pre-load resource '{}'. Did you forget to register "
                     "the resource type with the wdResourceManager?",
          e.m_sAssetTypeName, wdArgSensitive(e.m_sResourceID, "ResourceID"));
      }
    }
    else
    {
      wdLog::Error("Asset '{}' had an empty asset type name. Cannot pre-load it.", wdArgSensitive(e.m_sResourceID, "ResourceID"));
    }

    m_PreloadedResources.PushBack(hTypeless);

    if (hTypeless.IsValid())
    {
      wdResourceManager::PreloadResource(hTypeless);
    }
  }

  return m_PreloadedResources.GetCount() < m_Collection.m_Resources.GetCount();
}

bool wdCollectionResource::IsLoadingFinished(float* out_pProgress) const
{
  WD_LOCK(m_PreloadMutex);

  wdUInt64 loadedWeight = 0;
  wdUInt64 totalWeight = 0;

  for (wdUInt32 i = 0; i < m_PreloadedResources.GetCount(); i++)
  {
    const wdTypelessResourceHandle& hResource = m_PreloadedResources[i];
    if (!hResource.IsValid())
      continue;

    const wdCollectionEntry& entry = m_Collection.m_Resources[i];
    wdUInt64 thisWeight = wdMath::Max(entry.m_uiFileSize, 1ull); // if file sizes are not specified, we weight by 1
    wdResourceState state = wdResourceManager::GetLoadingState(hResource);

    if (state == wdResourceState::Loaded || state == wdResourceState::LoadedResourceMissing)
    {
      loadedWeight += thisWeight;
    }

    if (state != wdResourceState::Invalid)
    {
      totalWeight += thisWeight;
    }
  }

  if (out_pProgress != nullptr)
  {
    const float maxLoadedFraction = m_Collection.m_Resources.GetCount() == 0 ? 1.f : (float)m_PreloadedResources.GetCount() / m_Collection.m_Resources.GetCount();
    if (totalWeight != 0 && totalWeight != loadedWeight)
    {
      *out_pProgress = static_cast<float>(static_cast<double>(loadedWeight) / totalWeight) * maxLoadedFraction;
    }
    else
    {
      *out_pProgress = maxLoadedFraction;
    }
  }

  if (totalWeight == 0 || totalWeight == loadedWeight)
  {
    return true;
  }

  return false;
}


const wdCollectionResourceDescriptor& wdCollectionResource::GetDescriptor() const
{
  return m_Collection;
}

WD_RESOURCE_IMPLEMENT_CREATEABLE(wdCollectionResource, wdCollectionResourceDescriptor)
{
  m_Collection = descriptor;

  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Loaded;

  return res;
}

wdResourceLoadDesc wdCollectionResource::UnloadData(Unload WhatToUnload)
{
  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Unloaded;

  {
    UnregisterNames();
    // This lock unnecessary as this function is only called when the reference count is 0, i.e. if we deallocate this.
    // It is intentionally removed as it caused this lock and the resource manager lock to be locked in reverse order.
    // To prevent potential deadlocks and be able to sanity check our locking the entire codebase should never lock any
    // locks in reverse order, even if this lock is probably fine it prevents us from reasoning over the entire system.
    //WD_LOCK(m_preloadMutex);
    m_PreloadedResources.Clear();
    m_Collection.m_Resources.Clear();

    m_PreloadedResources.Compact();
    m_Collection.m_Resources.Compact();
  }

  return res;
}

wdResourceLoadDesc wdCollectionResource::UpdateContent(wdStreamReader* Stream)
{
  WD_LOG_BLOCK("wdCollectionResource::UpdateContent", GetResourceDescription().GetData());

  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;

  if (Stream == nullptr)
  {
    res.m_State = wdResourceState::LoadedResourceMissing;
    return res;
  }

  // skip the absolute file path data that the standard file reader writes into the stream
  {
    wdStringBuilder sAbsFilePath;
    (*Stream) >> sAbsFilePath;
  }

  // skip the asset file header at the start of the file
  wdAssetFileHeader AssetHash;
  AssetHash.Read(*Stream).IgnoreResult();

  m_Collection.Load(*Stream);

  res.m_State = wdResourceState::Loaded;
  return res;
}

void wdCollectionResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  WD_LOCK(m_PreloadMutex);
  out_NewMemoryUsage.m_uiMemoryGPU = 0;
  out_NewMemoryUsage.m_uiMemoryCPU = static_cast<wdUInt32>(m_PreloadedResources.GetHeapMemoryUsage() + m_Collection.m_Resources.GetHeapMemoryUsage());
}


void wdCollectionResource::RegisterNames()
{
  if (m_bRegistered)
    return;

  m_bRegistered = true;

  WD_LOCK(wdResourceManager::GetMutex());

  for (const auto& entry : m_Collection.m_Resources)
  {
    if (!entry.m_sOptionalNiceLookupName.IsEmpty())
    {
      wdResourceManager::RegisterNamedResource(entry.m_sOptionalNiceLookupName, entry.m_sResourceID);
    }
  }
}


void wdCollectionResource::UnregisterNames()
{
  if (!m_bRegistered)
    return;

  m_bRegistered = false;

  WD_LOCK(wdResourceManager::GetMutex());

  for (const auto& entry : m_Collection.m_Resources)
  {
    if (!entry.m_sOptionalNiceLookupName.IsEmpty())
    {
      wdResourceManager::UnregisterNamedResource(entry.m_sOptionalNiceLookupName);
    }
  }
}

void wdCollectionResourceDescriptor::Save(wdStreamWriter& inout_stream) const
{
  const wdUInt8 uiVersion = 3;
  const wdUInt8 uiIdentifier = 0xC0;
  const wdUInt32 uiNumResources = m_Resources.GetCount();

  inout_stream << uiVersion;
  inout_stream << uiIdentifier;
  inout_stream << uiNumResources;

  for (wdUInt32 i = 0; i < uiNumResources; ++i)
  {
    inout_stream << m_Resources[i].m_sAssetTypeName;
    inout_stream << m_Resources[i].m_sOptionalNiceLookupName;
    inout_stream << m_Resources[i].m_sResourceID;
    inout_stream << m_Resources[i].m_uiFileSize;
  }
}

void wdCollectionResourceDescriptor::Load(wdStreamReader& inout_stream)
{
  wdUInt8 uiVersion = 0;
  wdUInt8 uiIdentifier = 0;
  wdUInt32 uiNumResources = 0;

  inout_stream >> uiVersion;
  inout_stream >> uiIdentifier;

  if (uiVersion == 1)
  {
    wdUInt16 uiNumResourcesShort;
    inout_stream >> uiNumResourcesShort;
    uiNumResources = uiNumResourcesShort;
  }
  else
  {
    inout_stream >> uiNumResources;
  }

  WD_ASSERT_DEV(uiIdentifier == 0xC0, "File does not contain a valid wdCollectionResourceDescriptor");
  WD_ASSERT_DEV(uiVersion > 0 && uiVersion <= 3, "Invalid file version {0}", uiVersion);

  m_Resources.SetCount(uiNumResources);

  for (wdUInt32 i = 0; i < uiNumResources; ++i)
  {
    inout_stream >> m_Resources[i].m_sAssetTypeName;
    inout_stream >> m_Resources[i].m_sOptionalNiceLookupName;
    inout_stream >> m_Resources[i].m_sResourceID;
    if (uiVersion >= 3)
    {
      inout_stream >> m_Resources[i].m_uiFileSize;
    }
  }
}



WD_STATICLINK_FILE(Core, Core_Collection_Implementation_CollectionResource);
