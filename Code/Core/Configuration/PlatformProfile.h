#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Reflection/Reflection.h>

class wdChunkStreamWriter;
class wdChunkStreamReader;

struct wdProfileTargetPlatform
{
  enum Enum
  {
    PC,
    UWP,
    Android,

    Default = PC
  };

  typedef wdUInt8 StorageType;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdProfileTargetPlatform);

//////////////////////////////////////////////////////////////////////////

/// \brief Base class for configuration objects that store e.g. asset transform settings or runtime configuration information
class WD_CORE_DLL wdProfileConfigData : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdProfileConfigData, wdReflectedClass);

public:
  wdProfileConfigData();
  ~wdProfileConfigData();

  virtual void SaveRuntimeData(wdChunkStreamWriter& inout_stream) const;
  virtual void LoadRuntimeData(wdChunkStreamReader& inout_stream);
};

//////////////////////////////////////////////////////////////////////////

class WD_CORE_DLL wdPlatformProfile : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdPlatformProfile, wdReflectedClass);

public:
  wdPlatformProfile();
  ~wdPlatformProfile();

  wdStringView GetConfigName() const { return m_sName; }

  void Clear();
  void AddMissingConfigs();

  template <typename TYPE>
  const TYPE* GetTypeConfig() const
  {
    return static_cast<const TYPE*>(GetTypeConfig(wdGetStaticRTTI<TYPE>()));
  }

  template <typename TYPE>
  TYPE* GetTypeConfig()
  {
    return static_cast<TYPE*>(GetTypeConfig(wdGetStaticRTTI<TYPE>()));
  }

  const wdProfileConfigData* GetTypeConfig(const wdRTTI* pRtti) const;
  wdProfileConfigData* GetTypeConfig(const wdRTTI* pRtti);

  wdResult SaveForRuntime(wdStringView sFile) const;
  wdResult LoadForRuntime(wdStringView sFile);

  wdString m_sName;
  wdEnum<wdProfileTargetPlatform> m_TargetPlatform;
  wdDynamicArray<wdProfileConfigData*> m_Configs;
};

//////////////////////////////////////////////////////////////////////////
