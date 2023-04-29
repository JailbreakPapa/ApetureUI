#include <Utilities/UtilitiesPCH.h>

#include <Foundation/CodeUtils/Preprocessor.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>
#include <Utilities/Resources/ConfigFileResource.h>

static wdConfigFileResourceLoader s_ConfigFileResourceLoader;

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Utilties, ConfigFileResource)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    wdResourceManager::SetResourceTypeLoader<wdConfigFileResource>(&s_ConfigFileResourceLoader);

    auto hFallback = wdResourceManager::LoadResource<wdConfigFileResource>("Empty.wdConfig");
    wdResourceManager::SetResourceTypeMissingFallback<wdConfigFileResource>(hFallback);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    wdResourceManager::SetResourceTypeMissingFallback<wdConfigFileResource>(wdConfigFileResourceHandle());
    wdResourceManager::SetResourceTypeLoader<wdConfigFileResource>(nullptr);
    wdConfigFileResource::CleanupDynamicPluginReferences();
  }

  WD_END_SUBSYSTEM_DECLARATION;
// clang-format on

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdConfigFileResource, 1, wdRTTIDefaultAllocator<wdConfigFileResource>)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

WD_RESOURCE_IMPLEMENT_COMMON_CODE(wdConfigFileResource);

wdConfigFileResource::wdConfigFileResource()
  : wdResource(wdResource::DoUpdate::OnAnyThread, 0)
{
}

wdConfigFileResource::~wdConfigFileResource() = default;

wdInt32 wdConfigFileResource::GetInt(wdTempHashedString sName, wdInt32 iFallback) const
{
  auto it = m_IntData.Find(sName);
  if (it.IsValid())
    return it.Value();

  return iFallback;
}

wdInt32 wdConfigFileResource::GetInt(wdTempHashedString sName) const
{
  auto it = m_IntData.Find(sName);
  if (it.IsValid())
    return it.Value();

  wdLog::Error("{}: 'int' config variable (name hash = {}) doesn't exist.", this->GetResourceDescription(), sName.GetHash());
  return 0;
}

float wdConfigFileResource::GetFloat(wdTempHashedString sName, float fFallback) const
{
  auto it = m_FloatData.Find(sName);
  if (it.IsValid())
    return it.Value();

  return fFallback;
}

float wdConfigFileResource::GetFloat(wdTempHashedString sName) const
{
  auto it = m_FloatData.Find(sName);
  if (it.IsValid())
    return it.Value();

  wdLog::Error("{}: 'float' config variable (name hash = {}) doesn't exist.", this->GetResourceDescription(), sName.GetHash());
  return 0;
}

bool wdConfigFileResource::GetBool(wdTempHashedString sName, bool bFallback) const
{
  auto it = m_BoolData.Find(sName);
  if (it.IsValid())
    return it.Value();

  return bFallback;
}

bool wdConfigFileResource::GetBool(wdTempHashedString sName) const
{
  auto it = m_BoolData.Find(sName);
  if (it.IsValid())
    return it.Value();

  wdLog::Error("{}: 'float' config variable (name hash = {}) doesn't exist.", this->GetResourceDescription(), sName.GetHash());
  return false;
}

const char* wdConfigFileResource::GetString(wdTempHashedString sName, const char* szFallback) const
{
  auto it = m_StringData.Find(sName);
  if (it.IsValid())
    return it.Value();

  return szFallback;
}

const char* wdConfigFileResource::GetString(wdTempHashedString sName) const
{
  auto it = m_StringData.Find(sName);
  if (it.IsValid())
    return it.Value();

  wdLog::Error("{}: 'string' config variable '(name hash = {}) doesn't exist.", this->GetResourceDescription(), sName.GetHash());
  return "";
}

wdResourceLoadDesc wdConfigFileResource::UnloadData(Unload WhatToUnload)
{
  m_IntData.Clear();
  m_FloatData.Clear();
  m_StringData.Clear();
  m_BoolData.Clear();

  wdResourceLoadDesc d;
  d.m_State = wdResourceState::Unloaded;
  d.m_uiQualityLevelsDiscardable = 0;
  d.m_uiQualityLevelsLoadable = 0;
  return d;
}

