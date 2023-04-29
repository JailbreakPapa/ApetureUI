#include <Core/CorePCH.h>

#include <Core/Messages/DeleteObjectMessage.h>
#include <Core/Messages/HierarchyChangedMessages.h>
#include <Core/World/EventMessageHandlerComponent.h>
#include <Core/World/World.h>
#include <Core/World/WorldModule.h>
#include <Foundation/Memory/FrameAllocator.h>
#include <Foundation/Profiling/Profiling.h>
#include <Foundation/Utilities/Stats.h>

wdStaticArray<wdWorld*, wdWorld::GetMaxNumWorlds()> wdWorld::s_Worlds;

static wdGameObjectHandle DefaultGameObjectReferenceResolver(const void* pData, wdComponentHandle hThis, const char* szProperty)
{
  const char* szRef = reinterpret_cast<const char*>(pData);

  if (wdStringUtils::IsNullOrEmpty(szRef))
    return wdGameObjectHandle();

  // this is a convention used by wdPrefabReferenceComponent:
  // a string starting with this means a 'global game object reference', ie a reference that is valid within the current world
  // what follows is an integer that is the internal storage of an wdGameObjectHandle
  // thus parsing the int and casting it to an wdGameObjectHandle gives the desired result
  if (wdStringUtils::StartsWith(szRef, "#!GGOR-"))
  {
    wdInt64 id;
    if (wdConversionUtils::StringToInt64(szRef + 7, id).Succeeded())
    {
      return wdGameObjectHandle(wdGameObjectId(reinterpret_cast<wdUInt64&>(id)));
    }
  }

  return wdGameObjectHandle();
}

wdWorld::wdWorld(wdWorldDesc& ref_desc)
  : m_Data(ref_desc)
{
  m_pUpdateTask = WD_DEFAULT_NEW(wdDelegateTask<void>, "", wdMakeDelegate(&wdWorld::UpdateFromThread, this));
  m_Data.m_pCoordinateSystemProvider->m_pOwnerWorld = this;

  wdStringBuilder sb = ref_desc.m_sName.GetString();
  sb.Append(".Update");
  m_pUpdateTask->ConfigureTask(sb, wdTaskNesting::Maybe);

  m_uiIndex = wdInvalidIndex;

  // find a free world slot
  const wdUInt32 uiWorldCount = s_Worlds.GetCount();
  for (wdUInt32 i = 0; i < uiWorldCount; i++)
  {
    if (s_Worlds[i] == nullptr)
    {
      s_Worlds[i] = this;
      m_uiIndex = i;
      break;
    }
  }

  if (m_uiIndex == wdInvalidIndex)
  {
    m_uiIndex = s_Worlds.GetCount();
    WD_ASSERT_DEV(m_uiIndex < GetMaxNumWorlds(), "Max world index reached: {}", GetMaxNumWorlds());
    static_assert(GetMaxNumWorlds() == WD_MAX_WORLDS);

    s_Worlds.PushBack(this);
  }

  SetGameObjectReferenceResolver(DefaultGameObjectReferenceResolver);
}

wdWorld::~wdWorld()
{
  SetWorldSimulationEnabled(false);

  WD_LOCK(GetWriteMarker());
  m_Data.Clear();

  s_Worlds[m_uiIndex] = nullptr;
  m_uiIndex = wdInvalidIndex;
}


void wdWorld::Clear()
{
  CheckForWriteAccess();

  while (GetObjectCount() > 0)
  {
    for (auto it = GetObjects(); it.IsValid(); ++it)
    {
      DeleteObjectNow(it->GetHandle());
    }

    if (GetObjectCount() > 0)
    {
      wdLog::Dev("Remaining objects after wdWorld::Clear: {}", GetObjectCount());
    }
  }

  for (wdWorldModule* pModule : m_Data.m_Modules)
  {
    if (pModule != nullptr)
    {
      pModule->WorldClear();
    }
  }

  // make sure all dead objects and components are cleared right now
  DeleteDeadObjects();
  DeleteDeadComponents();

  wdEventMessageHandlerComponent::ClearGlobalEventHandlersForWorld(this);
}

void wdWorld::SetCoordinateSystemProvider(const wdSharedPtr<wdCoordinateSystemProvider>& pProvider)
{
  WD_ASSERT_DEV(pProvider != nullptr, "Coordinate System Provider must not be null");

  m_Data.m_pCoordinateSystemProvider = pProvider;
  m_Data.m_pCoordinateSystemProvider->m_pOwnerWorld = this;
}

void wdWorld::SetGameObjectReferenceResolver(const ReferenceResolver& resolver)
{
  m_Data.m_GameObjectReferenceResolver = resolver;
}

const wdWorld::ReferenceResolver& wdWorld::GetGameObjectReferenceResolver() const
{
  return m_Data.m_GameObjectReferenceResolver;
}

// a super simple, but also efficient random number generator
inline static wdUInt32 NextStableRandomSeed(wdUInt32& ref_uiSeed)
{
  ref_uiSeed = 214013L * ref_uiSeed + 2531011L;
  return ((ref_uiSeed >> 16) & 0x7FFFF);
}

