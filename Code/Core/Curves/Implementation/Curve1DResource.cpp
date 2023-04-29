#include <Core/CorePCH.h>

#include <Core/Assets/AssetFileHeader.h>
#include <Core/Curves/Curve1DResource.h>

WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdCurve1DResource, 1, wdRTTIDefaultAllocator<wdCurve1DResource>)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_RESOURCE_IMPLEMENT_COMMON_CODE(wdCurve1DResource);

wdCurve1DResource::wdCurve1DResource()
  : wdResource(DoUpdate::OnAnyThread, 1)
{
}

WD_RESOURCE_IMPLEMENT_CREATEABLE(wdCurve1DResource, wdCurve1DResourceDescriptor)
{
  m_Descriptor = descriptor;

  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Loaded;

  return res;
}

wdResourceLoadDesc wdCurve1DResource::UnloadData(Unload WhatToUnload)
{
  wdResourceLoadDesc res;
  res.m_uiQualityLevelsDiscardable = 0;
  res.m_uiQualityLevelsLoadable = 0;
  res.m_State = wdResourceState::Unloaded;

  m_Descriptor.m_Curves.Clear();

  return res;
}

wdResourceLoadDesc wdCurve1DResource::UpdateContent(wdStreamReader* Stream)
{
  WD_LOG_BLOCK("wdCurve1DResource::UpdateContent", GetResourceDescription().GetData());

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

void wdCurve1DResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  out_NewMemoryUsage.m_uiMemoryGPU = 0;
  out_NewMemoryUsage.m_uiMemoryCPU = static_cast<wdUInt32>(m_Descriptor.m_Curves.GetHeapMemoryUsage()) + static_cast<wdUInt32>(sizeof(m_Descriptor));

  for (const auto& curve : m_Descriptor.m_Curves)
  {
    out_NewMemoryUsage.m_uiMemoryCPU += curve.GetHeapMemoryUsage();
  }
}

void wdCurve1DResourceDescriptor::Save(wdStreamWriter& inout_stream) const
{
  const wdUInt8 uiVersion = 1;

  inout_stream << uiVersion;

  const wdUInt8 uiCurves = static_cast<wdUInt8>(m_Curves.GetCount());
  inout_stream << uiCurves;

  for (wdUInt32 i = 0; i < uiCurves; ++i)
  {
    m_Curves[i].Save(inout_stream);
  }
}

void wdCurve1DResourceDescriptor::Load(wdStreamReader& inout_stream)
{
  wdUInt8 uiVersion = 0;

  inout_stream >> uiVersion;

  WD_ASSERT_DEV(uiVersion == 1, "Invalid file version {0}", uiVersion);

  wdUInt8 uiCurves = 0;
  inout_stream >> uiCurves;

  m_Curves.SetCount(uiCurves);

  for (wdUInt32 i = 0; i < uiCurves; ++i)
  {
    m_Curves[i].Load(inout_stream);

    /// \todo We can do this on load, or somehow ensure this is always already correctly saved
    m_Curves[i].SortControlPoints();
    m_Curves[i].CreateLinearApproximation();
  }
}



WD_STATICLINK_FILE(Core, Core_Curves_Implementation_Curve1DResource);
