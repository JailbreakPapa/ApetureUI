
WD_ALWAYS_INLINE wdStringView wdWorld::GetName() const
{
  return m_Data.m_sName;
}

WD_ALWAYS_INLINE wdUInt32 wdWorld::GetIndex() const
{
  return m_uiIndex;
}

WD_FORCE_INLINE wdGameObjectHandle wdWorld::CreateObject(const wdGameObjectDesc& desc)
{
  wdGameObject* pNewObject;
  return CreateObject(desc, pNewObject);
}

WD_ALWAYS_INLINE const wdEvent<const wdGameObject*>& wdWorld::GetObjectDeletionEvent() const
{
  return m_Data.m_ObjectDeletionEvent;
}

WD_FORCE_INLINE bool wdWorld::IsValidObject(const wdGameObjectHandle& hObject) const
{
  CheckForReadAccess();
  WD_ASSERT_DEV(hObject.IsInvalidated() || hObject.m_InternalId.m_WorldIndex == m_uiIndex,
    "Object does not belong to this world. Expected world id {0} got id {1}", m_uiIndex, hObject.m_InternalId.m_WorldIndex);

  return m_Data.m_Objects.Contains(hObject);
}

WD_FORCE_INLINE bool wdWorld::TryGetObject(const wdGameObjectHandle& hObject, wdGameObject*& out_pObject)
{
  CheckForReadAccess();
  WD_ASSERT_DEV(hObject.IsInvalidated() || hObject.m_InternalId.m_WorldIndex == m_uiIndex,
    "Object does not belong to this world. Expected world id {0} got id {1}", m_uiIndex, hObject.m_InternalId.m_WorldIndex);

  return m_Data.m_Objects.TryGetValue(hObject, out_pObject);
}

WD_FORCE_INLINE bool wdWorld::TryGetObject(const wdGameObjectHandle& hObject, const wdGameObject*& out_pObject) const
{
  CheckForReadAccess();
  WD_ASSERT_DEV(hObject.IsInvalidated() || hObject.m_InternalId.m_WorldIndex == m_uiIndex,
    "Object does not belong to this world. Expected world id {0} got id {1}", m_uiIndex, hObject.m_InternalId.m_WorldIndex);

  wdGameObject* pObject = nullptr;
  bool bResult = m_Data.m_Objects.TryGetValue(hObject, pObject);
  out_pObject = pObject;
  return bResult;
}

WD_FORCE_INLINE bool wdWorld::TryGetObjectWithGlobalKey(const wdTempHashedString& sGlobalKey, wdGameObject*& out_pObject)
{
  CheckForReadAccess();
  wdGameObjectId id;
  if (m_Data.m_GlobalKeyToIdTable.TryGetValue(sGlobalKey.GetHash(), id))
  {
    out_pObject = m_Data.m_Objects[id];
    return true;
  }

  return false;
}

WD_FORCE_INLINE bool wdWorld::TryGetObjectWithGlobalKey(const wdTempHashedString& sGlobalKey, const wdGameObject*& out_pObject) const
{
  CheckForReadAccess();
  wdGameObjectId id;
  if (m_Data.m_GlobalKeyToIdTable.TryGetValue(sGlobalKey.GetHash(), id))
  {
    out_pObject = m_Data.m_Objects[id];
    return true;
  }

  return false;
}

WD_FORCE_INLINE wdUInt32 wdWorld::GetObjectCount() const
{
  CheckForReadAccess();
  // Subtract one to exclude dummy object with instance index 0
  return static_cast<wdUInt32>(m_Data.m_Objects.GetCount() - 1);
}

WD_FORCE_INLINE wdInternal::WorldData::ObjectIterator wdWorld::GetObjects()
{
  CheckForWriteAccess();
  return wdInternal::WorldData::ObjectIterator(m_Data.m_ObjectStorage.GetIterator(0));
}