wdGameObjectHandle wdWorld::CreateObject(const wdGameObjectDesc& desc, wdGameObject*& out_pObject)
{
  CheckForWriteAccess();

  WD_ASSERT_DEV(m_Data.m_Objects.GetCount() < GetMaxNumGameObjects(), "Max number of game objects reached: {}", GetMaxNumGameObjects());

  wdGameObject* pParentObject = nullptr;
  wdGameObject::TransformationData* pParentData = nullptr;
  wdUInt32 uiParentIndex = 0;
  wdUInt64 uiHierarchyLevel = 0;
  bool bDynamic = desc.m_bDynamic;

  if (TryGetObject(desc.m_hParent, pParentObject))
  {
    pParentData = pParentObject->m_pTransformationData;
    uiParentIndex = desc.m_hParent.m_InternalId.m_InstanceIndex;
    uiHierarchyLevel = pParentObject->m_uiHierarchyLevel + 1; // if there is a parent hierarchy level is parent level + 1
    WD_ASSERT_DEV(uiHierarchyLevel < GetMaxNumHierarchyLevels(), "Max hierarchy level reached: {}", GetMaxNumHierarchyLevels());
    bDynamic |= pParentObject->IsDynamic();
  }

  // get storage for the transformation data
  wdGameObject::TransformationData* pTransformationData = m_Data.CreateTransformationData(bDynamic, static_cast<wdUInt32>(uiHierarchyLevel));

  // get storage for the object itself
  wdGameObject* pNewObject = m_Data.m_ObjectStorage.Create();

  // insert the new object into the id mapping table
  wdGameObjectId newId = m_Data.m_Objects.Insert(pNewObject);
  newId.m_WorldIndex = wdGameObjectId::StorageType(m_uiIndex & (WD_MAX_WORLDS - 1));

  // fill out some data
  pNewObject->m_InternalId = newId;
  pNewObject->m_Flags = wdObjectFlags::None;
  pNewObject->m_Flags.AddOrRemove(wdObjectFlags::Dynamic, bDynamic);
  pNewObject->m_Flags.AddOrRemove(wdObjectFlags::ActiveFlag, desc.m_bActiveFlag);
  pNewObject->m_sName = desc.m_sName;
  pNewObject->m_uiParentIndex = uiParentIndex;
  pNewObject->m_Tags = desc.m_Tags;
  pNewObject->m_uiTeamID = desc.m_uiTeamID;

  static_assert((GetMaxNumHierarchyLevels() - 1) <= wdMath::MaxValue<wdUInt16>());
  pNewObject->m_uiHierarchyLevel = static_cast<wdUInt16>(uiHierarchyLevel);

  // fill out the transformation data
  pTransformationData->m_pObject = pNewObject;
  pTransformationData->m_pParentData = pParentData;
  pTransformationData->m_localPosition = wdSimdConversion::ToVec3(desc.m_LocalPosition);
  pTransformationData->m_localRotation = wdSimdConversion::ToQuat(desc.m_LocalRotation);
  pTransformationData->m_localScaling = wdSimdConversion::ToVec4(desc.m_LocalScaling.GetAsVec4(desc.m_LocalUniformScaling));
  pTransformationData->m_globalTransform.SetIdentity();
#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
  pTransformationData->m_velocity.SetZero();
#endif
  pTransformationData->m_localBounds.SetInvalid();
  pTransformationData->m_localBounds.m_BoxHalfExtents.SetW(wdSimdFloat::Zero());
  pTransformationData->m_globalBounds = pTransformationData->m_localBounds;
  pTransformationData->m_hSpatialData.Invalidate();
  pTransformationData->m_uiSpatialDataCategoryBitmask = 0;
  pTransformationData->m_uiStableRandomSeed = desc.m_uiStableRandomSeed;

  // if seed is set to 0xFFFFFFFF, use the parent's seed to create a deterministic value for this object
  if (pTransformationData->m_uiStableRandomSeed == 0xFFFFFFFF && pTransformationData->m_pParentData != nullptr)
  {
    wdUInt32 seed = pTransformationData->m_pParentData->m_uiStableRandomSeed + pTransformationData->m_pParentData->m_pObject->GetChildCount();

    do
    {
      pTransformationData->m_uiStableRandomSeed = NextStableRandomSeed(seed);

    } while (pTransformationData->m_uiStableRandomSeed == 0 || pTransformationData->m_uiStableRandomSeed == 0xFFFFFFFF);
  }

  // if the seed is zero (or there was no parent to derive the seed from), assign a random value
  while (pTransformationData->m_uiStableRandomSeed == 0 || pTransformationData->m_uiStableRandomSeed == 0xFFFFFFFF)
  {
    pTransformationData->m_uiStableRandomSeed = GetRandomNumberGenerator().UInt();
  }

  pTransformationData->UpdateGlobalTransformNonRecursive();

#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
  pTransformationData->m_lastGlobalPosition = pTransformationData->m_globalTransform.m_Position;
#endif

  // link the transformation data to the game object
  pNewObject->m_pTransformationData = pTransformationData;

  // fix links
  LinkToParent(pNewObject);

  pNewObject->UpdateActiveState(pParentObject == nullptr ? true : pParentObject->IsActive());

  out_pObject = pNewObject;
  return wdGameObjectHandle(newId);
}

void wdWorld::DeleteObjectNow(const wdGameObjectHandle& hObject0, bool bAlsoDeleteEmptyParents /*= true*/)
{
  CheckForWriteAccess();

  wdGameObject* pObject = nullptr;
  if (!m_Data.m_Objects.TryGetValue(hObject0, pObject))
    return;

  wdGameObjectHandle hObject = hObject0;

  if (bAlsoDeleteEmptyParents)
  {
    wdGameObject* pParent = pObject->GetParent();

    while (pParent)
    {
      if (pParent->GetChildCount() != 1 || pParent->GetComponents().GetCount() != 0)
        break;

      pObject = pParent;

      pParent = pParent->GetParent();
    }

    hObject = pObject->GetHandle();
  }

  // inform external systems that we are about to delete this object
  m_Data.m_ObjectDeletionEvent.Broadcast(pObject);

  // set object to inactive so components and children know that they shouldn't access the object anymore.
  pObject->m_Flags.Remove(wdObjectFlags::ActiveFlag | wdObjectFlags::ActiveState);

  // delete children
  for (auto it = pObject->GetChildren(); it.IsValid(); ++it)
  {
    DeleteObjectNow(it->GetHandle(), false);
  }

  // delete attached components
  while (!pObject->m_Components.IsEmpty())
  {
    wdComponent* pComponent = pObject->m_Components[0];
    pComponent->GetOwningManager()->DeleteComponent(pComponent->GetHandle());
  }
  WD_ASSERT_DEV(pObject->m_Components.GetCount() == 0, "Components should already be removed");

  // fix parent and siblings
  UnlinkFromParent(pObject);

  // remove from global key tables
  SetObjectGlobalKey(pObject, wdHashedString());

  // invalidate (but preserve world index) and remove from id table
  pObject->m_InternalId.Invalidate();
  pObject->m_InternalId.m_WorldIndex = m_uiIndex;

  m_Data.m_DeadObjects.Insert(pObject);
  WD_VERIFY(m_Data.m_Objects.Remove(hObject), "Implementation error.");
}

void wdWorld::DeleteObjectDelayed(const wdGameObjectHandle& hObject, bool bAlsoDeleteEmptyParents /*= true*/)
{
  wdMsgDeleteGameObject msg;
  msg.m_bDeleteEmptyParents = bAlsoDeleteEmptyParents;
  PostMessage(hObject, msg, wdTime::Zero());
}

wdComponentInitBatchHandle wdWorld::CreateComponentInitBatch(wdStringView sBatchName, bool bMustFinishWithinOneFrame /*= true*/)
{
  auto pInitBatch = WD_NEW(GetAllocator(), wdInternal::WorldData::InitBatch, GetAllocator(), sBatchName, bMustFinishWithinOneFrame);
  return wdComponentInitBatchHandle(m_Data.m_InitBatches.Insert(pInitBatch));
}

void wdWorld::DeleteComponentInitBatch(const wdComponentInitBatchHandle& hBatch)
{
  auto& pInitBatch = m_Data.m_InitBatches[hBatch.GetInternalID()];
  WD_ASSERT_DEV(pInitBatch->m_ComponentsToInitialize.IsEmpty() && pInitBatch->m_ComponentsToStartSimulation.IsEmpty(), "Init batch has not been completely processed");
  m_Data.m_InitBatches.Remove(hBatch.GetInternalID());
}

void wdWorld::BeginAddingComponentsToInitBatch(const wdComponentInitBatchHandle& hBatch)
{
  WD_ASSERT_DEV(m_Data.m_pCurrentInitBatch == m_Data.m_pDefaultInitBatch, "Nested init batches are not supported");
  m_Data.m_pCurrentInitBatch = m_Data.m_InitBatches[hBatch.GetInternalID()].Borrow();
}

void wdWorld::EndAddingComponentsToInitBatch(const wdComponentInitBatchHandle& hBatch)
{
  WD_ASSERT_DEV(m_Data.m_InitBatches[hBatch.GetInternalID()] == m_Data.m_pCurrentInitBatch, "Init batch with id {} is currently not active", hBatch.GetInternalID().m_Data);
  m_Data.m_pCurrentInitBatch = m_Data.m_pDefaultInitBatch;
}

