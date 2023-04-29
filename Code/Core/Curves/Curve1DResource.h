#pragma once

#include <Core/ResourceManager/Resource.h>
#include <Foundation/Tracks/Curve1D.h>

/// \brief A curve resource can contain more than one curve, but all of the same type.
struct WD_CORE_DLL wdCurve1DResourceDescriptor
{
  wdDynamicArray<wdCurve1D> m_Curves;

  void Save(wdStreamWriter& inout_stream) const;
  void Load(wdStreamReader& inout_stream);
};

using wdCurve1DResourceHandle = wdTypedResourceHandle<class wdCurve1DResource>;

/// \brief A resource that stores 1D curves. The curves are stored in the descriptor.
class WD_CORE_DLL wdCurve1DResource : public wdResource
{
  WD_ADD_DYNAMIC_REFLECTION(wdCurve1DResource, wdResource);
  WD_RESOURCE_DECLARE_COMMON_CODE(wdCurve1DResource);
  WD_RESOURCE_DECLARE_CREATEABLE(wdCurve1DResource, wdCurve1DResourceDescriptor);

public:
  wdCurve1DResource();

  /// \brief Returns all the data that is stored in this resource.
  const wdCurve1DResourceDescriptor& GetDescriptor() const { return m_Descriptor; }

private:
  virtual wdResourceLoadDesc UnloadData(Unload WhatToUnload) override;
  virtual wdResourceLoadDesc UpdateContent(wdStreamReader* Stream) override;
  virtual void UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage) override;

  wdCurve1DResourceDescriptor m_Descriptor;
};
