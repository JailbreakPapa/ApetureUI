#include <Core/CorePCH.h>

#include <Core/World/World.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdWorldModule, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdWorldModule::wdWorldModule(wdWorld* pWorld)
  : m_pWorld(pWorld)
{
}

wdWorldModule::~wdWorldModule() {}

wdUInt32 wdWorldModule::GetWorldIndex() const
{
  return GetWorld()->GetIndex();
}

// protected methods

void wdWorldModule::RegisterUpdateFunction(const UpdateFunctionDesc& desc)
{
  m_pWorld->RegisterUpdateFunction(desc);
}

void wdWorldModule::DeregisterUpdateFunction(const UpdateFunctionDesc& desc)
{
  m_pWorld->DeregisterUpdateFunction(desc);
}

wdAllocatorBase* wdWorldModule::GetAllocator()
{
  return m_pWorld->GetAllocator();
}

wdInternal::WorldLargeBlockAllocator* wdWorldModule::GetBlockAllocator()
{
  return m_pWorld->GetBlockAllocator();
}

bool wdWorldModule::GetWorldSimulationEnabled() const
{
  return m_pWorld->GetWorldSimulationEnabled();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, WorldModuleFactory)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Reflection"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    wdPlugin::Events().AddEventHandler(wdWorldModuleFactory::PluginEventHandler);
    wdWorldModuleFactory::GetInstance()->FillBaseTypeIds();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    wdPlugin::Events().RemoveEventHandler(wdWorldModuleFactory::PluginEventHandler);
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on

static wdWorldModuleTypeId s_uiNextTypeId = 0;
static wdDynamicArray<wdWorldModuleTypeId> s_freeTypeIds;
static constexpr wdWorldModuleTypeId s_InvalidWorldModuleTypeId = wdWorldModuleTypeId(-1);

wdWorldModuleFactory::wdWorldModuleFactory() = default;

// static
wdWorldModuleFactory* wdWorldModuleFactory::GetInstance()
{
  static wdWorldModuleFactory* pInstance = new wdWorldModuleFactory();
  return pInstance;
}

wdWorldModuleTypeId wdWorldModuleFactory::GetTypeId(const wdRTTI* pRtti)
{
  wdWorldModuleTypeId uiTypeId = s_InvalidWorldModuleTypeId;
  m_TypeToId.TryGetValue(pRtti, uiTypeId);
  return uiTypeId;
}

wdWorldModule* wdWorldModuleFactory::CreateWorldModule(wdWorldModuleTypeId typeId, wdWorld* pWorld)
{
  if (typeId < m_CreatorFuncs.GetCount())
  {
    CreatorFunc func = m_CreatorFuncs[typeId].m_Func;
    return (*func)(pWorld->GetAllocator(), pWorld);
  }

  return nullptr;
}