void wdWorld::SubmitComponentInitBatch(const wdComponentInitBatchHandle& hBatch)
{
  m_Data.m_InitBatches[hBatch.GetInternalID()]->m_bIsReady = true;
  m_Data.m_pCurrentInitBatch = m_Data.m_pDefaultInitBatch;
}

bool wdWorld::IsComponentInitBatchCompleted(const wdComponentInitBatchHandle& hBatch, double* pCompletionFactor /*= nullptr*/)
{
  auto& pInitBatch = m_Data.m_InitBatches[hBatch.GetInternalID()];
  WD_ASSERT_DEV(pInitBatch->m_bIsReady, "Batch is not submitted yet");

  if (pCompletionFactor != nullptr)
  {
    if (pInitBatch->m_ComponentsToInitialize.IsEmpty())
    {
      double fStartSimCompletion = pInitBatch->m_ComponentsToStartSimulation.IsEmpty() ? 1.0 : (double)pInitBatch->m_uiNextComponentToStartSimulation / pInitBatch->m_ComponentsToStartSimulation.GetCount();
      *pCompletionFactor = fStartSimCompletion * 0.5 + 0.5;
    }
    else
    {
      double fInitCompletion = pInitBatch->m_ComponentsToInitialize.IsEmpty() ? 1.0 : (double)pInitBatch->m_uiNextComponentToInitialize / pInitBatch->m_ComponentsToInitialize.GetCount();
      *pCompletionFactor = fInitCompletion * 0.5;
    }
  }

  return pInitBatch->m_ComponentsToInitialize.IsEmpty() && pInitBatch->m_ComponentsToStartSimulation.IsEmpty();
}

void wdWorld::CancelComponentInitBatch(const wdComponentInitBatchHandle& hBatch)
{
  auto& pInitBatch = m_Data.m_InitBatches[hBatch.GetInternalID()];
  pInitBatch->m_ComponentsToInitialize.Clear();
  pInitBatch->m_ComponentsToStartSimulation.Clear();
}
void wdWorld::PostMessage(const wdGameObjectHandle& receiverObject, const wdMessage& msg, wdObjectMsgQueueType::Enum queueType, wdTime delay, bool bRecursive) const
{
  // This method is allowed to be called from multiple threads.

  WD_ASSERT_DEBUG((receiverObject.m_InternalId.m_Data >> 62) == 0, "Upper 2 bits in object id must not be set");

  QueuedMsgMetaData metaData;
  metaData.m_uiReceiverObjectOrComponent = receiverObject.m_InternalId.m_Data;
  metaData.m_uiReceiverIsComponent = false;
  metaData.m_uiRecursive = bRecursive;

  wdRTTIAllocator* pMsgRTTIAllocator = msg.GetDynamicRTTI()->GetAllocator();
  if (delay.GetSeconds() > 0.0)
  {
    wdMessage* pMsgCopy = pMsgRTTIAllocator->Clone<wdMessage>(&msg, &m_Data.m_Allocator);

    metaData.m_Due = m_Data.m_Clock.GetAccumulatedTime() + delay;
    m_Data.m_TimedMessageQueues[queueType].Enqueue(pMsgCopy, metaData);
  }
  else
  {
    wdMessage* pMsgCopy = pMsgRTTIAllocator->Clone<wdMessage>(&msg, m_Data.m_StackAllocator.GetCurrentAllocator());
    m_Data.m_MessageQueues[queueType].Enqueue(pMsgCopy, metaData);
  }
}

void wdWorld::PostMessage(const wdComponentHandle& hReceiverComponent, const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  // This method is allowed to be called from multiple threads.

  WD_ASSERT_DEBUG((hReceiverComponent.m_InternalId.m_Data >> 62) == 0, "Upper 2 bits in component id must not be set");

  QueuedMsgMetaData metaData;
  metaData.m_uiReceiverObjectOrComponent = hReceiverComponent.m_InternalId.m_Data;
  metaData.m_uiReceiverIsComponent = true;
  metaData.m_uiRecursive = false;

  wdRTTIAllocator* pMsgRTTIAllocator = msg.GetDynamicRTTI()->GetAllocator();
  if (delay.GetSeconds() > 0.0)
  {
    wdMessage* pMsgCopy = pMsgRTTIAllocator->Clone<wdMessage>(&msg, &m_Data.m_Allocator);

    metaData.m_Due = m_Data.m_Clock.GetAccumulatedTime() + delay;
    m_Data.m_TimedMessageQueues[queueType].Enqueue(pMsgCopy, metaData);
  }
  else
  {
    wdMessage* pMsgCopy = pMsgRTTIAllocator->Clone<wdMessage>(&msg, m_Data.m_StackAllocator.GetCurrentAllocator());
    m_Data.m_MessageQueues[queueType].Enqueue(pMsgCopy, metaData);
  }
}

void wdWorld::FindEventMsgHandlers(const wdMessage& msg, wdGameObject* pSearchObject, wdDynamicArray<wdComponent*>& out_components)
{
  FindEventMsgHandlers(*this, msg, pSearchObject, out_components);
}

void wdWorld::FindEventMsgHandlers(const wdMessage& msg, const wdGameObject* pSearchObject, wdDynamicArray<const wdComponent*>& out_components) const
{
  FindEventMsgHandlers(*this, msg, pSearchObject, out_components);
}