wdResourceLoadDesc wdConfigFileResource::UpdateContent(wdStreamReader* Stream)
{
  wdResourceLoadDesc d;
  d.m_uiQualityLevelsDiscardable = 0;
  d.m_uiQualityLevelsLoadable = 0;
  d.m_State = wdResourceState::Loaded;

  if (Stream == nullptr)
  {
    d.m_State = wdResourceState::LoadedResourceMissing;
    return d;
  }

  m_RequiredFiles.ReadDependencyFile(*Stream).IgnoreResult();
  Stream->ReadHashTable(m_IntData).IgnoreResult();
  Stream->ReadHashTable(m_FloatData).IgnoreResult();
  Stream->ReadHashTable(m_StringData).IgnoreResult();
  Stream->ReadHashTable(m_BoolData).IgnoreResult();

  return d;
}

void wdConfigFileResource::UpdateMemoryUsage(MemoryUsage& out_NewMemoryUsage)
{
  out_NewMemoryUsage.m_uiMemoryCPU = m_IntData.GetHeapMemoryUsage() + m_FloatData.GetHeapMemoryUsage() + m_StringData.GetHeapMemoryUsage() + m_BoolData.GetHeapMemoryUsage();
  out_NewMemoryUsage.m_uiMemoryGPU = 0;
}

//////////////////////////////////////////////////////////////////////////

wdResult wdConfigFileResourceLoader::LoadedData::PrePropFileLocator(const char* szCurAbsoluteFile, const char* szIncludeFile, wdPreprocessor::IncludeType incType, wdStringBuilder& out_sAbsoluteFilePath)
{
  wdResult res = wdPreprocessor::DefaultFileLocator(szCurAbsoluteFile, szIncludeFile, incType, out_sAbsoluteFilePath);

  m_RequiredFiles.AddFileDependency(out_sAbsoluteFilePath);

  return res;
}

