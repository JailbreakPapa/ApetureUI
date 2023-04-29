#pragma once

#include <Core/ResourceManager/Resource.h>
#include <Core/ResourceManager/ResourceTypeLoader.h>
#include <Foundation/CodeUtils/Preprocessor.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/IO/DependencyFile.h>
#include <Foundation/Strings/HashedString.h>
#include <Utilities/UtilitiesDLL.h>

using wdConfigFileResourceHandle = wdTypedResourceHandle<class wdConfigFileResource>;

/// \brief This resource loads config files containing key/value pairs
///
/// The config files usually use the file extension '.wdConfig'.
///
/// The file format looks like this:
///
/// To declare a key/value pair for the first time, write its type, name and value:
///   int i = 1
///   float f = 2.3
///   bool b = false
///   string s = "hello"
///
/// To set a variable to a different value than before, it has to be marked with 'override':
///
///   override i = 4
///
/// The format supports C preprocessor features like #include, #define, #ifdef, etc.
/// This can be used to build hierarchical config files:
///
///   #include "BaseConfig.wdConfig"
///   override int SomeValue = 7
///
/// It can also be used to define 'enum types':
///
///   #define SmallValue 3
///   #define BigValue 5
///   int MyValue = BigValue
///
/// Since resources can be reloaded at runtime, config resources are a convenient way to define game parameters
/// that you may want to tweak at any time.
/// Using C preprocessor logic (#define, #if, #else, etc) you can quickly select between different configuration sets.
///
/// Once loaded, accessing the data is very efficient.
class WD_UTILITIES_DLL wdConfigFileResource : public wdResource
{
  WD_ADD_DYNAMIC_REFLECTION(wdConfigFileResource, wdResource);

  WD_RESOURCE_DECLARE_COMMON_CODE(wdConfigFileResource);

public:
  wdConfigFileResource();
  ~wdConfigFileResource();

  /// \brief Returns the 'int' variable with the given name. Logs an error, if the variable doesn't exist in the config file.
  wdInt32 GetInt(wdTempHashedString sName) const;

  /// \brief Returns the 'float' variable with the given name. Logs an error, if the variable doesn't exist in the config file.
  float GetFloat(wdTempHashedString sName) const;

  /// \brief Returns the 'bool' variable with the given name. Logs an error, if the variable doesn't exist in the config file.
  bool GetBool(wdTempHashedString sName) const;

  /// \brief Returns the 'string' variable with the given name. Logs an error, if the variable doesn't exist in the config file.
  const char* GetString(wdTempHashedString sName) const;

  /// \brief Returns the 'int' variable with the given name. Returns the 'fallback' value, if the variable doesn't exist in the config file.
  wdInt32 GetInt(wdTempHashedString sName, wdInt32 iFallback) const;

  /// \brief Returns the 'float' variable with the given name. Returns the 'fallback' value, if the variable doesn't exist in the config file.
  float GetFloat(wdTempHashedString sName, float fFallback) const;

  /// \brief Returns the 'bool' variable with the given name. Returns the 'fallback' value, if the variable doesn't exist in the config file.
  bool GetBool(wdTempHashedString sName, bool bFallback) const;

  /// \brief Returns the 'string' variable with the given name. Returns the 'fallback' value, if the variable doesn't exist in the config file.
  const char* GetString(wdTempHashedString sName, const char* szFallback) const;

protected:
  virtual wdResourceLoadDesc UnloadData(Unload WhatToUnload) override;
  virtual wdResourceLoadDesc UpdateContent(wdStreamReader* Stream) override;
  virtual void UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage) override;

private:
  friend class wdConfigFileResourceLoader;

  wdHashTable<wdHashedString, wdInt32> m_IntData;
  wdHashTable<wdHashedString, float> m_FloatData;
  wdHashTable<wdHashedString, wdString> m_StringData;
  wdHashTable<wdHashedString, bool> m_BoolData;

  wdDependencyFile m_RequiredFiles;
};


class WD_UTILITIES_DLL wdConfigFileResourceLoader : public wdResourceTypeLoader
{
public:
  struct LoadedData
  {
    LoadedData()
      : m_Reader(&m_Storage)
    {
    }

    wdDefaultMemoryStreamStorage m_Storage;
    wdMemoryStreamReader m_Reader;
    wdDependencyFile m_RequiredFiles;

    wdResult PrePropFileLocator(const char* szCurAbsoluteFile, const char* szIncludeFile, wdPreprocessor::IncludeType incType, wdStringBuilder& out_sAbsoluteFilePath);
  };

  virtual wdResourceLoadData OpenDataStream(const wdResource* pResource) override;
  virtual void CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData) override;
  virtual bool IsResourceOutdated(const wdResource* pResource) const override;
};