void wdWorld::Update()
{
  CheckForWriteAccess();

  WD_LOG_BLOCK(m_Data.m_sName.GetData());

  {
    wdStringBuilder sStatName;
    sStatName.Format("World Update/{0}/Game Object Count", m_Data.m_sName);

    wdStringBuilder sStatValue;
    wdStats::SetStat(sStatName, GetObjectCount());
  }

  if (!m_Data.m_bSimulateWorld)
  {
    // only change the pause mode temporarily
    // so that user choices don't get overridden

    const bool bClockPaused = m_Data.m_Clock.GetPaused();
    m_Data.m_Clock.SetPaused(true);
    m_Data.m_Clock.Update();
    m_Data.m_Clock.SetPaused(bClockPaused);
  }
  else
  {
    m_Data.m_Clock.Update();
  }

  if (m_Data.m_pSpatialSystem != nullptr)
  {
    m_Data.m_pSpatialSystem->StartNewFrame();
  }

  // initialize phase
  {
    WD_PROFILE_SCOPE("Initialize Phase");
    ProcessComponentsToInitialize();
    ProcessUpdateFunctionsToRegister();

    ProcessQueuedMessages(wdObjectMsgQueueType::AfterInitialized);
  }

  // pre-async phase
  {
    WD_PROFILE_SCOPE("Pre-Async Phase");
    ProcessQueuedMessages(wdObjectMsgQueueType::NextFrame);
    UpdateSynchronous(m_Data.m_UpdateFunctions[wdComponentManagerBase::UpdateFunctionDesc::Phase::PreAsync]);
  }

  // async phase
  {
    // remove write marker but keep the read marker. Thus no one can mark the world for writing now. Only reading is allowed in async phase.
    m_Data.m_WriteThreadID = (wdThreadID)0;

    WD_PROFILE_SCOPE("Async Phase");
    UpdateAsynchronous();

    // restore write marker
    m_Data.m_WriteThreadID = wdThreadUtils::GetCurrentThreadID();
  }

  // post-async phase
  {
    WD_PROFILE_SCOPE("Post-Async Phase");
    ProcessQueuedMessages(wdObjectMsgQueueType::PostAsync);
    UpdateSynchronous(m_Data.m_UpdateFunctions[wdComponentManagerBase::UpdateFunctionDesc::Phase::PostAsync]);
  }

  // delete dead objects and update the object hierarchy
  {
    WD_PROFILE_SCOPE("Delete Dead Objects");
    DeleteDeadObjects();
    DeleteDeadComponents();
  }

  // update transforms
  {
    float fInvDelta = 0.0f;

    // when the clock is paused just use zero
    const float fDelta = (float)m_Data.m_Clock.GetTimeDiff().GetSeconds();
    if (fDelta > 0.0f)
      fInvDelta = 1.0f / fDelta;

    WD_PROFILE_SCOPE("Update Transforms");
    m_Data.UpdateGlobalTransforms(fInvDelta);
  }

  // post-transform phase
  {
    WD_PROFILE_SCOPE("Post-Transform Phase");
    ProcessQueuedMessages(wdObjectMsgQueueType::PostTransform);
    UpdateSynchronous(m_Data.m_UpdateFunctions[wdComponentManagerBase::UpdateFunctionDesc::Phase::PostTransform]);
  }

  // Process again so new component can receive render messages, otherwise we introduce a frame delay.
  {
    WD_PROFILE_SCOPE("Initialize Phase 2");
    // Only process the default init batch here since it contains the components created at runtime.
    // Also make sure that all initialization is finished after this call by giving it enough time.
    ProcessInitializationBatch(*m_Data.m_pDefaultInitBatch, wdTime::Now() + wdTime::Hours(10000));

    ProcessQueuedMessages(wdObjectMsgQueueType::AfterInitialized);
  }

  // Swap our double buffered stack allocator
  m_Data.m_StackAllocator.Swap();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

wdWorldModule* wdWorld::GetOrCreateModule(const wdRTTI* pRtti)
{
  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = wdWorldModuleFactory::GetInstance()->GetTypeId(pRtti);
  if (uiTypeId == 0xFFFF)
  {
    return nullptr;
  }

  m_Data.m_Modules.EnsureCount(uiTypeId + 1);

  wdWorldModule* pModule = m_Data.m_Modules[uiTypeId];
  if (pModule == nullptr)
  {
    pModule = wdWorldModuleFactory::GetInstance()->CreateWorldModule(uiTypeId, this);
    pModule->Initialize();

    m_Data.m_Modules[uiTypeId] = pModule;
    m_Data.m_ModulesToStartSimulation.PushBack(pModule);
  }

  return pModule;
}

void wdWorld::DeleteModule(const wdRTTI* pRtti)
{
  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = wdWorldModuleFactory::GetInstance()->GetTypeId(pRtti);
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    if (wdWorldModule* pModule = m_Data.m_Modules[uiTypeId])
    {
      m_Data.m_Modules[uiTypeId] = nullptr;

      pModule->Deinitialize();
      DeregisterUpdateFunctions(pModule);
      WD_DELETE(&m_Data.m_Allocator, pModule);
    }
  }
}

wdWorldModule* wdWorld::GetModule(const wdRTTI* pRtti)
{
  CheckForWriteAccess();

  const wdWorldModuleTypeId uiTypeId = wdWorldModuleFactory::GetInstance()->GetTypeId(pRtti);
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    return m_Data.m_Modules[uiTypeId];
  }

  return nullptr;
}

const wdWorldModule* wdWorld::GetModule(const wdRTTI* pRtti) const
{
  CheckForReadAccess();

  const wdWorldModuleTypeId uiTypeId = wdWorldModuleFactory::GetInstance()->GetTypeId(pRtti);
  if (uiTypeId < m_Data.m_Modules.GetCount())
  {
    return m_Data.m_Modules[uiTypeId];
  }

  return nullptr;
}

void wdWorld::SetParent(wdGameObject* pObject, wdGameObject* pNewParent, wdGameObject::TransformPreservation preserve)
{
  WD_ASSERT_DEV(pObject != pNewParent, "Object can't be its own parent!");
  WD_ASSERT_DEV(pNewParent == nullptr || pObject->IsDynamic() || pNewParent->IsStatic(), "Can't attach a static object to a dynamic parent!");
  CheckForWriteAccess();

  if (GetObjectUnchecked(pObject->m_uiParentIndex) == pNewParent)
    return;

  UnlinkFromParent(pObject);
  // UnlinkFromParent does not clear these as they are still needed in DeleteObjectNow to allow deletes while iterating.
  pObject->m_uiNextSiblingIndex = 0;
  pObject->m_uiPrevSiblingIndex = 0;
  if (pNewParent != nullptr)
  {
    // Ensure that the parent's global transform is up-to-date otherwise the object's local transform will be wrong afterwards.
    pNewParent->UpdateGlobalTransform();

    pObject->m_uiParentIndex = pNewParent->m_InternalId.m_InstanceIndex;
    LinkToParent(pObject);
  }

  PatchHierarchyData(pObject, preserve);

  // TODO: the functions above send messages such as wdMsgChildrenChanged, which will not arrive for inactive components, is that a problem ?
  // 1) if a component was active before and now gets deactivated, it may not care about the message anymore anyway
  // 2) if a component was inactive before, it did not get the message, but upon activation it can update the state for which it needed the message
  // so probably it is fine, only components that were active and stay active need the message, and that will be the case
  pObject->UpdateActiveState(pNewParent == nullptr ? true : pNewParent->IsActive());
}

void wdWorld::LinkToParent(wdGameObject* pObject)
{
  WD_ASSERT_DEBUG(pObject->m_uiNextSiblingIndex == 0 && pObject->m_uiPrevSiblingIndex == 0, "Object is either still linked to another parent or data was not cleared.");
  if (wdGameObject* pParentObject = pObject->GetParent())
  {
    const wdUInt32 uiIndex = pObject->m_InternalId.m_InstanceIndex;

    if (pParentObject->m_uiFirstChildIndex != 0)
    {
      pObject->m_uiPrevSiblingIndex = pParentObject->m_uiLastChildIndex;
      GetObjectUnchecked(pParentObject->m_uiLastChildIndex)->m_uiNextSiblingIndex = uiIndex;
    }
    else
    {
      pParentObject->m_uiFirstChildIndex = uiIndex;
    }

    pParentObject->m_uiLastChildIndex = uiIndex;
    pParentObject->m_uiChildCount++;

    pObject->m_pTransformationData->m_pParentData = pParentObject->m_pTransformationData;

    if (pObject->m_Flags.IsSet(wdObjectFlags::ParentChangesNotifications))
    {
      wdMsgParentChanged msg;
      msg.m_Type = wdMsgParentChanged::Type::ParentLinked;
      msg.m_hParent = pParentObject->GetHandle();

      pObject->SendMessage(msg);
    }

    if (pParentObject->m_Flags.IsSet(wdObjectFlags::ChildChangesNotifications))
    {
      wdMsgChildrenChanged msg;
      msg.m_Type = wdMsgChildrenChanged::Type::ChildAdded;
      msg.m_hParent = pParentObject->GetHandle();
      msg.m_hChild = pObject->GetHandle();

      pParentObject->SendNotificationMessage(msg);
    }
  }
}

