
WD_FORCE_INLINE bool wdComponentManagerBase::IsValidComponent(const wdComponentHandle& hComponent) const
{
  return m_Components.Contains(hComponent);
}

WD_FORCE_INLINE bool wdComponentManagerBase::TryGetComponent(const wdComponentHandle& hComponent, wdComponent*& out_pComponent)
{
  return m_Components.TryGetValue(hComponent, out_pComponent);
}

WD_FORCE_INLINE bool wdComponentManagerBase::TryGetComponent(const wdComponentHandle& hComponent, const wdComponent*& out_pComponent) const
{
  wdComponent* pComponent = nullptr;
  bool res = m_Components.TryGetValue(hComponent, pComponent);
  out_pComponent = pComponent;
  return res;
}

WD_ALWAYS_INLINE wdUInt32 wdComponentManagerBase::GetComponentCount() const
{
  return static_cast<wdUInt32>(m_Components.GetCount());
}

template <typename ComponentType>
WD_ALWAYS_INLINE wdComponentHandle wdComponentManagerBase::CreateComponent(wdGameObject* pOwnerObject, ComponentType*& out_pComponent)
{
  wdComponent* pComponent = nullptr;
  wdComponentHandle hComponent = CreateComponentNoInit(pOwnerObject, pComponent);

  if (pComponent != nullptr)
  {
    InitializeComponent(pComponent);
  }

  out_pComponent = wdStaticCast<ComponentType*>(pComponent);
  return hComponent;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, wdBlockStorageType::Enum StorageType>
wdComponentManager<T, StorageType>::wdComponentManager(wdWorld* pWorld)
  : wdComponentManagerBase(pWorld)
  , m_ComponentStorage(GetBlockAllocator(), GetAllocator())
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponent, ComponentType), "Not a valid component type");
}