WD_FORCE_INLINE wdInternal::WorldData::ConstObjectIterator wdWorld::GetObjects() const
{
  CheckForReadAccess();
  return wdInternal::WorldData::ConstObjectIterator(m_Data.m_ObjectStorage.GetIterator(0));
}

WD_FORCE_INLINE void wdWorld::Traverse(VisitorFunc visitorFunc, TraversalMethod method /*= DepthFirst*/)
{
  CheckForWriteAccess();

  if (method == DepthFirst)
  {
    m_Data.TraverseDepthFirst(visitorFunc);
  }
  else // method == BreadthFirst
  {
    m_Data.TraverseBreadthFirst(visitorFunc);
  }
}

template <typename ModuleType>
WD_ALWAYS_INLINE ModuleType* wdWorld::GetOrCreateModule()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdWorldModule, ModuleType), "Not a valid module type");

  return wdStaticCast<ModuleType*>(GetOrCreateModule(wdGetStaticRTTI<ModuleType>()));
}

template <typename ModuleType>
WD_ALWAYS_INLINE void wdWorld::DeleteModule()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdWorldModule, ModuleType), "Not a valid module type");

  DeleteModule(wdGetStaticRTTI<ModuleType>());
}

template <typename ModuleType>
WD_ALWAYS_INLINE ModuleType* wdWorld::GetModule()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdWorldModule, ModuleType), "Not a valid module type");

  return wdStaticCast<ModuleType*>(GetModule(wdGetStaticRTTI<ModuleType>()));
}

template <typename ModuleType>
WD_ALWAYS_INLINE const ModuleType* wdWorld::GetModule() const
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdWorldModule, ModuleType), "Not a valid module type");

  return wdStaticCast<const ModuleType*>(GetModule(wdGetStaticRTTI<ModuleType>()));
}

template <typename ModuleType>
WD_ALWAYS_INLINE const ModuleType* wdWorld::GetModuleReadOnly() const
{
  return GetModule<ModuleType>();
}

template <typename ManagerType>
ManagerType* wdWorld::GetOrCreateComponentManager()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponentManagerBase, ManagerType), "Not a valid component manager type");

  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = ManagerType::TypeId();
  m_Data.m_Modules.EnsureCount(uiTypeId + 1);

  ManagerType* pModule = static_cast<ManagerType*>(m_Data.m_Modules[uiTypeId]);
  if (pModule == nullptr)
  {
    pModule = WD_NEW(&m_Data.m_Allocator, ManagerType, this);
    static_cast<wdWorldModule*>(pModule)->Initialize();

    m_Data.m_Modules[uiTypeId] = pModule;
    m_Data.m_ModulesToStartSimulation.PushBack(pModule);
  }

  return pModule;
}

WD_ALWAYS_INLINE wdComponentManagerBase* wdWorld::GetOrCreateManagerForComponentType(const wdRTTI* pComponentRtti)
{
  WD_ASSERT_DEV(pComponentRtti->IsDerivedFrom<wdComponent>(), "Invalid component type '%s'", pComponentRtti->GetTypeName());

  return wdStaticCast<wdComponentManagerBase*>(GetOrCreateModule(pComponentRtti));
}

template <typename ManagerType>
void wdWorld::DeleteComponentManager()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponentManagerBase, ManagerType), "Not a valid component manager type");

  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = ManagerType::TypeId();
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    if (ManagerType* pModule = static_cast<ManagerType*>(m_Data.m_Modules[uiTypeId]))
    {
      m_Data.m_Modules[uiTypeId] = nullptr;

      static_cast<wdWorldModule*>(pModule)->Deinitialize();
      DeregisterUpdateFunctions(pModule);
      WD_DELETE(&m_Data.m_Allocator, pModule);
    }
  }
}

