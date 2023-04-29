#include <Core/CorePCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Foundation/IO/StringDeduplicationContext.h>
#include <Foundation/Types/ScopeExit.h>
#include <Foundation/Utilities/Progress.h>

wdWorldReader::FindComponentTypeCallback wdWorldReader::s_FindComponentTypeCallback;

wdWorldReader::wdWorldReader() = default;
wdWorldReader::~wdWorldReader() = default;

wdResult wdWorldReader::ReadWorldDescription(wdStreamReader& inout_stream, bool bWarningOnUknownSkip)
{
  m_pStream = &inout_stream;

  m_uiVersion = 0;
  inout_stream >> m_uiVersion;

  if (m_uiVersion < 8 || m_uiVersion > 10)
  {
    wdLog::Error("Invalid world version (got {}).", m_uiVersion);
    return WD_FAILURE;
  }

  // destroy old context first
  m_pStringDedupReadContext = nullptr;
  m_pStringDedupReadContext = WD_DEFAULT_NEW(wdStringDeduplicationReadContext, inout_stream);

  if (m_uiVersion == 8)
  {
    // add tags from the stream
    WD_SUCCEED_OR_RETURN(wdTagRegistry::GetGlobalRegistry().Load(inout_stream));
  }

  wdUInt32 uiNumRootObjects = 0;
  inout_stream >> uiNumRootObjects;

  wdUInt32 uiNumChildObjects = 0;
  inout_stream >> uiNumChildObjects;

  wdUInt32 uiNumComponentTypes = 0;
  inout_stream >> uiNumComponentTypes;

  if (uiNumComponentTypes > wdMath::MaxValue<wdUInt16>())
  {
    wdLog::Error("World description has too many component types, got {0} - maximum allowed are {1}", uiNumComponentTypes, wdMath::MaxValue<wdUInt16>());
    return WD_FAILURE;
  }

  m_RootObjectsToCreate.Reserve(uiNumRootObjects);
  m_ChildObjectsToCreate.Reserve(uiNumChildObjects);

  m_IndexToGameObjectHandle.SetCountUninitialized(uiNumRootObjects + uiNumChildObjects + 1);

  for (wdUInt32 i = 0; i < uiNumRootObjects; ++i)
  {
    ReadGameObjectDesc(m_RootObjectsToCreate.ExpandAndGetRef());
  }

  for (wdUInt32 i = 0; i < uiNumChildObjects; ++i)
  {
    ReadGameObjectDesc(m_ChildObjectsToCreate.ExpandAndGetRef());
  }

  m_ComponentTypes.SetCount(uiNumComponentTypes);
  m_ComponentTypeVersions.Reserve(uiNumComponentTypes);
  for (wdUInt32 i = 0; i < uiNumComponentTypes; ++i)
  {
    ReadComponentTypeInfo(i);
  }

  // read all component data
  ReadComponentDataToMemStream(bWarningOnUknownSkip);
  m_pStringDedupReadContext->SetActive(false);

  return WD_SUCCESS;
}

wdUniquePtr<wdWorldReader::InstantiationContextBase> wdWorldReader::InstantiateWorld(wdWorld& ref_world, const wdUInt16* pOverrideTeamID, wdTime maxStepTime, wdProgress* pProgress)
{
  wdPrefabInstantiationOptions options;
  options.m_pOverrideTeamID = pOverrideTeamID;
  options.m_MaxStepTime = maxStepTime;
  options.m_pProgress = pProgress;
  options.m_RandomSeedMode = wdPrefabInstantiationOptions::RandomSeedMode::FixedFromSerialization;

  return Instantiate(ref_world, false, wdTransform(), options);
}

wdUniquePtr<wdWorldReader::InstantiationContextBase> wdWorldReader::InstantiatePrefab(wdWorld& ref_world, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options)
{
  return Instantiate(ref_world, true, rootTransform, options);
}

wdGameObjectHandle wdWorldReader::ReadGameObjectHandle()
{
  wdUInt32 idx = 0;
  *m_pStream >> idx;

  return m_IndexToGameObjectHandle[idx];
}