void wdWorld::UnlinkFromParent(wdGameObject* pObject)
{
  if (wdGameObject* pParentObject = pObject->GetParent())
  {
    const wdUInt32 uiIndex = pObject->m_InternalId.m_InstanceIndex;

    if (uiIndex == pParentObject->m_uiFirstChildIndex)
      pParentObject->m_uiFirstChildIndex = pObject->m_uiNextSiblingIndex;

    if (uiIndex == pParentObject->m_uiLastChildIndex)
      pParentObject->m_uiLastChildIndex = pObject->m_uiPrevSiblingIndex;

    if (wdGameObject* pNextObject = GetObjectUnchecked(pObject->m_uiNextSiblingIndex))
      pNextObject->m_uiPrevSiblingIndex = pObject->m_uiPrevSiblingIndex;

    if (wdGameObject* pPrevObject = GetObjectUnchecked(pObject->m_uiPrevSiblingIndex))
      pPrevObject->m_uiNextSiblingIndex = pObject->m_uiNextSiblingIndex;

    pParentObject->m_uiChildCount--;
    pObject->m_uiParentIndex = 0;
    pObject->m_pTransformationData->m_pParentData = nullptr;

    if (pObject->m_Flags.IsSet(wdObjectFlags::ParentChangesNotifications))
    {
      wdMsgParentChanged msg;
      msg.m_Type = wdMsgParentChanged::Type::ParentUnlinked;
      msg.m_hParent = pParentObject->GetHandle();

      pObject->SendMessage(msg);
    }

    // Note that the sibling indices must not be set to 0 here.
    // They are still needed if we currently iterate over child objects.

    if (pParentObject->m_Flags.IsSet(wdObjectFlags::ChildChangesNotifications))
    {
      wdMsgChildrenChanged msg;
      msg.m_Type = wdMsgChildrenChanged::Type::ChildRemoved;
      msg.m_hParent = pParentObject->GetHandle();
      msg.m_hChild = pObject->GetHandle();

      pParentObject->SendNotificationMessage(msg);
    }
  }
}

void wdWorld::SetObjectGlobalKey(wdGameObject* pObject, const wdHashedString& sGlobalKey)
{
  if (m_Data.m_GlobalKeyToIdTable.Contains(sGlobalKey.GetHash()))
  {
    wdLog::Error("Can't set global key to '{0}' because an object with this global key already exists. Global keys have to be unique.", sGlobalKey);
    return;
  }

  const wdUInt32 uiId = pObject->m_InternalId.m_InstanceIndex;

  // Remove existing entry first.
  wdHashedString* pOldGlobalKey;
  if (m_Data.m_IdToGlobalKeyTable.TryGetValue(uiId, pOldGlobalKey))
  {
    if (sGlobalKey == *pOldGlobalKey)
    {
      return;
    }

    WD_VERIFY(m_Data.m_GlobalKeyToIdTable.Remove(pOldGlobalKey->GetHash()), "Implementation error.");
    WD_VERIFY(m_Data.m_IdToGlobalKeyTable.Remove(uiId), "Implementation error.");
  }

  // Insert new one if key is valid.
  if (!sGlobalKey.IsEmpty())
  {
    m_Data.m_GlobalKeyToIdTable.Insert(sGlobalKey.GetHash(), pObject->m_InternalId);
    m_Data.m_IdToGlobalKeyTable.Insert(uiId, sGlobalKey);
  }
}

wdStringView wdWorld::GetObjectGlobalKey(const wdGameObject* pObject) const
{
  const wdUInt32 uiId = pObject->m_InternalId.m_InstanceIndex;

  const wdHashedString* pGlobalKey;
  if (m_Data.m_IdToGlobalKeyTable.TryGetValue(uiId, pGlobalKey))
  {
    return pGlobalKey->GetView();
  }

  return {};
}

void wdWorld::ProcessQueuedMessage(const wdInternal::WorldData::MessageQueue::Entry& entry)
{
  if (entry.m_MetaData.m_uiReceiverIsComponent)
  {
    wdComponentHandle hComponent(wdComponentId(entry.m_MetaData.m_uiReceiverObjectOrComponent));

    wdComponent* pReceiverComponent = nullptr;
    if (TryGetComponent(hComponent, pReceiverComponent))
    {
      pReceiverComponent->SendMessageInternal(*entry.m_pMessage, true);
    }
    else
    {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
      if (entry.m_pMessage->GetDebugMessageRouting())
      {
        wdLog::Warning("wdWorld::ProcessQueuedMessage: Receiver wdComponent for message of type '{0}' does not exist anymore.", entry.m_pMessage->GetId());
      }
#endif
    }
  }
  else
  {
    wdGameObjectHandle hObject(wdGameObjectId(entry.m_MetaData.m_uiReceiverObjectOrComponent));

    wdGameObject* pReceiverObject = nullptr;
    if (TryGetObject(hObject, pReceiverObject))
    {
      if (entry.m_MetaData.m_uiRecursive)
      {
        pReceiverObject->SendMessageRecursiveInternal(*entry.m_pMessage, true);
      }
      else
      {
        pReceiverObject->SendMessageInternal(*entry.m_pMessage, true);
      }
    }
    else
    {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
      if (entry.m_pMessage->GetDebugMessageRouting())
      {
        wdLog::Warning("wdWorld::ProcessQueuedMessage: Receiver wdGameObject for message of type '{0}' does not exist anymore.", entry.m_pMessage->GetId());
      }
#endif
    }
  }
}

void wdWorld::ProcessQueuedMessages(wdObjectMsgQueueType::Enum queueType)
{
  WD_PROFILE_SCOPE("Process Queued Messages");

  struct MessageComparer
  {
    WD_FORCE_INLINE bool Less(const wdInternal::WorldData::MessageQueue::Entry& a, const wdInternal::WorldData::MessageQueue::Entry& b) const
    {
      if (a.m_MetaData.m_Due != b.m_MetaData.m_Due)
        return a.m_MetaData.m_Due < b.m_MetaData.m_Due;

      const wdInt32 iKeyA = a.m_pMessage->GetSortingKey();
      const wdInt32 iKeyB = b.m_pMessage->GetSortingKey();
      if (iKeyA != iKeyB)
        return iKeyA < iKeyB;

      if (a.m_pMessage->GetId() != b.m_pMessage->GetId())
        return a.m_pMessage->GetId() < b.m_pMessage->GetId();

      if (a.m_MetaData.m_uiReceiverData != b.m_MetaData.m_uiReceiverData)
        return a.m_MetaData.m_uiReceiverData < b.m_MetaData.m_uiReceiverData;

      if (a.m_uiMessageHash == 0)
      {
        a.m_uiMessageHash = a.m_pMessage->GetHash();
      }

      if (b.m_uiMessageHash == 0)
      {
        b.m_uiMessageHash = b.m_pMessage->GetHash();
      }

      return a.m_uiMessageHash < b.m_uiMessageHash;
    }
  };

  // regular messages
  {
    wdInternal::WorldData::MessageQueue& queue = m_Data.m_MessageQueues[queueType];
    queue.Sort(MessageComparer());

    for (wdUInt32 i = 0; i < queue.GetCount(); ++i)
    {
      ProcessQueuedMessage(queue[i]);

      // no need to deallocate these messages, they are allocated through a frame allocator
    }

    queue.Clear();
  }

  // timed messages
  {
    wdInternal::WorldData::MessageQueue& queue = m_Data.m_TimedMessageQueues[queueType];
    queue.Sort(MessageComparer());

    const wdTime now = m_Data.m_Clock.GetAccumulatedTime();

    while (!queue.IsEmpty())
    {
      auto& entry = queue.Peek();
      if (entry.m_MetaData.m_Due > now)
        break;

      ProcessQueuedMessage(entry);

      WD_DELETE(&m_Data.m_Allocator, entry.m_pMessage);

      queue.Dequeue();
    }
  }
}

