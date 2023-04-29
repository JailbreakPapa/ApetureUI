#include <Core/CorePCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Core/Curves/ColorGradientResource.h>

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdColorGradientResource, 1, wdRTTIDefaultAllocator<wdColorGradientResource>)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_RESOURCE_IMPLEMENT_COMMON_CODE(wdColorGradientResource);

wdColorGradientResource::wdColorGradientResource()
  : wdResource(DoUpdate::OnAnyThread, 1)
{
}

WD_RESOURCE_IMPLEMENT_CREATEABLE(wdColorGradientResource, wdColorGradientResourceDescriptor)
{
  m_Descriptor = descriptor;

  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Loaded;

  return res;
}

wdResourceLoadDesc wdColorGradientResource::UnloadData(Unload WhatToUnload)
{
  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Unloaded;

  m_Descriptor.m_Gradient.Clear();

  return res;
}

wdResourceLoadDesc wdColorGradientResource::UpdateContent(wdStreamReader* Stream)
{
  WD_LOG_BLOCK("wdColorGradientResource::UpdateContent", GetResourceDescription().GetData());

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

  m_Descriptor.Load(*Stream);

  res.m_State = wdResourceState::Loaded;
  return res;
}

void wdColorGradientResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  out_NewMemoryUsage.m_uiMemoryGPU = 0;
  out_NewMemoryUsage.m_uiMemoryCPU = static_cast<wdUInt32>(m_Descriptor.m_Gradient.GetHeapMemoryUsage()) + static_cast<wdUInt32>(sizeof(m_Descriptor));
}

void wdColorGradientResourceDescriptor::Save(wdStreamWriter& inout_stream) const
{
  const wdUInt8 uiVersion = 1;

  inout_stream << uiVersion;

  m_Gradient.Save(inout_stream);
}

void wdColorGradientResourceDescriptor::Load(wdStreamReader& inout_stream)
{
  wdUInt8 uiVersion = 0;

  inout_stream >> uiVersion;

  WD_ASSERT_DEV(uiVersion == 1, "Invalid file version {0}", uiVersion);

  m_Gradient.Load(inout_stream);
}



WD_STATICLINK_FILE(Core, Core_Curves_Implementation_ColorGradientResource);