void wdWorldReader::ReadComponentHandle(wdComponentHandle& out_hComponent)
{
  wdUInt16 uiTypeIndex = 0;
  wdUInt32 uiIndex = 0;

  *m_pStream >> uiTypeIndex;
  *m_pStream >> uiIndex;

  out_hComponent.Invalidate();

  if (uiTypeIndex < m_ComponentTypes.GetCount())
  {
    auto& indexToHandle = m_ComponentTypes[uiTypeIndex].m_ComponentIndexToHandle;
    if (uiIndex < indexToHandle.GetCount())
    {
      out_hComponent = indexToHandle[uiIndex];
    }
  }
}

wdUInt32 wdWorldReader::GetComponentTypeVersion(const wdRTTI* pRtti) const
{
  wdUInt32 uiVersion = 0xFFFFFFFF;
  m_ComponentTypeVersions.TryGetValue(pRtti, uiVersion);

  return uiVersion;
}

void wdWorldReader::ClearAndCompact()
{
  m_IndexToGameObjectHandle.Clear();
  m_IndexToGameObjectHandle.Compact();

  m_RootObjectsToCreate.Clear();
  m_RootObjectsToCreate.Compact();

  m_ChildObjectsToCreate.Clear();
  m_ChildObjectsToCreate.Compact();

  m_ComponentTypes.Clear();
  m_ComponentTypes.Compact();

  m_ComponentTypeVersions.Clear();
  m_ComponentTypeVersions.Compact();

  m_ComponentCreationStream.Clear();
  m_ComponentCreationStream.Compact();

  m_ComponentDataStream.Clear();
  m_ComponentDataStream.Compact();
}

wdUInt64 wdWorldReader::GetHeapMemoryUsage() const
{
  return m_IndexToGameObjectHandle.GetHeapMemoryUsage() + m_RootObjectsToCreate.GetHeapMemoryUsage() + m_ChildObjectsToCreate.GetHeapMemoryUsage() + m_ComponentTypes.GetHeapMemoryUsage() + m_ComponentTypeVersions.GetHeapMemoryUsage() + m_ComponentCreationStream.GetHeapMemoryUsage() +
         m_ComponentDataStream.GetHeapMemoryUsage();
}

wdUInt32 wdWorldReader::GetRootObjectCount() const
{
  return m_RootObjectsToCreate.GetCount();
}


wdUInt32 wdWorldReader::GetChildObjectCount() const
{
  return m_ChildObjectsToCreate.GetCount();
}

void wdWorldReader::SetMaxStepTime(InstantiationContextBase* pContext, wdTime maxStepTime)
{
  return static_cast<InstantiationContext*>(pContext)->SetMaxStepTime(maxStepTime);
}

wdTime wdWorldReader::GetMaxStepTime(InstantiationContextBase* pContext)
{
  return static_cast<InstantiationContext*>(pContext)->GetMaxStepTime();
}

void wdWorldReader::ReadGameObjectDesc(GameObjectToCreate& godesc)
{
  wdGameObjectDesc& desc = godesc.m_Desc;
  wdStringBuilder sName, sGlobalKey;

  *m_pStream >> godesc.m_uiParentHandleIdx;
  *m_pStream >> sName;

  *m_pStream >> sGlobalKey;
  godesc.m_sGlobalKey = sGlobalKey;

  *m_pStream >> desc.m_LocalPosition;
  *m_pStream >> desc.m_LocalRotation;
  *m_pStream >> desc.m_LocalScaling;
  *m_pStream >> desc.m_LocalUniformScaling;

  *m_pStream >> desc.m_bActiveFlag;
  *m_pStream >> desc.m_bDynamic;

  desc.m_Tags.Load(*m_pStream, wdTagRegistry::GetGlobalRegistry());

  *m_pStream >> desc.m_uiTeamID;

  desc.m_sName.Assign(sName.GetData());

  if (m_uiVersion >= 10)
  {
    *m_pStream >> desc.m_uiStableRandomSeed;
  }
}