template <typename ManagerType>
WD_FORCE_INLINE ManagerType* wdWorld::GetComponentManager()
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponentManagerBase, ManagerType), "Not a valid component manager type");

  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = ManagerType::TypeId();
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    return wdStaticCast<ManagerType*>(m_Data.m_Modules[uiTypeId]);
  }

  return nullptr;
}

template <typename ManagerType>
WD_FORCE_INLINE const ManagerType* wdWorld::GetComponentManager() const
{
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponentManagerBase, ManagerType), "Not a valid component manager type");

  CheckForReadAccess();

  const wdWorldModuleTypeId uiTypeId = ManagerType::TypeId();
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    return wdStaticCast<const ManagerType*>(m_Data.m_Modules[uiTypeId]);
  }

  return nullptr;
}

WD_ALWAYS_INLINE wdComponentManagerBase* wdWorld::GetManagerForComponentType(const wdRTTI* pComponentRtti)
{
  WD_ASSERT_DEV(pComponentRtti->IsDerivedFrom<wdComponent>(), "Invalid component type '{0}'", pComponentRtti->GetTypeName());

  return wdStaticCast<wdComponentManagerBase*>(GetModule(pComponentRtti));
}

WD_ALWAYS_INLINE const wdComponentManagerBase* wdWorld::GetManagerForComponentType(const wdRTTI* pComponentRtti) const
{
  WD_ASSERT_DEV(pComponentRtti->IsDerivedFrom<wdComponent>(), "Invalid component type '{0}'", pComponentRtti->GetTypeName());

  return wdStaticCast<const wdComponentManagerBase*>(GetModule(pComponentRtti));
}

inline bool wdWorld::IsValidComponent(const wdComponentHandle& hComponent) const
{
  CheckForReadAccess();
  const wdWorldModuleTypeId uiTypeId = hComponent.m_InternalId.m_TypeId;

  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    if (const wdWorldModule* pModule = m_Data.m_Modules[uiTypeId])
    {
      return static_cast<const wdComponentManagerBase*>(pModule)->IsValidComponent(hComponent);
    }
  }

  return false;
}

template <typename ComponentType>
inline bool wdWorld::TryGetComponent(const wdComponentHandle& hComponent, ComponentType*& out_pComponent)
{
  CheckForWriteAccess();
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponent, ComponentType), "Not a valid component type");

  const wdWorldModuleTypeId uiTypeId = hComponent.m_InternalId.m_TypeId;

  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    if (wdWorldModule* pModule = m_Data.m_Modules[uiTypeId])
    {
      wdComponent* pComponent = nullptr;
      bool bResult = static_cast<wdComponentManagerBase*>(pModule)->TryGetComponent(hComponent, pComponent);
      out_pComponent = wdDynamicCast<ComponentType*>(pComponent);
      return bResult && out_pComponent != nullptr;
    }
  }

  return false;
}

template <typename ComponentType>
inline bool wdWorld::TryGetComponent(const wdComponentHandle& hComponent, const ComponentType*& out_pComponent) const
{
  CheckForReadAccess();
  WD_CHECK_AT_COMPILETIME_MSG(WD_IS_DERIVED_FROM_STATIC(wdComponent, ComponentType), "Not a valid component type");

  const wdWorldModuleTypeId uiTypeId = hComponent.m_InternalId.m_TypeId;

  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    if (const wdWorldModule* pModule = m_Data.m_Modules[uiTypeId])
    {
      const wdComponent* pComponent = nullptr;
      bool bResult = static_cast<const wdComponentManagerBase*>(pModule)->TryGetComponent(hComponent, pComponent);
      out_pComponent = wdDynamicCast<const ComponentType*>(pComponent);
      return bResult && out_pComponent != nullptr;
    }
  }

  return false;
}

