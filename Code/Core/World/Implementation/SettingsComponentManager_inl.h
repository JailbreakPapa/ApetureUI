
template <typename ComponentType>
wdSettingsComponentManager<ComponentType>::wdSettingsComponentManager(wdWorld* pWorld)
  : wdComponentManagerBase(pWorld)
{
}

template <typename ComponentType>
wdSettingsComponentManager<ComponentType>::~wdSettingsComponentManager()
{
  for (auto& component : m_Components)
  {
    DeinitializeComponent(component.Borrow());
  }
}

template <typename ComponentType>
WD_ALWAYS_INLINE ComponentType* wdSettingsComponentManager<ComponentType>::GetSingletonComponent()
{
  for (const auto& pComponent : m_Components)
  {
    // retrieve the first component that is active
    if (pComponent->IsActive())
      return pComponent.Borrow();
  }

  return nullptr;
}

template <typename ComponentType>
WD_ALWAYS_INLINE const ComponentType* wdSettingsComponentManager<ComponentType>::GetSingletonComponent() const
{
  for (const auto& pComponent : m_Components)
  {
    // retrieve the first component that is active
    if (pComponent->IsActive())
      return pComponent.Borrow();
  }

  return nullptr;
}

// static
template <typename ComponentType>
WD_ALWAYS_INLINE wdWorldModuleTypeId wdSettingsComponentManager<ComponentType>::TypeId()
{
  return ComponentType::TypeId();
}

template <typename ComponentType>
void wdSettingsComponentManager<ComponentType>::CollectAllComponents(wdDynamicArray<wdComponentHandle>& out_allComponents, bool bOnlyActive)
{
  for (auto& component : m_Components)
  {
    if (!bOnlyActive || component->IsActive())
    {
      out_allComponents.PushBack(component->GetHandle());
    }
  }
}

template <typename ComponentType>
void wdSettingsComponentManager<ComponentType>::CollectAllComponents(wdDynamicArray<wdComponent*>& out_allComponents, bool bOnlyActive)
{
  for (auto& component : m_Components)
  {
    if (!bOnlyActive || component->IsActive())
    {
      out_allComponents.PushBack(component.Borrow());
    }
  }
}

template <typename ComponentType>
wdComponent* wdSettingsComponentManager<ComponentType>::CreateComponentStorage()
{
  if (!m_Components.IsEmpty())
  {
    wdLog::Warning("A component of type '{0}' is already present in this world. Having more than one is not allowed.", wdGetStaticRTTI<ComponentType>()->GetTypeName());
  }

  m_Components.PushBack(WD_NEW(GetAllocator(), ComponentType));
  return m_Components.PeekBack().Borrow();
}

template <typename ComponentType>
void wdSettingsComponentManager<ComponentType>::DeleteComponentStorage(wdComponent* pComponent, wdComponent*& out_pMovedComponent)
{
  out_pMovedComponent = pComponent;

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    if (m_Components[i].Borrow() == pComponent)
    {
      m_Components.RemoveAtAndCopy(i);
      break;
    }
  }
}