// static
template <typename World, typename GameObject, typename Component>
void wdWorld::FindEventMsgHandlers(World& world, const wdMessage& msg, GameObject pSearchObject, wdDynamicArray<Component>& out_components)
{
  using EventMessageHandlerComponentType = typename std::conditional<std::is_const<World>::value, const wdEventMessageHandlerComponent*, wdEventMessageHandlerComponent*>::type;

  out_components.Clear();

  // walk the graph upwards until an object is found with at least one wdComponent that handles this type of message
  {
    auto pCurrentObject = pSearchObject;

    while (pCurrentObject != nullptr)
    {
      bool bContinueSearch = true;
      for (auto pComponent : pCurrentObject->GetComponents())
      {
        if constexpr (std::is_const<World>::value == false)
        {
          pComponent->EnsureInitialized();
        }

        if (pComponent->HandlesMessage(msg))
        {
          out_components.PushBack(pComponent);
          bContinueSearch = false;
        }
        else
        {
          if constexpr (std::is_const<World>::value)
          {
            if (pComponent->IsInitialized() == false)
            {
              wdLog::Warning("Component of type '{}' was not initialized (yet) and thus might have reported an incorrect result in HandlesMessage(). "
                             "To allow this component to be automatically initialized at this point in time call the non-const variant of SendEventMessage.",
                pComponent->GetDynamicRTTI()->GetTypeName());
            }
          }

          // only continue to search on parent objects if all event handlers on the current object have the "pass through unhandled events" flag set.
          if (auto pEventMessageHandlerComponent = wdDynamicCast<EventMessageHandlerComponentType>(pComponent))
          {
            bContinueSearch &= pEventMessageHandlerComponent->GetPassThroughUnhandledEvents();
          }
        }
      }

      if (!bContinueSearch)
      {
        // stop searching as we found at least one wdEventMessageHandlerComponent or one doesn't have the "pass through" flag set.
        return;
      }

      pCurrentObject = pCurrentObject->GetParent();
    }
  }

  // if no components have been found, check all event handler components that are registered as 'global event handlers'
  if (out_components.IsEmpty())
  {
    auto globalEventMessageHandler = wdEventMessageHandlerComponent::GetAllGlobalEventHandler(&world);
    for (auto hEventMessageHandlerComponent : globalEventMessageHandler)
    {
      EventMessageHandlerComponentType pEventMessageHandlerComponent = nullptr;
      if (world.TryGetComponent(hEventMessageHandlerComponent, pEventMessageHandlerComponent))
      {
        if (pEventMessageHandlerComponent->HandlesMessage(msg))
        {
          out_components.PushBack(pEventMessageHandlerComponent);
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void wdWorld::RegisterUpdateFunction(const wdComponentManagerBase::UpdateFunctionDesc& desc)
{
  CheckForWriteAccess();

  WD_ASSERT_DEV(desc.m_Phase == wdComponentManagerBase::UpdateFunctionDesc::Phase::Async || desc.m_uiGranularity == 0, "Granularity must be 0 for synchronous update functions");
  WD_ASSERT_DEV(desc.m_Phase != wdComponentManagerBase::UpdateFunctionDesc::Phase::Async || desc.m_DependsOn.GetCount() == 0, "Asynchronous update functions must not have dependencies");
  WD_ASSERT_DEV(desc.m_Function.IsComparable(), "Delegates with captures are not allowed as wdWorld update functions.");

  m_Data.m_UpdateFunctionsToRegister.PushBack(desc);
}

void wdWorld::DeregisterUpdateFunction(const wdComponentManagerBase::UpdateFunctionDesc& desc)
{
  CheckForWriteAccess();

  wdDynamicArrayBase<wdInternal::WorldData::RegisteredUpdateFunction>& updateFunctions = m_Data.m_UpdateFunctions[desc.m_Phase.GetValue()];

  for (wdUInt32 i = updateFunctions.GetCount(); i-- > 0;)
  {
    if (updateFunctions[i].m_Function.IsEqualIfComparable(desc.m_Function))
    {
      updateFunctions.RemoveAtAndCopy(i);
    }
  }
}

void wdWorld::DeregisterUpdateFunctions(wdWorldModule* pModule)
{
  CheckForWriteAccess();

  for (wdUInt32 phase = wdWorldModule::UpdateFunctionDesc::Phase::PreAsync; phase < wdWorldModule::UpdateFunctionDesc::Phase::COUNT; ++phase)
  {
    wdDynamicArrayBase<wdInternal::WorldData::RegisteredUpdateFunction>& updateFunctions = m_Data.m_UpdateFunctions[phase];

    for (wdUInt32 i = updateFunctions.GetCount(); i-- > 0;)
    {
      if (updateFunctions[i].m_Function.GetClassInstance() == pModule)
      {
        updateFunctions.RemoveAtAndCopy(i);
      }
    }
  }
}

void wdWorld::AddComponentToInitialize(wdComponentHandle hComponent)
{
  m_Data.m_pCurrentInitBatch->m_ComponentsToInitialize.PushBack(hComponent);
}

void wdWorld::UpdateFromThread()
{
  WD_LOCK(GetWriteMarker());

  Update();
}

void wdWorld::UpdateSynchronous(const wdArrayPtr<wdInternal::WorldData::RegisteredUpdateFunction>& updateFunctions)
{
  wdWorldModule::UpdateContext context;
  context.m_uiFirstComponentIndex = 0;
  context.m_uiComponentCount = wdInvalidIndex;

  for (auto& updateFunction : updateFunctions)
  {
    if (updateFunction.m_bOnlyUpdateWhenSimulating && !m_Data.m_bSimulateWorld)
      continue;

    {
      WD_PROFILE_SCOPE(updateFunction.m_sFunctionName);
      updateFunction.m_Function(context);
    }
  }
}

void wdWorld::UpdateAsynchronous()
{
  wdTaskGroupID taskGroupId = wdTaskSystem::CreateTaskGroup(wdTaskPriority::EarlyThisFrame);

  wdDynamicArrayBase<wdInternal::WorldData::RegisteredUpdateFunction>& updateFunctions = m_Data.m_UpdateFunctions[wdComponentManagerBase::UpdateFunctionDesc::Phase::Async];

  wdUInt32 uiCurrentTaskIndex = 0;

  for (auto& updateFunction : updateFunctions)
  {
    if (updateFunction.m_bOnlyUpdateWhenSimulating && !m_Data.m_bSimulateWorld)
      continue;

    wdWorldModule* pModule = static_cast<wdWorldModule*>(updateFunction.m_Function.GetClassInstance());
    wdComponentManagerBase* pManager = wdDynamicCast<wdComponentManagerBase*>(pModule);

    // a world module can also register functions in the async phase so we want at least one task
    const wdUInt32 uiTotalCount = pManager != nullptr ? pManager->GetComponentCount() : 1;
    const wdUInt32 uiGranularity = (updateFunction.m_uiGranularity != 0) ? updateFunction.m_uiGranularity : uiTotalCount;

    wdUInt32 uiStartIndex = 0;
    while (uiStartIndex < uiTotalCount)
    {
      wdSharedPtr<wdInternal::WorldData::UpdateTask> pTask;
      if (uiCurrentTaskIndex < m_Data.m_UpdateTasks.GetCount())
      {
        pTask = m_Data.m_UpdateTasks[uiCurrentTaskIndex];
      }
      else
      {
        pTask = WD_NEW(&m_Data.m_Allocator, wdInternal::WorldData::UpdateTask);
        m_Data.m_UpdateTasks.PushBack(pTask);
      }

      pTask->ConfigureTask(updateFunction.m_sFunctionName, wdTaskNesting::Maybe);
      pTask->m_Function = updateFunction.m_Function;
      pTask->m_uiStartIndex = uiStartIndex;
      pTask->m_uiCount = (uiStartIndex + uiGranularity < uiTotalCount) ? uiGranularity : wdInvalidIndex;
      wdTaskSystem::AddTaskToGroup(taskGroupId, pTask);

      ++uiCurrentTaskIndex;
      uiStartIndex += uiGranularity;
    }
  }

  wdTaskSystem::StartTaskGroup(taskGroupId);
  wdTaskSystem::WaitForGroup(taskGroupId);
}

bool wdWorld::ProcessInitializationBatch(wdInternal::WorldData::InitBatch& batch, wdTime endTime)
{
  CheckForWriteAccess();

  // ensure that all components that are created during this batch (e.g. from prefabs)
  // will also get initialized within this batch
  m_Data.m_pCurrentInitBatch = &batch;
  WD_SCOPE_EXIT(m_Data.m_pCurrentInitBatch = m_Data.m_pDefaultInitBatch);

  if (!batch.m_ComponentsToInitialize.IsEmpty())
  {
    wdStringBuilder profileScopeName("Init ", batch.m_sName);
    WD_PROFILE_SCOPE(profileScopeName);

    // Reserve for later use
    batch.m_ComponentsToStartSimulation.Reserve(batch.m_ComponentsToInitialize.GetCount());

    // Can't use foreach here because the array might be resized during iteration.
    for (; batch.m_uiNextComponentToInitialize < batch.m_ComponentsToInitialize.GetCount(); ++batch.m_uiNextComponentToInitialize)
    {
      wdComponentHandle hComponent = batch.m_ComponentsToInitialize[batch.m_uiNextComponentToInitialize];

      // if it is in the editor, the component might have been added and already deleted, without ever running the simulation
      wdComponent* pComponent = nullptr;
      if (!TryGetComponent(hComponent, pComponent))
        continue;

      WD_ASSERT_DEBUG(pComponent->GetOwner() != nullptr, "Component must have a valid owner");

      // make sure the object's transform is up to date before the component is initialized.
      pComponent->GetOwner()->UpdateGlobalTransform();

      pComponent->EnsureInitialized();

      if (pComponent->IsActive())
      {
        pComponent->OnActivated();

        batch.m_ComponentsToStartSimulation.PushBack(hComponent);
      }

      // Check if there is still time left to initialize more components
      if (wdTime::Now() >= endTime)
      {
        ++batch.m_uiNextComponentToInitialize;
        return false;
      }
    }

    batch.m_ComponentsToInitialize.Clear();
    batch.m_uiNextComponentToInitialize = 0;
  }

  if (m_Data.m_bSimulateWorld)
  {
    wdStringBuilder startSimName("Start Sim ", batch.m_sName);
    WD_PROFILE_SCOPE(startSimName);

    // Can't use foreach here because the array might be resized during iteration.
    for (; batch.m_uiNextComponentToStartSimulation < batch.m_ComponentsToStartSimulation.GetCount(); ++batch.m_uiNextComponentToStartSimulation)
    {
      wdComponentHandle hComponent = batch.m_ComponentsToStartSimulation[batch.m_uiNextComponentToStartSimulation];

      // if it is in the editor, the component might have been added and already deleted,  without ever running the simulation
      wdComponent* pComponent = nullptr;
      if (!TryGetComponent(hComponent, pComponent))
        continue;

      if (pComponent->IsActiveAndInitialized())
      {
        pComponent->EnsureSimulationStarted();
      }

      // Check if there is still time left to initialize more components
      if (wdTime::Now() >= endTime)
      {
        ++batch.m_uiNextComponentToStartSimulation;
        return false;
      }
    }

    batch.m_ComponentsToStartSimulation.Clear();
    batch.m_uiNextComponentToStartSimulation = 0;
  }

  return true;
}

void wdWorld::ProcessComponentsToInitialize()
{
  CheckForWriteAccess();

  if (m_Data.m_bSimulateWorld)
  {
    WD_PROFILE_SCOPE("Modules Start Simulation");

    // Can't use foreach here because the array might be resized during iteration.
    for (wdUInt32 i = 0; i < m_Data.m_ModulesToStartSimulation.GetCount(); ++i)
    {
      m_Data.m_ModulesToStartSimulation[i]->OnSimulationStarted();
    }

    m_Data.m_ModulesToStartSimulation.Clear();
  }

  WD_PROFILE_SCOPE("Initialize Components");

  wdTime endTime = wdTime::Now() + m_Data.m_MaxInitializationTimePerFrame;

  // First process all component init batches that have to finish within this frame
  for (auto it = m_Data.m_InitBatches.GetIterator(); it.IsValid(); ++it)
  {
    auto& pInitBatch = it.Value();
    if (pInitBatch->m_bIsReady && pInitBatch->m_bMustFinishWithinOneFrame)
    {
      ProcessInitializationBatch(*pInitBatch, wdTime::Now() + wdTime::Hours(10000));
    }
  }

  // If there is still time left process other component init batches
  if (wdTime::Now() < endTime)
  {
    for (auto it = m_Data.m_InitBatches.GetIterator(); it.IsValid(); ++it)
    {
      auto& pInitBatch = it.Value();
      if (!pInitBatch->m_bIsReady || pInitBatch->m_bMustFinishWithinOneFrame)
        continue;

      if (!ProcessInitializationBatch(*pInitBatch, endTime))
        return;
    }
  }
}

void wdWorld::ProcessUpdateFunctionsToRegister()
{
  CheckForWriteAccess();

  if (m_Data.m_UpdateFunctionsToRegister.IsEmpty())
    return;

  WD_PROFILE_SCOPE("Register update functions");

  while (!m_Data.m_UpdateFunctionsToRegister.IsEmpty())
  {
    const wdUInt32 uiNumFunctionsToRegister = m_Data.m_UpdateFunctionsToRegister.GetCount();

    for (wdUInt32 i = uiNumFunctionsToRegister; i-- > 0;)
    {
      if (RegisterUpdateFunctionInternal(m_Data.m_UpdateFunctionsToRegister[i]).Succeeded())
      {
        m_Data.m_UpdateFunctionsToRegister.RemoveAtAndCopy(i);
      }
    }

    WD_ASSERT_DEV(m_Data.m_UpdateFunctionsToRegister.GetCount() < uiNumFunctionsToRegister, "No functions have been registered because the dependencies could not be found.");
  }
}

wdResult wdWorld::RegisterUpdateFunctionInternal(const wdWorldModule::UpdateFunctionDesc& desc)
{
  wdDynamicArrayBase<wdInternal::WorldData::RegisteredUpdateFunction>& updateFunctions = m_Data.m_UpdateFunctions[desc.m_Phase.GetValue()];
  wdUInt32 uiInsertionIndex = 0;

  for (wdUInt32 i = 0; i < desc.m_DependsOn.GetCount(); ++i)
  {
    wdUInt32 uiDependencyIndex = wdInvalidIndex;

    for (wdUInt32 j = 0; j < updateFunctions.GetCount(); ++j)
    {
      if (updateFunctions[j].m_sFunctionName == desc.m_DependsOn[i])
      {
        uiDependencyIndex = j;
        break;
      }
    }

    if (uiDependencyIndex == wdInvalidIndex) // dependency not found
    {
      return WD_FAILURE;
    }
    else
    {
      uiInsertionIndex = wdMath::Max(uiInsertionIndex, uiDependencyIndex + 1);
    }
  }

  wdInternal::WorldData::RegisteredUpdateFunction newFunction;
  newFunction.FillFromDesc(desc);

  while (uiInsertionIndex < updateFunctions.GetCount())
  {
    const auto& existingFunction = updateFunctions[uiInsertionIndex];
    if (newFunction < existingFunction)
    {
      break;
    }

    ++uiInsertionIndex;
  }

  updateFunctions.Insert(newFunction, uiInsertionIndex);

  return WD_SUCCESS;
}

void wdWorld::DeleteDeadObjects()
{
  while (!m_Data.m_DeadObjects.IsEmpty())
  {
    wdGameObject* pObject = m_Data.m_DeadObjects.GetIterator().Key();

    if (!pObject->m_pTransformationData->m_hSpatialData.IsInvalidated())
    {
      m_Data.m_pSpatialSystem->DeleteSpatialData(pObject->m_pTransformationData->m_hSpatialData);
    }

    m_Data.DeleteTransformationData(pObject->IsDynamic(), pObject->m_uiHierarchyLevel, pObject->m_pTransformationData);

    wdGameObject* pMovedObject = nullptr;
    m_Data.m_ObjectStorage.Delete(pObject, pMovedObject);

    if (pObject != pMovedObject)
    {
      // patch the id table: the last element in the storage has been moved to deleted object's location,
      // thus the pointer now points to another object
      wdGameObjectId id = pObject->m_InternalId;
      if (id.m_InstanceIndex != wdGameObjectId::INVALID_INSTANCE_INDEX)
        m_Data.m_Objects[id] = pObject;

      // The moved object might be deleted as well so we remove it from the dead objects set instead.
      // If that is not the case we remove the original object from the set.
      if (m_Data.m_DeadObjects.Remove(pMovedObject))
      {
        continue;
      }
    }

    m_Data.m_DeadObjects.Remove(pObject);
  }
}

void wdWorld::DeleteDeadComponents()
{
  while (!m_Data.m_DeadComponents.IsEmpty())
  {
    wdComponent* pComponent = m_Data.m_DeadComponents.GetIterator().Key();

    wdComponentManagerBase* pManager = pComponent->GetOwningManager();
    wdComponent* pMovedComponent = nullptr;
    pManager->DeleteComponentStorage(pComponent, pMovedComponent);

    // another component has been moved to the deleted component location
    if (pComponent != pMovedComponent)
    {
      pManager->PatchIdTable(pComponent);

      if (wdGameObject* pOwner = pComponent->GetOwner())
      {
        pOwner->FixComponentPointer(pMovedComponent, pComponent);
      }

      // The moved component might be deleted as well so we remove it from the dead components set instead.
      // If that is not the case we remove the original component from the set.
      if (m_Data.m_DeadComponents.Remove(pMovedComponent))
      {
        continue;
      }
    }

    m_Data.m_DeadComponents.Remove(pComponent);
  }
}

void wdWorld::PatchHierarchyData(wdGameObject* pObject, wdGameObject::TransformPreservation preserve)
{
  wdGameObject* pParent = pObject->GetParent();

  RecreateHierarchyData(pObject, pObject->IsDynamic());

  pObject->m_pTransformationData->m_pParentData = pParent != nullptr ? pParent->m_pTransformationData : nullptr;

  if (preserve == wdGameObject::TransformPreservation::PreserveGlobal)
  {
    // SetGlobalTransform will internally trigger bounds update for static objects
    pObject->SetGlobalTransform(pObject->m_pTransformationData->m_globalTransform);
  }
  else
  {
    // Explicitly trigger transform AND bounds update, otherwise bounds would be outdated for static objects
    // Don't call pObject->UpdateGlobalTransformAndBounds() here since that would recursively update the parent global transform which is already up-to-date.
    pObject->m_pTransformationData->UpdateGlobalTransformNonRecursive();

    pObject->m_pTransformationData->UpdateGlobalBounds(GetSpatialSystem());
  }

  for (auto it = pObject->GetChildren(); it.IsValid(); ++it)
  {
    PatchHierarchyData(it, preserve);
  }
  WD_ASSERT_DEBUG(pObject->m_pTransformationData != pObject->m_pTransformationData->m_pParentData, "Hierarchy corrupted!");
}

void wdWorld::RecreateHierarchyData(wdGameObject* pObject, bool bWasDynamic)
{
  wdGameObject* pParent = pObject->GetParent();

  const wdUInt32 uiNewHierarchyLevel = pParent != nullptr ? pParent->m_uiHierarchyLevel + 1 : 0;
  const wdUInt32 uiOldHierarchyLevel = pObject->m_uiHierarchyLevel;

  const bool bIsDynamic = pObject->IsDynamic();

  if (uiNewHierarchyLevel != uiOldHierarchyLevel || bIsDynamic != bWasDynamic)
  {
    wdGameObject::TransformationData* pOldTransformationData = pObject->m_pTransformationData;

    wdGameObject::TransformationData* pNewTransformationData = m_Data.CreateTransformationData(bIsDynamic, uiNewHierarchyLevel);
    wdMemoryUtils::Copy(pNewTransformationData, pOldTransformationData, 1);

    pObject->m_uiHierarchyLevel = static_cast<wdUInt16>(uiNewHierarchyLevel);
    pObject->m_pTransformationData = pNewTransformationData;

    // fix parent transform data for children as well
    for (auto it = pObject->GetChildren(); it.IsValid(); ++it)
    {
      wdGameObject::TransformationData* pTransformData = it->m_pTransformationData;
      pTransformData->m_pParentData = pNewTransformationData;
    }

    m_Data.DeleteTransformationData(bWasDynamic, uiOldHierarchyLevel, pOldTransformationData);
  }
}

void wdWorld::SetMaxInitializationTimePerFrame(wdTime maxInitTime)
{
  CheckForWriteAccess();

  m_Data.m_MaxInitializationTimePerFrame = maxInitTime;
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_World);