void wdWorldModuleFactory::RegisterInterfaceImplementation(wdStringView sInterfaceName, wdStringView sImplementationName)
{
  m_InterfaceImplementations.Insert(sInterfaceName, sImplementationName);

  wdStringBuilder sTemp = sInterfaceName;
  const wdRTTI* pInterfaceRtti = wdRTTI::FindTypeByName(sTemp);

  sTemp = sImplementationName;
  const wdRTTI* pImplementationRtti = wdRTTI::FindTypeByName(sTemp);

  if (pInterfaceRtti != nullptr && pImplementationRtti != nullptr)
  {
    m_TypeToId[pInterfaceRtti] = m_TypeToId[pImplementationRtti];
    return;
  }

  // Clear existing mapping if it maps to the wrong type
  wdUInt16 uiTypeId;
  if (pInterfaceRtti != nullptr && m_TypeToId.TryGetValue(pInterfaceRtti, uiTypeId))
  {
    if (m_CreatorFuncs[uiTypeId].m_pRtti->GetTypeName() != sImplementationName)
    {
      WD_ASSERT_DEV(pImplementationRtti == nullptr, "Implementation error");
      m_TypeToId.Remove(pInterfaceRtti);
    }
  }
}
wdWorldModuleTypeId wdWorldModuleFactory::RegisterWorldModule(const wdRTTI* pRtti, CreatorFunc creatorFunc)
{
  WD_ASSERT_DEV(pRtti != wdGetStaticRTTI<wdWorldModule>(), "Trying to register a world module that is not reflected!");
  WD_ASSERT_DEV(
    m_TypeToId.GetCount() < wdWorld::GetMaxNumWorldModules(), "Max number of world modules reached: {}", wdWorld::GetMaxNumWorldModules());

  wdWorldModuleTypeId uiTypeId = s_InvalidWorldModuleTypeId;
  if (m_TypeToId.TryGetValue(pRtti, uiTypeId))
  {
    return uiTypeId;
  }

  if (s_freeTypeIds.IsEmpty())
  {
    WD_ASSERT_DEV(s_uiNextTypeId < WD_MAX_WORLD_MODULE_TYPES - 1, "World module id overflow!");

  uiTypeId = s_uiNextTypeId++;
  }
  else
  {
    uiTypeId = s_freeTypeIds.PeekBack();
    s_freeTypeIds.PopBack();
  }

  m_TypeToId.Insert(pRtti, uiTypeId);

  m_CreatorFuncs.EnsureCount(uiTypeId + 1);

  auto& creatorFuncContext = m_CreatorFuncs[uiTypeId];
  creatorFuncContext.m_Func = creatorFunc;
  creatorFuncContext.m_pRtti = pRtti;

  return uiTypeId;
}

// static
void wdWorldModuleFactory::PluginEventHandler(const wdPluginEvent& EventData)
{
  if (EventData.m_EventType == wdPluginEvent::AfterLoadingBeforeInit)
  {
    wdWorldModuleFactory::GetInstance()->FillBaseTypeIds();
  }

  if (EventData.m_EventType == wdPluginEvent::AfterUnloading)
  {
    wdWorldModuleFactory::GetInstance()->ClearUnloadedTypeToIDs();
  }
}

namespace
{
  struct NewEntry
  {
    WD_DECLARE_POD_TYPE();

    const wdRTTI* m_pRtti;
    wdWorldModuleTypeId m_uiTypeId;
  };
} // namespace

void wdWorldModuleFactory::AdjustBaseTypeId(const wdRTTI* pParentRtti, const wdRTTI* pRtti, wdUInt16 uiParentTypeId)
{
  wdDynamicArray<wdPlugin::PluginInfo> infos;
  wdPlugin::GetAllPluginInfos(infos);

  auto HasManualDependency = [&](const char* szPluginName) -> bool {
    for (const auto& p : infos)
    {
      if (p.m_sName == szPluginName)
      {
        return !p.m_LoadFlags.IsSet(wdPluginLoadFlags::CustomDependency);
      }
    }

    return false;
  };

  const char* szPlugin1 = m_CreatorFuncs[uiParentTypeId].m_pRtti->GetPluginName();
  const char* szPlugin2 = pRtti->GetPluginName();

  const bool bPrio1 = HasManualDependency(szPlugin1);
  const bool bPrio2 = HasManualDependency(szPlugin2);

  if (bPrio1 && !bPrio2)
  {
    // keep the previous one
    return;
  }

  if (!bPrio1 && bPrio2)
  {
    // take the new one
    m_TypeToId[pParentRtti] = m_TypeToId[pRtti];
    return;
  }

  wdLog::Error("Interface '{}' is already implemented by '{}'. Specify which implementation should be used via RegisterInterfaceImplementation() or WorldModules.ddl config file.", pParentRtti->GetTypeName(), m_CreatorFuncs[uiParentTypeId].m_pRtti->GetTypeName());
}

