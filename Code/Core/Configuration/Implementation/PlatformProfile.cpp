#include <Core/CorePCH.h>

#include <Core/Configuration/PlatformProfile.h>
#include <Foundation/IO/ChunkStream.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/Reflection/ReflectionUtils.h>

#include <Core/ResourceManager/ResourceManager.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_ENUM(wdProfileTargetPlatform, 1)
  WD_ENUM_CONSTANTS(wdProfileTargetPlatform::PC, wdProfileTargetPlatform::UWP, wdProfileTargetPlatform::Android)
WD_END_STATIC_REFLECTED_ENUM;
// clang-format on

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdProfileConfigData, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE
// clang-format on

wdProfileConfigData::wdProfileConfigData() = default;
wdProfileConfigData::~wdProfileConfigData() = default;

void wdProfileConfigData::SaveRuntimeData(wdChunkStreamWriter& inout_stream) const {}
void wdProfileConfigData::LoadRuntimeData(wdChunkStreamReader& inout_stream) {}

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdPlatformProfile, 1, wdRTTIDefaultAllocator<wdPlatformProfile>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Name", m_sName)->AddAttributes(new wdHiddenAttribute()),
    WD_ENUM_MEMBER_PROPERTY("Platform", wdProfileTargetPlatform, m_TargetPlatform),
    WD_ARRAY_MEMBER_PROPERTY("Configs", m_Configs)->AddFlags(wdPropertyFlags::PointerOwner)->AddAttributes(new wdContainerAttribute(false, false, false)),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdPlatformProfile::wdPlatformProfile() = default;

wdPlatformProfile::~wdPlatformProfile()
{
  Clear();
}

void wdPlatformProfile::Clear()
{
  for (auto pType : m_Configs)
  {
    pType->GetDynamicRTTI()->GetAllocator()->Deallocate(pType);
  }

  m_Configs.Clear();
}

void wdPlatformProfile::AddMissingConfigs()
{
  for (auto pRtti = wdRTTI::GetFirstInstance(); pRtti != nullptr; pRtti = pRtti->GetNextInstance())
  {
    // find all types derived from wdProfileConfigData
    if (!pRtti->GetTypeFlags().IsAnySet(wdTypeFlags::Abstract) && pRtti->IsDerivedFrom<wdProfileConfigData>() && pRtti->GetAllocator()->CanAllocate())
    {
      bool bHasTypeAlready = false;

      // check whether we already have an instance of this type
      for (auto pType : m_Configs)
      {
        if (pType && pType->GetDynamicRTTI() == pRtti)
        {
          bHasTypeAlready = true;
          break;
        }
      }

      if (!bHasTypeAlready)
      {
        // if not, allocate one
        wdProfileConfigData* pObject = pRtti->GetAllocator()->Allocate<wdProfileConfigData>();
        WD_ASSERT_DEV(pObject != nullptr, "Invalid profile config");
        wdReflectionUtils::SetAllMemberPropertiesToDefault(pRtti, pObject);

        m_Configs.PushBack(pObject);
      }
    }
  }

  // sort all configs alphabetically
  m_Configs.Sort([](const wdProfileConfigData* lhs, const wdProfileConfigData* rhs) -> bool { return wdStringUtils::Compare(lhs->GetDynamicRTTI()->GetTypeName(), rhs->GetDynamicRTTI()->GetTypeName()) < 0; });
}

const wdProfileConfigData* wdPlatformProfile::GetTypeConfig(const wdRTTI* pRtti) const
{
  for (const auto* pConfig : m_Configs)
  {
    if (pConfig->GetDynamicRTTI() == pRtti)
      return pConfig;
  }

  return nullptr;
}

wdProfileConfigData* wdPlatformProfile::GetTypeConfig(const wdRTTI* pRtti)
{
  // reuse the const-version
  return const_cast<wdProfileConfigData*>(((const wdPlatformProfile*)this)->GetTypeConfig(pRtti));
}

wdResult wdPlatformProfile::SaveForRuntime(wdStringView sFile) const
{
  wdFileWriter file;
  WD_SUCCEED_OR_RETURN(file.Open(sFile));

  wdChunkStreamWriter chunk(file);

  chunk.BeginStream(1);

  for (auto* pConfig : m_Configs)
  {
    pConfig->SaveRuntimeData(chunk);
  }

  chunk.EndStream();

  return WD_SUCCESS;
}

wdResult wdPlatformProfile::LoadForRuntime(wdStringView sFile)
{
  wdFileReader file;
  WD_SUCCEED_OR_RETURN(file.Open(sFile));

  wdChunkStreamReader chunk(file);

  chunk.BeginStream();

  while (chunk.GetCurrentChunk().m_bValid)
  {
    for (auto* pConfig : m_Configs)
    {
      pConfig->LoadRuntimeData(chunk);
    }

    chunk.NextChunk();
  }

  chunk.EndStream();

  return WD_SUCCESS;
}



WD_STATICLINK_FILE(Core, Core_Configuration_Implementation_PlatformProfile);
