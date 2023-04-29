#pragma once

#include <Core/ResourceManager/Resource.h>
#include <Foundation/Tracks/ColorGradient.h>

struct WD_CORE_DLL wdColorGradientResourceDescriptor
{
  wdColorGradient m_Gradient;

  void Save(wdStreamWriter& inout_stream) const;
  void Load(wdStreamReader& inout_stream);
};

using wdColorGradientResourceHandle = wdTypedResourceHandle<class wdColorGradientResource>;

/// \brief A resource that stores a single color gradient. The data is stored in the descriptor.
class WD_CORE_DLL wdColorGradientResource : public wdResource
{
  WD_ADD_DYNAMIC_REFLECTION(wdColorGradientResource, wdResource);
  WD_RESOURCE_DECLARE_COMMON_CODE(wdColorGradientResource);
  WD_RESOURCE_DECLARE_CREATEABLE(wdColorGradientResource, wdColorGradientResourceDescriptor);

public:
  wdColorGradientResource();

  /// \brief Returns all the data that is stored in this resource.
  const wdColorGradientResourceDescriptor& GetDescriptor() const { return m_Descriptor; }

  inline wdColor Evaluate(double x) const
  {
    wdColor result;
    m_Descriptor.m_Gradient.Evaluate(x, result);
    return result;
  }

private:
  virtual wdResourceLoadDesc UnloadData(Unload WhatToUnload) override;
  virtual wdResourceLoadDesc UpdateContent(wdStreamReader* Stream) override;
  virtual void UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage) override;

  wdColorGradientResourceDescriptor m_Descriptor;
};