void wdWorldModuleFactory::FillBaseTypeIds()
{
  // m_TypeToId contains RTTI types for wdWorldModules and wdComponents
  // m_TypeToId[wdComponent] maps to TypeID for its respective wdComponentManager
  // m_TypeToId[wdWorldModule] maps to TypeID for itself OR in case of an interface to the derived type that implements the interface
  // after types are registered we only have a mapping for m_TypeToId[wdWorldModule(impl)] and now we want to add
  // the mapping for m_TypeToId[wdWorldModule(interface)], such that querying the TypeID for the interface works as well
  // and yields the implementation

  wdHybridArray<NewEntry, 64, wdStaticAllocatorWrapper> newEntries;
  const wdRTTI* pModuleRtti = wdGetStaticRTTI<wdWorldModule>(); // base type where we want to stop iterating upwards

  // explicit mappings
  for (auto it = m_InterfaceImplementations.GetIterator(); it.IsValid(); ++it)
  {
    const wdRTTI* pInterfaceRtti = wdRTTI::FindTypeByName(it.Key());
    const wdRTTI* pImplementationRtti = wdRTTI::FindTypeByName(it.Value());

    if (pInterfaceRtti != nullptr && pImplementationRtti != nullptr)
    {
      m_TypeToId[pInterfaceRtti] = m_TypeToId[pImplementationRtti];
    }
  }

  // automatic mappings
  for (auto it = m_TypeToId.GetIterator(); it.IsValid(); ++it)
  {
    const wdRTTI* pRtti = it.Key();

    // ignore components, we only want to fill out mappings for the base types of world modules
    if (!pRtti->IsDerivedFrom<wdWorldModule>())
      continue;

    const wdWorldModuleTypeId uiTypeId = it.Value();

    for (const wdRTTI* pParentRtti = pRtti->GetParentType(); pParentRtti != pModuleRtti; pParentRtti = pParentRtti->GetParentType())
    {
      // we are only interested in parent types that are pure interfaces
      if (!pParentRtti->GetTypeFlags().IsSet(wdTypeFlags::Abstract))
        continue;

      // skip if we have an explicit mapping for this interface, they are already handled above
      if (m_InterfaceImplementations.GetValue(pParentRtti->GetTypeName()) != nullptr)
        continue;


      if (wdUInt16* pParentTypeId = m_TypeToId.GetValue(pParentRtti))
      {
        if (*pParentTypeId != uiTypeId)
        {
          AdjustBaseTypeId(pParentRtti, pRtti, *pParentTypeId);
        }
      }
      else
      {
        auto& newEntry = newEntries.ExpandAndGetRef();
        newEntry.m_pRtti = pParentRtti;
        newEntry.m_uiTypeId = uiTypeId;
      }
    }
  }

  // delayed insertion to not interfere with the iteration above
  for (auto& newEntry : newEntries)
  {
    m_TypeToId.Insert(newEntry.m_pRtti, newEntry.m_uiTypeId);
  }
}

void wdWorldModuleFactory::ClearUnloadedTypeToIDs()
{
  wdSet<const wdRTTI*> allRttis;

  for (const wdRTTI* pRtti = wdRTTI::GetFirstInstance(); pRtti != nullptr; pRtti = pRtti->GetNextInstance())
  {
    allRttis.Insert(pRtti);
  }

  wdSet<wdWorldModuleTypeId> mappedIdsToRemove;

  for (auto it = m_TypeToId.GetIterator(); it.IsValid();)
  {
    const wdRTTI* pRtti = it.Key();
    const wdWorldModuleTypeId uiTypeId = it.Value();

    if (!allRttis.Contains(pRtti))
    {
      // type got removed, clear it from the map
      it = m_TypeToId.Remove(it);

      // and record that all other types that map to the same typeId also must be removed
      mappedIdsToRemove.Insert(uiTypeId);
    }
    else
    {
      ++it;
    }
  }

  // now remove all mappings that map to an invalid typeId
  // this can be more than one, since we can map multiple (interface) types to the same implementation
  for (auto it = m_TypeToId.GetIterator(); it.IsValid();)
  {
    const wdWorldModuleTypeId uiTypeId = it.Value();

    if (mappedIdsToRemove.Contains(uiTypeId))
    {
      it = m_TypeToId.Remove(it);
    }
    else
    {
      ++it;
    }
  }

  // Finally, adding all invalid typeIds to the free list for reusing later
  for (wdWorldModuleTypeId removedId : mappedIdsToRemove)
  {
    s_freeTypeIds.PushBack(removedId);
  }
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_WorldModule);