void wdWorldReader::ReadComponentTypeInfo(wdUInt32 uiComponentTypeIdx)
{
  wdStreamReader& s = *m_pStream;

  wdStringBuilder sRttiName;
  wdUInt32 uiRttiVersion = 0;

  s >> sRttiName;
  s >> uiRttiVersion;

  const wdRTTI* pRtti = nullptr;

  if (s_FindComponentTypeCallback.IsValid())
  {
    pRtti = s_FindComponentTypeCallback(sRttiName);
  }
  else
  {
    pRtti = wdRTTI::FindTypeByName(sRttiName);

    if (pRtti == nullptr)
    {
      wdLog::Error("Unknown component type '{0}'. Components of this type will be skipped.", sRttiName);
    }
  }

  m_ComponentTypes[uiComponentTypeIdx].m_pRtti = pRtti;
  m_ComponentTypeVersions[pRtti] = uiRttiVersion;
}

void wdWorldReader::ReadComponentDataToMemStream(bool warningOnUnknownSkip)
{
  auto WriteToMemStream = [&](wdMemoryStreamWriter& ref_writer, bool bReadNumComponents) {
    wdUInt8 Temp[4096];
    for (auto& compTypeInfo : m_ComponentTypes)
    {
      wdUInt32 uiAllComponentsSize = 0;
      *m_pStream >> uiAllComponentsSize;

      if (compTypeInfo.m_pRtti == nullptr)
      {
        if (warningOnUnknownSkip)
        {
          wdLog::Warning("Skipping components of unknown type");
        }

        m_pStream->SkipBytes(uiAllComponentsSize);
      }
      else
      {
        if (bReadNumComponents)
        {
          *m_pStream >> compTypeInfo.m_uiNumComponents;
          uiAllComponentsSize -= sizeof(wdUInt32);

          m_uiTotalNumComponents += compTypeInfo.m_uiNumComponents;
        }

        while (uiAllComponentsSize > 0)
        {
          const wdUInt64 uiRead = m_pStream->ReadBytes(Temp, wdMath::Min<wdUInt32>(uiAllComponentsSize, WD_ARRAY_SIZE(Temp)));

          ref_writer.WriteBytes(Temp, uiRead).IgnoreResult();

          uiAllComponentsSize -= (wdUInt32)uiRead;
        }
      }
    }
  };

  {
    wdMemoryStreamWriter writer(&m_ComponentCreationStream);
    WriteToMemStream(writer, true);
  }

  {
    wdMemoryStreamWriter writer(&m_ComponentDataStream);
    WriteToMemStream(writer, false);
  }
}

void wdWorldReader::ClearHandles()
{
  m_IndexToGameObjectHandle.Clear();
  m_IndexToGameObjectHandle.PushBack(wdGameObjectHandle());

  for (auto& compTypeInfo : m_ComponentTypes)
  {
    compTypeInfo.m_ComponentIndexToHandle.Clear();
    compTypeInfo.m_ComponentIndexToHandle.PushBack(wdComponentHandle());
  }
}

wdUniquePtr<wdWorldReader::InstantiationContextBase> wdWorldReader::Instantiate(wdWorld& world, bool bUseTransform, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options)
{
  m_pWorld = &world;

  ClearHandles();

  if (options.m_MaxStepTime <= wdTime::Zero())
  {
    InstantiationContext context = InstantiationContext(*this, bUseTransform, rootTransform, options);

    WD_VERIFY(context.Step() == InstantiationContextBase::StepResult::Finished, "Instantiation should be completed after this call");
    return nullptr;
  }

  wdUniquePtr<InstantiationContext> pContext = WD_DEFAULT_NEW(InstantiationContext, *this, bUseTransform, rootTransform, options);

  return std::move(pContext);
}