template <typename T, wdBlockStorageType::Enum StorageType>
wdComponentManager<T, StorageType>::~wdComponentManager()
{
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_FORCE_INLINE bool wdComponentManager<T, StorageType>::TryGetComponent(const wdComponentHandle& hComponent, ComponentType*& out_pComponent)
{
  WD_ASSERT_DEV(ComponentType::TypeId() == hComponent.GetInternalID().m_TypeId,
    "The given component handle is not of the expected type. Expected type id {0}, got type id {1}", ComponentType::TypeId(),
    hComponent.GetInternalID().m_TypeId);
  WD_ASSERT_DEV(hComponent.GetInternalID().m_WorldIndex == GetWorldIndex(),
    "Component does not belong to this world. Expected world id {0} got id {1}", GetWorldIndex(), hComponent.GetInternalID().m_WorldIndex);

  wdComponent* pComponent = nullptr;
  bool bResult = wdComponentManagerBase::TryGetComponent(hComponent, pComponent);
  out_pComponent = static_cast<ComponentType*>(pComponent);
  return bResult;
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_FORCE_INLINE bool wdComponentManager<T, StorageType>::TryGetComponent(
  const wdComponentHandle& hComponent, const ComponentType*& out_pComponent) const
{
  WD_ASSERT_DEV(ComponentType::TypeId() == hComponent.GetInternalID().m_TypeId,
    "The given component handle is not of the expected type. Expected type id {0}, got type id {1}", ComponentType::TypeId(),
    hComponent.GetInternalID().m_TypeId);
  WD_ASSERT_DEV(hComponent.GetInternalID().m_WorldIndex == GetWorldIndex(),
    "Component does not belong to this world. Expected world id {0} got id {1}", GetWorldIndex(), hComponent.GetInternalID().m_WorldIndex);

  const wdComponent* pComponent = nullptr;
  bool bResult = wdComponentManagerBase::TryGetComponent(hComponent, pComponent);
  out_pComponent = static_cast<const ComponentType*>(pComponent);
  return bResult;
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_ALWAYS_INLINE typename wdBlockStorage<T, wdInternal::DEFAULT_BLOCK_SIZE, StorageType>::Iterator wdComponentManager<T, StorageType>::GetComponents(wdUInt32 uiStartIndex /*= 0*/)
{
  return m_ComponentStorage.GetIterator(uiStartIndex);
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_ALWAYS_INLINE typename wdBlockStorage<T, wdInternal::DEFAULT_BLOCK_SIZE, StorageType>::ConstIterator
wdComponentManager<T, StorageType>::GetComponents(wdUInt32 uiStartIndex /*= 0*/) const
{
  return m_ComponentStorage.GetIterator(uiStartIndex);
}

// static
template <typename T, wdBlockStorageType::Enum StorageType>
WD_ALWAYS_INLINE wdWorldModuleTypeId wdComponentManager<T, StorageType>::TypeId()
{
  return T::TypeId();
}

template <typename T, wdBlockStorageType::Enum StorageType>
void wdComponentManager<T, StorageType>::CollectAllComponents(wdDynamicArray<wdComponentHandle>& out_allComponents, bool bOnlyActive)
{
  out_allComponents.Reserve(out_allComponents.GetCount() + m_ComponentStorage.GetCount());

  for (auto it = GetComponents(); it.IsValid(); it.Next())
  {
    if (!bOnlyActive || it->IsActive())
    {
      out_allComponents.PushBack(it->GetHandle());
    }
  }
}

template <typename T, wdBlockStorageType::Enum StorageType>
void wdComponentManager<T, StorageType>::CollectAllComponents(wdDynamicArray<wdComponent*>& out_allComponents, bool bOnlyActive)
{
  out_allComponents.Reserve(out_allComponents.GetCount() + m_ComponentStorage.GetCount());

  for (auto it = GetComponents(); it.IsValid(); it.Next())
  {
    if (!bOnlyActive || it->IsActive())
    {
      out_allComponents.PushBack(it);
    }
  }
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_ALWAYS_INLINE wdComponent* wdComponentManager<T, StorageType>::CreateComponentStorage()
{
  return m_ComponentStorage.Create();
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_FORCE_INLINE void wdComponentManager<T, StorageType>::DeleteComponentStorage(wdComponent* pComponent, wdComponent*& out_pMovedComponent)
{
  T* pMovedComponent = nullptr;
  m_ComponentStorage.Delete(static_cast<T*>(pComponent), pMovedComponent);
  out_pMovedComponent = pMovedComponent;
}

template <typename T, wdBlockStorageType::Enum StorageType>
WD_FORCE_INLINE void wdComponentManager<T, StorageType>::RegisterUpdateFunction(UpdateFunctionDesc& desc)
{
  // round up to multiple of data block capacity so tasks only have to deal with complete data blocks
  if (desc.m_uiGranularity != 0)
    desc.m_uiGranularity = static_cast<wdUInt16>(
      wdMath::RoundUp(static_cast<wdInt32>(desc.m_uiGranularity), wdDataBlock<ComponentType, wdInternal::DEFAULT_BLOCK_SIZE>::CAPACITY));

  wdComponentManagerBase::RegisterUpdateFunction(desc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename ComponentType, wdComponentUpdateType::Enum UpdateType, wdBlockStorageType::Enum StorageType>
wdComponentManagerSimple<ComponentType, UpdateType, StorageType>::wdComponentManagerSimple(wdWorld* pWorld)
  : wdComponentManager<ComponentType, StorageType>(pWorld)
{
}

template <typename ComponentType, wdComponentUpdateType::Enum UpdateType, wdBlockStorageType::Enum StorageType>
void wdComponentManagerSimple<ComponentType, UpdateType, StorageType>::Initialize()
{
  typedef wdComponentManagerSimple<ComponentType, UpdateType, StorageType> OwnType;

  wdStringBuilder functionName;
  SimpleUpdateName(functionName);

  auto desc = wdWorldModule::UpdateFunctionDesc(wdWorldModule::UpdateFunction(&OwnType::SimpleUpdate, this), functionName);
  desc.m_bOnlyUpdateWhenSimulating = (UpdateType == wdComponentUpdateType::WhenSimulating);

  this->RegisterUpdateFunction(desc);
}

template <typename ComponentType, wdComponentUpdateType::Enum UpdateType, wdBlockStorageType::Enum StorageType>
void wdComponentManagerSimple<ComponentType, UpdateType, StorageType>::SimpleUpdate(const wdWorldModule::UpdateContext& context)
{
  for (auto it = this->m_ComponentStorage.GetIterator(context.m_uiFirstComponentIndex, context.m_uiComponentCount); it.IsValid(); ++it)
  {
    ComponentType* pComponent = it;
    if (pComponent->IsActiveAndInitialized())
    {
      pComponent->Update();
    }
  }
}

// static
template <typename ComponentType, wdComponentUpdateType::Enum UpdateType, wdBlockStorageType::Enum StorageType>
void wdComponentManagerSimple<ComponentType, UpdateType, StorageType>::SimpleUpdateName(wdStringBuilder& out_sName)
{
  wdStringView sName(WD_SOURCE_FUNCTION);
  const char* szEnd = sName.FindSubString(",");

  if (szEnd != nullptr && sName.StartsWith("wdComponentManagerSimple<class "))
  {
    wdStringView sChoppedName(sName.GetStartPointer() + wdStringUtils::GetStringElementCount("wdComponentManagerSimple<class "), szEnd);

    WD_ASSERT_DEV(!sChoppedName.IsEmpty(), "Chopped name is empty: '{0}'", sName);

    out_sName = sChoppedName;
    out_sName.Append("::SimpleUpdate");
  }
  else
  {
    out_sName = sName;
  }
}