WD_FORCE_INLINE void wdWorld::SendMessage(const wdGameObjectHandle& hReceiverObject, wdMessage& ref_msg)
{
  CheckForWriteAccess();

  wdGameObject* pReceiverObject = nullptr;
  if (TryGetObject(hReceiverObject, pReceiverObject))
  {
    pReceiverObject->SendMessage(ref_msg);
  }
  else
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (ref_msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdWorld::SendMessage: The receiver wdGameObject for message of type '{0}' does not exist.", ref_msg.GetId());
    }
#endif
  }
}

WD_FORCE_INLINE void wdWorld::SendMessageRecursive(const wdGameObjectHandle& hReceiverObject, wdMessage& ref_msg)
{
  CheckForWriteAccess();

  wdGameObject* pReceiverObject = nullptr;
  if (TryGetObject(hReceiverObject, pReceiverObject))
  {
    pReceiverObject->SendMessageRecursive(ref_msg);
  }
  else
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (ref_msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdWorld::SendMessageRecursive: The receiver wdGameObject for message of type '{0}' does not exist.", ref_msg.GetId());
    }
#endif
  }
}

WD_ALWAYS_INLINE void wdWorld::PostMessage(
  const wdGameObjectHandle& hReceiverObject, const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  // This method is allowed to be called from multiple threads.
  PostMessage(hReceiverObject, msg, queueType, delay, false);
}

WD_ALWAYS_INLINE void wdWorld::PostMessageRecursive(
  const wdGameObjectHandle& hReceiverObject, const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  // This method is allowed to be called from multiple threads.
  PostMessage(hReceiverObject, msg, queueType, delay, true);
}

WD_FORCE_INLINE void wdWorld::SendMessage(const wdComponentHandle& hReceiverComponent, wdMessage& ref_msg)
{
  CheckForWriteAccess();

  wdComponent* pReceiverComponent = nullptr;
  if (TryGetComponent(hReceiverComponent, pReceiverComponent))
  {
    pReceiverComponent->SendMessage(ref_msg);
  }
  else
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (ref_msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdWorld::SendMessage: The receiver wdComponent for message of type '{0}' does not exist.", ref_msg.GetId());
    }
#endif
  }
}

WD_ALWAYS_INLINE void wdWorld::SetWorldSimulationEnabled(bool bEnable)
{
  m_Data.m_bSimulateWorld = bEnable;
}

WD_ALWAYS_INLINE bool wdWorld::GetWorldSimulationEnabled() const
{
  return m_Data.m_bSimulateWorld;
}

WD_ALWAYS_INLINE const wdSharedPtr<wdTask>& wdWorld::GetUpdateTask()
{
  return m_pUpdateTask;
}

WD_FORCE_INLINE wdSpatialSystem* wdWorld::GetSpatialSystem()
{
  CheckForWriteAccess();

  return m_Data.m_pSpatialSystem.Borrow();
}

WD_FORCE_INLINE const wdSpatialSystem* wdWorld::GetSpatialSystem() const
{
  CheckForReadAccess();

  return m_Data.m_pSpatialSystem.Borrow();
}

WD_ALWAYS_INLINE void wdWorld::GetCoordinateSystem(const wdVec3& vGlobalPosition, wdCoordinateSystem& out_coordinateSystem) const
{
  m_Data.m_pCoordinateSystemProvider->GetCoordinateSystem(vGlobalPosition, out_coordinateSystem);
}

WD_ALWAYS_INLINE wdCoordinateSystemProvider& wdWorld::GetCoordinateSystemProvider()
{
  return *(m_Data.m_pCoordinateSystemProvider.Borrow());
}

WD_ALWAYS_INLINE const wdCoordinateSystemProvider& wdWorld::GetCoordinateSystemProvider() const
{
  return *(m_Data.m_pCoordinateSystemProvider.Borrow());
}

WD_ALWAYS_INLINE wdClock& wdWorld::GetClock()
{
  return m_Data.m_Clock;
}

WD_ALWAYS_INLINE const wdClock& wdWorld::GetClock() const
{
  return m_Data.m_Clock;
}