wdWorldReader::InstantiationContext::InstantiationContext(wdWorldReader& ref_worldReader, bool bUseTransform, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options)
  : m_WorldReader(ref_worldReader)
  , m_bUseTransform(bUseTransform)
  , m_RootTransform(rootTransform)
  , m_Options(options)
{
  m_Phase = Phase::CreateRootObjects;

  if (m_Options.m_MaxStepTime.IsZeroOrNegative())
  {
    m_Options.m_MaxStepTime = wdTime::Hours(24 * 365);
  }

  if (options.m_MaxStepTime.IsPositive())
  {
    m_hComponentInitBatch = ref_worldReader.m_pWorld->CreateComponentInitBatch("WorldReaderBatch", options.m_MaxStepTime.IsPositive() ? false : true);
  }

  if (options.m_pProgress != nullptr)
  {
    m_pOverallProgressRange = WD_DEFAULT_NEW(wdProgressRange, "Instantiate", Phase::Count, false, options.m_pProgress);
    m_pOverallProgressRange->SetStepWeighting(Phase::CreateRootObjects, m_WorldReader.m_RootObjectsToCreate.GetCount() / 100.0f);
    m_pOverallProgressRange->SetStepWeighting(Phase::CreateChildObjects, m_WorldReader.m_ChildObjectsToCreate.GetCount() / 100.0f);
    m_pOverallProgressRange->SetStepWeighting(Phase::CreateComponents, m_WorldReader.m_uiTotalNumComponents / 100.0f);
    m_pOverallProgressRange->SetStepWeighting(Phase::DeserializeComponents, m_WorldReader.m_uiTotalNumComponents / 100.0f);
    // Ten times more weight since init components takes way longer than the rest
    m_pOverallProgressRange->SetStepWeighting(Phase::InitComponents, m_WorldReader.m_uiTotalNumComponents / 10.0f);

    m_pOverallProgressRange->BeginNextStep("CreateRootObjects");
  }
}

wdWorldReader::InstantiationContext::~InstantiationContext()
{
  if (!m_hComponentInitBatch.IsInvalidated())
  {
    m_WorldReader.m_pWorld->DeleteComponentInitBatch(m_hComponentInitBatch);
    m_hComponentInitBatch.Invalidate();
  }
}

wdWorldReader::InstantiationContext::StepResult wdWorldReader::InstantiationContext::Step()
{
  WD_ASSERT_DEV(m_Phase != Phase::Invalid, "InstantiationContext cannot be re-used.");

  WD_PROFILE_SCOPE("wdWorldReader::InstContext::Step");

  WD_LOCK(m_WorldReader.m_pWorld->GetWriteMarker());

  wdTime endTime = wdTime::Now() + m_Options.m_MaxStepTime;

  if (m_Phase == Phase::CreateRootObjects)
  {
    if (m_bUseTransform)
    {
      if (!CreateGameObjects<true>(m_WorldReader.m_RootObjectsToCreate, m_Options.m_hParent, m_Options.m_pCreatedRootObjectsOut, endTime))
        return StepResult::Continue;
    }
    else
    {
      if (!CreateGameObjects<false>(m_WorldReader.m_RootObjectsToCreate, m_Options.m_hParent, m_Options.m_pCreatedRootObjectsOut, endTime))
        return StepResult::Continue;
    }

    m_Phase = Phase::CreateChildObjects;
    BeginNextProgressStep("CreateChildObjects");
  }

  if (m_Phase == Phase::CreateChildObjects)
  {
    if (!CreateGameObjects<false>(m_WorldReader.m_ChildObjectsToCreate, wdGameObjectHandle(), m_Options.m_pCreatedChildObjectsOut, endTime))
      return StepResult::Continue;

    m_CurrentReader.SetStorage(&m_WorldReader.m_ComponentCreationStream);
    m_Phase = Phase::CreateComponents;
    BeginNextProgressStep("CreateComponents");
  }

  if (m_Phase == Phase::CreateComponents)
  {
    if (m_WorldReader.m_ComponentCreationStream.GetStorageSize64() > 0)
    {
      m_WorldReader.m_pStringDedupReadContext->SetActive(true);

      wdStreamReader* pPrevReader = m_WorldReader.m_pStream;
      m_WorldReader.m_pStream = &m_CurrentReader;

      WD_SCOPE_EXIT(m_WorldReader.m_pStream = pPrevReader; m_WorldReader.m_pStringDedupReadContext->SetActive(false););

      if (!CreateComponents(endTime))
        return StepResult::Continue;
    }

    m_CurrentReader.SetStorage(&m_WorldReader.m_ComponentDataStream);
    m_Phase = Phase::DeserializeComponents;
    BeginNextProgressStep("DeserializeComponents");
  }

  if (m_Phase == Phase::DeserializeComponents)
  {
    if (m_WorldReader.m_ComponentDataStream.GetStorageSize64() > 0)
    {
      m_WorldReader.m_pStringDedupReadContext->SetActive(true);

      wdStreamReader* pPrevReader = m_WorldReader.m_pStream;
      m_WorldReader.m_pStream = &m_CurrentReader;

      WD_SCOPE_EXIT(m_WorldReader.m_pStream = pPrevReader; m_WorldReader.m_pStringDedupReadContext->SetActive(false););

      if (!DeserializeComponents(endTime))
        return StepResult::Continue;
    }

    m_CurrentReader.SetStorage(nullptr);
    m_Phase = Phase::AddComponentsToBatch;
    BeginNextProgressStep("AddComponentsToBatch");
  }

  if (m_Phase == Phase::AddComponentsToBatch)
  {
    if (!AddComponentsToBatch(endTime))
      return StepResult::Continue;

    m_Phase = Phase::InitComponents;
    BeginNextProgressStep("InitComponents");
  }

  if (m_Phase == Phase::InitComponents)
  {
    if (!m_hComponentInitBatch.IsInvalidated())
    {
      double fCompletionFactor = 0.0;
      if (!m_WorldReader.m_pWorld->IsComponentInitBatchCompleted(m_hComponentInitBatch, &fCompletionFactor))
      {
        SetSubProgressCompletion(fCompletionFactor);
        return StepResult::ContinueNextFrame;
      }
    }

    m_Phase = Phase::Invalid;
    m_pSubProgressRange = nullptr;
    m_pOverallProgressRange = nullptr;
  }

  return StepResult::Finished;
}

