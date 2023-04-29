#include <Core/CorePCH.h>

#include <Core/World/World.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdComponentManagerBase, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdComponentManagerBase::wdComponentManagerBase(wdWorld* pWorld)
  : wdWorldModule(pWorld)
  , m_Components(pWorld->GetAllocator())
{
}

wdComponentManagerBase::~wdComponentManagerBase() {}

wdComponentHandle wdComponentManagerBase::CreateComponent(wdGameObject* pOwnerObject)
{
  wdComponent* pDummy;
  return CreateComponent(pOwnerObject, pDummy);
}

void wdComponentManagerBase::DeleteComponent(const wdComponentHandle& hComponent)
{
  wdComponent* pComponent = nullptr;
  if (!m_Components.TryGetValue(hComponent, pComponent))
    return;

  DeleteComponent(pComponent);
}

void wdComponentManagerBase::DeleteComponent(wdComponent* pComponent)
{
  if (pComponent == nullptr)
    return;

  DeinitializeComponent(pComponent);

  m_Components.Remove(pComponent->m_InternalId);

  pComponent->m_InternalId.Invalidate();
  pComponent->m_ComponentFlags.Remove(wdObjectFlags::ActiveFlag | wdObjectFlags::ActiveState);

  GetWorld()->m_Data.m_DeadComponents.Insert(pComponent);
}

void wdComponentManagerBase::Deinitialize()
{
  for (auto it = m_Components.GetIterator(); it.IsValid(); ++it)
  {
    DeinitializeComponent(it.Value());
  }

  SUPER::Deinitialize();
}

wdComponentHandle wdComponentManagerBase::CreateComponentNoInit(wdGameObject* pOwnerObject, wdComponent*& out_pComponent)
{
  WD_ASSERT_DEV(m_Components.GetCount() < wdWorld::GetMaxNumComponentsPerType(), "Max number of components per type reached: {}",
    wdWorld::GetMaxNumComponentsPerType());

  wdComponent* pComponent = CreateComponentStorage();
  if (pComponent == nullptr)
  {
    return wdComponentHandle();
  }

  wdComponentId newId = m_Components.Insert(pComponent);
  newId.m_WorldIndex = GetWorldIndex();
  newId.m_TypeId = pComponent->GetTypeId();

  pComponent->m_pManager = this;
  pComponent->m_InternalId = newId;
  pComponent->m_ComponentFlags.AddOrRemove(wdObjectFlags::Dynamic, pComponent->GetMode() == wdComponentMode::Dynamic);

  // In Editor we add components via reflection so it is fine to have a nullptr here.
  // We check for a valid owner before the Initialize() callback.
  if (pOwnerObject != nullptr)
  {
    // AddComponent will update the active state internally
    pOwnerObject->AddComponent(pComponent);
  }
  else
  {
    pComponent->UpdateActiveState(true);
  }

  out_pComponent = pComponent;
  return pComponent->GetHandle();
}

void wdComponentManagerBase::InitializeComponent(wdComponent* pComponent)
{
  GetWorld()->AddComponentToInitialize(pComponent->GetHandle());
}

void wdComponentManagerBase::DeinitializeComponent(wdComponent* pComponent)
{
  if (pComponent->IsInitialized())
  {
    pComponent->Deinitialize();
    pComponent->m_ComponentFlags.Remove(wdObjectFlags::Initialized);
  }

  if (wdGameObject* pOwner = pComponent->GetOwner())
  {
    pOwner->RemoveComponent(pComponent);
  }
}

void wdComponentManagerBase::PatchIdTable(wdComponent* pComponent)
{
  wdComponentId id = pComponent->m_InternalId;
  if (id.m_InstanceIndex != wdComponentId::INVALID_INSTANCE_INDEX)
    m_Components[id] = pComponent;
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_ComponentManager);