WD_ALWAYS_INLINE wdRandom& wdWorld::GetRandomNumberGenerator()
{
  return m_Data.m_Random;
}

WD_ALWAYS_INLINE wdAllocatorBase* wdWorld::GetAllocator()
{
  return &m_Data.m_Allocator;
}

WD_ALWAYS_INLINE wdInternal::WorldLargeBlockAllocator* wdWorld::GetBlockAllocator()
{
  return &m_Data.m_BlockAllocator;
}

WD_ALWAYS_INLINE wdDoubleBufferedStackAllocator* wdWorld::GetStackAllocator()
{
  return &m_Data.m_StackAllocator;
}

WD_ALWAYS_INLINE wdInternal::WorldData::ReadMarker& wdWorld::GetReadMarker() const
{
  return m_Data.m_ReadMarker;
}

WD_ALWAYS_INLINE wdInternal::WorldData::WriteMarker& wdWorld::GetWriteMarker()
{
  return m_Data.m_WriteMarker;
}

WD_FORCE_INLINE void wdWorld::SetUserData(void* pUserData)
{
  CheckForWriteAccess();

  m_Data.m_pUserData = pUserData;
}

WD_FORCE_INLINE void* wdWorld::GetUserData() const
{
  CheckForReadAccess();

  return m_Data.m_pUserData;
}

constexpr wdUInt64 wdWorld::GetMaxNumGameObjects()
{
  return wdGameObjectId::MAX_INSTANCES - 2;
}

constexpr wdUInt64 wdWorld::GetMaxNumHierarchyLevels()
{
  return 1 << (sizeof(wdGameObject::m_uiHierarchyLevel) * 8);
}

constexpr wdUInt64 wdWorld::GetMaxNumComponentsPerType()
{
  return wdComponentId::MAX_INSTANCES - 1;
}

constexpr wdUInt64 wdWorld::GetMaxNumWorldModules()
{
  return WD_MAX_WORLD_MODULE_TYPES;
}

constexpr wdUInt64 wdWorld::GetMaxNumComponentTypes()
{
  return WD_MAX_COMPONENT_TYPES;
}

constexpr wdUInt64 wdWorld::GetMaxNumWorlds()
{
  return WD_MAX_WORLDS;
}

// static
WD_ALWAYS_INLINE wdUInt32 wdWorld::GetWorldCount()
{
  return s_Worlds.GetCount();
}

// static
WD_ALWAYS_INLINE wdWorld* wdWorld::GetWorld(wdUInt32 uiIndex)
{
  return s_Worlds[uiIndex];
}

// static
WD_ALWAYS_INLINE wdWorld* wdWorld::GetWorld(const wdGameObjectHandle& hObject)
{
  return s_Worlds[hObject.GetInternalID().m_WorldIndex];
}

// static
WD_ALWAYS_INLINE wdWorld* wdWorld::GetWorld(const wdComponentHandle& hComponent)
{
  return s_Worlds[hComponent.GetInternalID().m_WorldIndex];
}

WD_ALWAYS_INLINE void wdWorld::CheckForReadAccess() const
{
  WD_ASSERT_DEV(m_Data.m_iReadCounter > 0, "Trying to read from World '{0}', but it is not marked for reading.", GetName());
}

WD_ALWAYS_INLINE void wdWorld::CheckForWriteAccess() const
{
  WD_ASSERT_DEV(
    m_Data.m_WriteThreadID == wdThreadUtils::GetCurrentThreadID(), "Trying to write to World '{0}', but it is not marked for writing.", GetName());
}

WD_ALWAYS_INLINE wdGameObject* wdWorld::GetObjectUnchecked(wdUInt32 uiIndex) const
{
  return m_Data.m_Objects.GetValueUnchecked(uiIndex);
}

WD_ALWAYS_INLINE bool wdWorld::ReportErrorWhenStaticObjectMoves() const
{
  return m_Data.m_bReportErrorWhenStaticObjectMoves;
}