void wdWorldReader::InstantiationContext::Cancel()
{
  if (!m_hComponentInitBatch.IsInvalidated())
  {
    m_WorldReader.m_pWorld->CancelComponentInitBatch(m_hComponentInitBatch);
  }

  m_Phase = Phase::Invalid;
  m_pSubProgressRange = nullptr;
  m_pOverallProgressRange = nullptr;
}

// a super simple, but also efficient random number generator
inline static wdUInt32 NextStableRandomSeed(wdUInt32& ref_uiSeed)
{
  ref_uiSeed = 214013L * ref_uiSeed + 2531011L;
  return ((ref_uiSeed >> 16) & 0x7FFFF);
}

template <bool UseTransform>
bool wdWorldReader::InstantiationContext::CreateGameObjects(const wdDynamicArray<GameObjectToCreate>& objects, wdGameObjectHandle hParent, wdDynamicArray<wdGameObject*>* out_pCreatedObjects, wdTime endTime)
{
  WD_PROFILE_SCOPE("wdWorldReader::CreateGameObjects");

  while (m_uiCurrentIndex < objects.GetCount())
  {
    auto& godesc = objects[m_uiCurrentIndex];

    wdGameObjectDesc desc = godesc.m_Desc; // make a copy
    desc.m_hParent = hParent.IsInvalidated() ? m_WorldReader.m_IndexToGameObjectHandle[godesc.m_uiParentHandleIdx] : hParent;
    desc.m_bDynamic |= m_Options.m_bForceDynamic;

    switch (m_Options.m_RandomSeedMode)
    {
      case wdPrefabInstantiationOptions::RandomSeedMode::DeterministicFromParent:
        desc.m_uiStableRandomSeed = 0xFFFFFFFF; // wdWorld::CreateObject() will either derive a deterministic value from the parent object, or assign a random value, if no parent exists
        break;

      case wdPrefabInstantiationOptions::RandomSeedMode::CompletelyRandom:
        desc.m_uiStableRandomSeed = 0; // wdWorld::CreateObject() will assign a random value to this object
        break;

      case wdPrefabInstantiationOptions::RandomSeedMode::FixedFromSerialization:
        // keep deserialized value
        break;

      case wdPrefabInstantiationOptions::RandomSeedMode::CustomRootValue:
        // we use the given seed root value to assign a deterministic (but different) value to each game object
        desc.m_uiStableRandomSeed = NextStableRandomSeed(m_Options.m_uiCustomRandomSeedRootValue);
        break;
    }

    if (m_Options.m_pOverrideTeamID != nullptr)
    {
      desc.m_uiTeamID = *m_Options.m_pOverrideTeamID;
    }

    if (UseTransform)
    {
      wdTransform tChild(desc.m_LocalPosition, desc.m_LocalRotation, desc.m_LocalScaling);
      wdTransform tFinal;
      tFinal.SetGlobalTransform(m_RootTransform, tChild);

      desc.m_LocalPosition = tFinal.m_vPosition;
      desc.m_LocalRotation = tFinal.m_qRotation;
      desc.m_LocalScaling = tFinal.m_vScale;
    }

    wdGameObject* pObject = nullptr;
    m_WorldReader.m_IndexToGameObjectHandle.PushBack(m_WorldReader.m_pWorld->CreateObject(desc, pObject));

    if (!godesc.m_sGlobalKey.IsEmpty())
    {
      pObject->SetGlobalKey(godesc.m_sGlobalKey);
    }

    if (out_pCreatedObjects)
    {
      out_pCreatedObjects->PushBack(pObject);
    }

    ++m_uiCurrentIndex;

    // exit here to ensure that we at least did some work
    if (wdTime::Now() >= endTime)
    {
      SetSubProgressCompletion(static_cast<double>(m_uiCurrentIndex) / objects.GetCount());
      return false;
    }
  }

  m_uiCurrentIndex = 0;

  return true;
}