wdResourceLoadData wdConfigFileResourceLoader::OpenDataStream(const wdResource* pResource)
{
  WD_PROFILE_SCOPE("ReadResourceFile");
  WD_LOG_BLOCK("Load Config Resource", pResource->GetResourceID());

  wdStringBuilder sConfig;

  wdMap<wdString, wdInt32> intData;
  wdMap<wdString, float> floatData;
  wdMap<wdString, wdString> stringData;
  wdMap<wdString, bool> boolData;

  LoadedData* pData = WD_DEFAULT_NEW(LoadedData);
  pData->m_Reader.SetStorage(&pData->m_Storage);

  wdPreprocessor preprop;

  // used to gather all the transitive file dependencies
  preprop.SetFileLocatorFunction(wdMakeDelegate(&wdConfigFileResourceLoader::LoadedData::PrePropFileLocator, pData));

  if (wdStringUtils::IsEqual(pResource->GetResourceID(), "Empty.wdConfig"))
  {
    // do nothing
  }
  else if (preprop.Process(pResource->GetResourceID(), sConfig, false, true, false).Succeeded())
  {
    sConfig.ReplaceAll("\r", "");
    sConfig.ReplaceAll("\n", ";");

    wdHybridArray<wdStringView, 32> lines;
    sConfig.Split(false, lines, ";");

    wdStringBuilder key, value, line;

    for (wdStringView tmp : lines)
    {
      line = tmp;
      line.Trim(" \t");

      if (line.IsEmpty())
        continue;

      const char* szAssign = line.FindSubString("=");

      if (szAssign == nullptr)
      {
        wdLog::Error("Invalid line in config file: '{}'", tmp);
      }
      else
      {
        value = szAssign + 1;
        value.Trim(" ");

        line.SetSubString_FromTo(line.GetData(), szAssign);
        line.ReplaceAll("\t", " ");
        line.ReplaceAll("  ", " ");
        line.Trim(" ");

        const bool bOverride = line.TrimWordStart("override ");
        line.Trim(" ");

        if (line.StartsWith("int "))
        {
          key.SetSubString_FromTo(line.GetData() + 4, szAssign);
          key.Trim(" ");

          if (bOverride && !intData.Contains(key))
            wdLog::Error("Config 'int' key '{}' is marked override, but doesn't exist yet. Remove 'override' keyword.", key);
          if (!bOverride && intData.Contains(key))
            wdLog::Error("Config 'int' key '{}' is not marked override, but exist already. Use 'override int' instead.", key);

          wdInt32 val;
          if (wdConversionUtils::StringToInt(value, val).Succeeded())
          {
            intData[key] = val;
          }
          else
          {
            wdLog::Error("Failed to parse 'int' in config file: '{}'", tmp);
          }
        }
        else if (line.StartsWith("float "))
        {
          key.SetSubString_FromTo(line.GetData() + 6, szAssign);
          key.Trim(" ");

          if (bOverride && !floatData.Contains(key))
            wdLog::Error("Config 'float' key '{}' is marked override, but doesn't exist yet. Remove 'override' keyword.", key);
          if (!bOverride && floatData.Contains(key))
            wdLog::Error("Config 'float' key '{}' is not marked override, but exist already. Use 'override float' instead.", key);

          double val;
          if (wdConversionUtils::StringToFloat(value, val).Succeeded())
          {
            floatData[key] = (float)val;
          }
          else
          {
            wdLog::Error("Failed to parse 'float' in config file: '{}'", tmp);
          }
        }
        else if (line.StartsWith("bool "))
        {
          key.SetSubString_FromTo(line.GetData() + 5, szAssign);
          key.Trim(" ");

          if (bOverride && !boolData.Contains(key))
            wdLog::Error("Config 'bool' key '{}' is marked override, but doesn't exist yet. Remove 'override' keyword.", key);
          if (!bOverride && boolData.Contains(key))
            wdLog::Error("Config 'bool' key '{}' is not marked override, but exist already. Use 'override bool' instead.", key);

          bool val;
          if (wdConversionUtils::StringToBool(value, val).Succeeded())
          {
            boolData[key] = val;
          }
          else
          {
            wdLog::Error("Failed to parse 'bool' in config file: '{}'", tmp);
          }
        }
        else if (line.StartsWith("string "))
        {
          key.SetSubString_FromTo(line.GetData() + 7, szAssign);
          key.Trim(" ");

          if (bOverride && !stringData.Contains(key))
            wdLog::Error("Config 'string' key '{}' is marked override, but doesn't exist yet. Remove 'override' keyword.", key);
          if (!bOverride && stringData.Contains(key))
            wdLog::Error("Config 'string' key '{}' is not marked override, but exist already. Use 'override string' instead.", key);

          if (!value.StartsWith("\"") || !value.EndsWith("\""))
          {
            wdLog::Error("Failed to parse 'string' in config file: '{}'", tmp);
          }
          else
          {
            value.Shrink(1, 1);
            stringData[key] = value;
          }
        }
        else
        {
          wdLog::Error("Invalid line in config file: '{}'", tmp);
        }
      }
    }
  }
  else
  {
    // empty stream
    return {};
  }

  wdResourceLoadData res;
  res.m_pDataStream = &pData->m_Reader;
  res.m_pCustomLoaderData = pData;

#if WD_ENABLED(WD_SUPPORTS_FILE_STATS)
  wdFileStats stat;
  if (wdFileSystem::GetFileStats(pResource->GetResourceID(), stat).Succeeded())
  {
    res.m_sResourceDescription = stat.m_sName;
    res.m_LoadedFileModificationDate = stat.m_LastModificationTime;
  }
#endif

  wdMemoryStreamWriter writer(&pData->m_Storage);

  pData->m_RequiredFiles.StoreCurrentTimeStamp();
  pData->m_RequiredFiles.WriteDependencyFile(writer).IgnoreResult();
  writer.WriteMap(intData).IgnoreResult();
  writer.WriteMap(floatData).IgnoreResult();
  writer.WriteMap(stringData).IgnoreResult();
  writer.WriteMap(boolData).IgnoreResult();

  return res;
}

void wdConfigFileResourceLoader::CloseDataStream(const wdResource* pResource, const wdResourceLoadData& loaderData)
{
  LoadedData* pData = static_cast<LoadedData*>(loaderData.m_pCustomLoaderData);

  WD_DEFAULT_DELETE(pData);
}

bool wdConfigFileResourceLoader::IsResourceOutdated(const wdResource* pResource) const
{
  return static_cast<const wdConfigFileResource*>(pResource)->m_RequiredFiles.HasAnyFileChanged();
}


WD_STATICLINK_FILE(Utilities, Utilities_Resources_ConfigFileResource);
