#include <Core/CorePCH.h>

#include <Core/World/WorldModule.h>
#include <Core/World/WorldModuleConfig.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/IO/OpenDdlReader.h>
#include <Foundation/IO/OpenDdlUtils.h>
#include <Foundation/IO/OpenDdlWriter.h>

wdResult wdWorldModuleConfig::Save()
{
  m_InterfaceImpls.Sort();

  wdStringBuilder sPath;
  sPath = ":project/WorldModules.ddl";

  wdFileWriter file;
  if (file.Open(sPath).Failed())
    return WD_FAILURE;

  wdOpenDdlWriter writer;
  writer.SetOutputStream(&file);
  writer.SetCompactMode(false);
  writer.SetPrimitiveTypeStringMode(wdOpenDdlWriter::TypeStringMode::Compliant);

  for (auto& interfaceImpl : m_InterfaceImpls)
  {
    writer.BeginObject("InterfaceImpl");

    wdOpenDdlUtils::StoreString(writer, interfaceImpl.m_sInterfaceName, "Interface");
    wdOpenDdlUtils::StoreString(writer, interfaceImpl.m_sImplementationName, "Implementation");

    writer.EndObject();
  }

  return WD_SUCCESS;
}

void wdWorldModuleConfig::Load()
{
  const char* szPath = ":project/WorldModules.ddl";

  WD_LOG_BLOCK("wdWorldModuleConfig::Load()", szPath);

  m_InterfaceImpls.Clear();

  wdFileReader file;
  if (file.Open(szPath).Failed())
  {
    wdLog::Dev("World module config file is not available: '{0}'", szPath);
    return;
  }
  else
  {
    wdLog::Success("World module config file is available: '{0}'", szPath);
  }

  wdOpenDdlReader reader;
  if (reader.ParseDocument(file, 0, wdLog::GetThreadLocalLogSystem()).Failed())
  {
    wdLog::Error("Failed to parse world module config file '{0}'", szPath);
    return;
  }

  const wdOpenDdlReaderElement* pTree = reader.GetRootElement();

  for (const wdOpenDdlReaderElement* pInterfaceImpl = pTree->GetFirstChild(); pInterfaceImpl != nullptr;
       pInterfaceImpl = pInterfaceImpl->GetSibling())
  {
    if (!pInterfaceImpl->IsCustomType("InterfaceImpl"))
      continue;

    const wdOpenDdlReaderElement* pInterface = pInterfaceImpl->FindChildOfType(wdOpenDdlPrimitiveType::String, "Interface");
    const wdOpenDdlReaderElement* pImplementation = pInterfaceImpl->FindChildOfType(wdOpenDdlPrimitiveType::String, "Implementation");

    // this prevents duplicates
    AddInterfaceImplementation(pInterface->GetPrimitivesString()[0], pImplementation->GetPrimitivesString()[0]);
  }
}

void wdWorldModuleConfig::Apply()
{
  WD_LOG_BLOCK("wdWorldModuleConfig::Apply");

  for (const auto& interfaceImpl : m_InterfaceImpls)
  {
    wdWorldModuleFactory::GetInstance()->RegisterInterfaceImplementation(interfaceImpl.m_sInterfaceName, interfaceImpl.m_sImplementationName);
  }
}

void wdWorldModuleConfig::AddInterfaceImplementation(wdStringView sInterfaceName, wdStringView sImplementationName)
{
  for (auto& interfaceImpl : m_InterfaceImpls)
  {
    if (interfaceImpl.m_sInterfaceName == sInterfaceName)
    {
      interfaceImpl.m_sImplementationName = sImplementationName;
      return;
    }
  }

  m_InterfaceImpls.PushBack({sInterfaceName, sImplementationName});
}

void wdWorldModuleConfig::RemoveInterfaceImplementation(wdStringView sInterfaceName)
{
  for (wdUInt32 i = 0; i < m_InterfaceImpls.GetCount(); ++i)
  {
    if (m_InterfaceImpls[i].m_sInterfaceName == sInterfaceName)
    {
      m_InterfaceImpls.RemoveAtAndCopy(i);
      return;
    }
  }
}


WD_STATICLINK_FILE(Core, Core_World_Implementation_WorldModuleConfig);