bool wdWorldReader::InstantiationContext::CreateComponents(wdTime endTime)
{
  WD_PROFILE_SCOPE("wdWorldReader::CreateComponents");

  wdStreamReader& s = *m_WorldReader.m_pStream;

  for (; m_uiCurrentComponentTypeIndex < m_WorldReader.m_ComponentTypes.GetCount(); ++m_uiCurrentComponentTypeIndex)
  {
    auto& compTypeInfo = m_WorldReader.m_ComponentTypes[m_uiCurrentComponentTypeIndex];

    // will be the case for all abstract component types
    if (compTypeInfo.m_pRtti == nullptr || compTypeInfo.m_uiNumComponents == 0)
      continue;

    wdComponentManagerBase* pManager = m_WorldReader.m_pWorld->GetOrCreateManagerForComponentType(compTypeInfo.m_pRtti);
    WD_ASSERT_DEV(pManager != nullptr, "Cannot create components of type '{0}', manager is not available.", compTypeInfo.m_pRtti->GetTypeName());

    while (m_uiCurrentIndex < compTypeInfo.m_uiNumComponents)
    {
      const wdGameObjectHandle hOwner = m_WorldReader.ReadGameObjectHandle();

      wdUInt32 uiComponentIdx = 0;
      s >> uiComponentIdx;

      bool bActive = true;
      s >> bActive;

      wdUInt8 userFlags = 0;
      s >> userFlags;

      wdGameObject* pOwnerObject = nullptr;
      if (!m_WorldReader.m_pWorld->TryGetObject(hOwner, pOwnerObject))
      {
        WD_REPORT_FAILURE("Owner object must be not null");
      }

      wdComponent* pComponent = nullptr;
      auto hComponent = pManager->CreateComponentNoInit(pOwnerObject, pComponent);

      pComponent->SetActiveFlag(bActive);

      for (wdUInt8 j = 0; j < 8; ++j)
      {
        pComponent->SetUserFlag(j, (userFlags & WD_BIT(j)) != 0);
      }

      WD_ASSERT_DEBUG(uiComponentIdx == compTypeInfo.m_ComponentIndexToHandle.GetCount(), "Component index doesn't match");
      compTypeInfo.m_ComponentIndexToHandle.PushBack(hComponent);

      ++m_uiCurrentIndex;
      ++m_uiCurrentNumComponentsProcessed;

      // exit here to ensure that we at least did some work
      if (wdTime::Now() >= endTime)
      {
        SetSubProgressCompletion((double)m_uiCurrentNumComponentsProcessed / m_WorldReader.m_uiTotalNumComponents);
        return false;
      }
    }

    m_uiCurrentIndex = 0;
  }

  m_uiCurrentIndex = 0;
  m_uiCurrentComponentTypeIndex = 0;
  m_uiCurrentNumComponentsProcessed = 0;

  return true;
}

bool wdWorldReader::InstantiationContext::DeserializeComponents(wdTime endTime)
{
  WD_PROFILE_SCOPE("wdWorldReader::DeserializeComponents");

  wdStreamReader& s = *m_WorldReader.m_pStream;

  for (; m_uiCurrentComponentTypeIndex < m_WorldReader.m_ComponentTypes.GetCount(); ++m_uiCurrentComponentTypeIndex)
  {
    auto& compTypeInfo = m_WorldReader.m_ComponentTypes[m_uiCurrentComponentTypeIndex];
    if (compTypeInfo.m_pRtti == nullptr)
      continue;

    while (m_uiCurrentIndex < compTypeInfo.m_ComponentIndexToHandle.GetCount())
    {
      wdComponent* pComponent = nullptr;
      if (m_WorldReader.m_pWorld->TryGetComponent(compTypeInfo.m_ComponentIndexToHandle[m_uiCurrentIndex++], pComponent))
      {
        pComponent->DeserializeComponent(m_WorldReader);

        ++m_uiCurrentNumComponentsProcessed;

        // exit here to ensure that we at least did some work
        if (wdTime::Now() >= endTime)
        {
          SetSubProgressCompletion((double)m_uiCurrentNumComponentsProcessed / m_WorldReader.m_uiTotalNumComponents);
          return false;
        }
      }
    }

    m_uiCurrentIndex = 0;
  }

  m_uiCurrentIndex = 0;
  m_uiCurrentComponentTypeIndex = 0;
  m_uiCurrentNumComponentsProcessed = 0;

  return true;
}

bool wdWorldReader::InstantiationContext::AddComponentsToBatch(wdTime endTime)
{
  WD_PROFILE_SCOPE("wdWorldReader::AddComponentsToBatch");

  wdUInt32 uiInitializedComponents = 0;

  if (!m_hComponentInitBatch.IsInvalidated())
  {
    m_WorldReader.m_pWorld->BeginAddingComponentsToInitBatch(m_hComponentInitBatch);
  }

  for (; m_uiCurrentComponentTypeIndex < m_WorldReader.m_ComponentTypes.GetCount(); ++m_uiCurrentComponentTypeIndex)
  {
    auto& compTypeInfo = m_WorldReader.m_ComponentTypes[m_uiCurrentComponentTypeIndex];
    if (compTypeInfo.m_pRtti == nullptr)
      continue;

    while (m_uiCurrentIndex < compTypeInfo.m_ComponentIndexToHandle.GetCount())
    {
      wdComponent* pComponent = nullptr;
      if (m_WorldReader.m_pWorld->TryGetComponent(compTypeInfo.m_ComponentIndexToHandle[m_uiCurrentIndex++], pComponent))
      {
        pComponent->GetOwningManager()->InitializeComponent(pComponent);
        ++uiInitializedComponents;

        ++m_uiCurrentNumComponentsProcessed;

        // exit here to ensure that we at least did some work
        if (wdTime::Now() >= endTime)
        {
          SetSubProgressCompletion((double)m_uiCurrentNumComponentsProcessed / m_WorldReader.m_uiTotalNumComponents);

          if (!m_hComponentInitBatch.IsInvalidated())
          {
            m_WorldReader.m_pWorld->EndAddingComponentsToInitBatch(m_hComponentInitBatch);
          }
          return false;
        }
      }
    }

    m_uiCurrentIndex = 0;
  }

  if (!m_hComponentInitBatch.IsInvalidated())
  {
    m_WorldReader.m_pWorld->SubmitComponentInitBatch(m_hComponentInitBatch);
  }

  m_uiCurrentIndex = 0;
  m_uiCurrentComponentTypeIndex = 0;
  m_uiCurrentNumComponentsProcessed = 0;

  return true;
}

void wdWorldReader::InstantiationContext::SetMaxStepTime(wdTime stepTime)
{
  m_Options.m_MaxStepTime = stepTime;
}

wdTime wdWorldReader::InstantiationContext::GetMaxStepTime() const
{
  return m_Options.m_MaxStepTime;
}

void wdWorldReader::InstantiationContext::BeginNextProgressStep(wdStringView sName)
{
  if (m_pOverallProgressRange != nullptr)
  {
    m_pOverallProgressRange->BeginNextStep(sName);
    m_pSubProgressRange = nullptr;
    m_pSubProgressRange = WD_DEFAULT_NEW(wdProgressRange, sName, false, m_pOverallProgressRange->GetProgressbar());
  }
}

void wdWorldReader::InstantiationContext::SetSubProgressCompletion(double fCompletion)
{
  if (m_pSubProgressRange != nullptr)
  {
    m_pSubProgressRange->SetCompletion(fCompletion);
  }
}

WD_STATICLINK_FILE(Core, Core_WorldSerializer_Implementation_WorldReader);
