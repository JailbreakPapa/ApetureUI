
namespace wdInternal
{
  // static
  WD_ALWAYS_INLINE WorldData::HierarchyType::Enum WorldData::GetHierarchyType(bool bIsDynamic)
  {
    return bIsDynamic ? HierarchyType::Dynamic : HierarchyType::Static;
  }

  // static
  template <typename VISITOR>
  WD_FORCE_INLINE wdVisitorExecution::Enum WorldData::TraverseHierarchyLevel(Hierarchy::DataBlockArray& blocks, void* pUserData /* = nullptr*/)
  {
    for (WorldData::Hierarchy::DataBlock& block : blocks)
    {
      wdGameObject::TransformationData* pCurrentData = block.m_pData;
      wdGameObject::TransformationData* pEndData = block.m_pData + block.m_uiCount;

      while (pCurrentData < pEndData)
      {
        wdVisitorExecution::Enum execution = VISITOR::Visit(pCurrentData, pUserData);
        if (execution != wdVisitorExecution::Continue)
          return execution;

        ++pCurrentData;
      }
    }

    return wdVisitorExecution::Continue;
  }

  // static
  template <typename VISITOR>
  WD_FORCE_INLINE wdVisitorExecution::Enum WorldData::TraverseHierarchyLevelMultiThreaded(
    Hierarchy::DataBlockArray& blocks, void* pUserData /* = nullptr*/)
  {
    wdParallelForParams parallelForParams;
    parallelForParams.m_uiBinSize = 100;
    parallelForParams.m_uiMaxTasksPerThread = 2;
    parallelForParams.m_pTaskAllocator = m_StackAllocator.GetCurrentAllocator();

    wdTaskSystem::ParallelFor(
      blocks.GetArrayPtr(),
      [pUserData](wdArrayPtr<WorldData::Hierarchy::DataBlock> blocksSlice) {
        for (WorldData::Hierarchy::DataBlock& block : blocksSlice)
        {
          wdGameObject::TransformationData* pCurrentData = block.m_pData;
          wdGameObject::TransformationData* pEndData = block.m_pData + block.m_uiCount;

          while (pCurrentData < pEndData)
          {
            VISITOR::Visit(pCurrentData, pUserData);
            ++pCurrentData;
          }
        }
      },
      "World DataBlock Traversal Task", parallelForParams);

    return wdVisitorExecution::Continue;
  }

  // static
  WD_FORCE_INLINE void WorldData::UpdateGlobalTransform(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds)
  {
    pData->UpdateGlobalTransformWithoutParent();
    pData->UpdateVelocity(fInvDeltaSeconds);
    pData->UpdateGlobalBounds();
  }

  // static
  WD_FORCE_INLINE void WorldData::UpdateGlobalTransformWithParent(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds)
  {
    pData->UpdateGlobalTransformWithParent();
    pData->UpdateVelocity(fInvDeltaSeconds);
    pData->UpdateGlobalBounds();
  }

  // static
  WD_FORCE_INLINE void WorldData::UpdateGlobalTransformAndSpatialData(
    wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds, wdSpatialSystem& spatialSystem)
  {
    pData->UpdateGlobalTransformWithoutParent();
    pData->UpdateVelocity(fInvDeltaSeconds);
    pData->UpdateGlobalBoundsAndSpatialData(spatialSystem);
  }

  // static
  WD_FORCE_INLINE void WorldData::UpdateGlobalTransformWithParentAndSpatialData(
    wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds, wdSpatialSystem& spatialSystem)
  {
    pData->UpdateGlobalTransformWithParent();
    pData->UpdateVelocity(fInvDeltaSeconds);
    pData->UpdateGlobalBoundsAndSpatialData(spatialSystem);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_ALWAYS_INLINE const wdGameObject& WorldData::ConstObjectIterator::operator*() const { return *m_Iterator; }

  WD_ALWAYS_INLINE const wdGameObject* WorldData::ConstObjectIterator::operator->() const { return m_Iterator; }

  WD_ALWAYS_INLINE WorldData::ConstObjectIterator::operator const wdGameObject*() const { return m_Iterator; }

  WD_ALWAYS_INLINE void WorldData::ConstObjectIterator::Next()
  {
    m_Iterator.Next();

    while (m_Iterator.IsValid() && m_Iterator->GetHandle().IsInvalidated())
    {
      m_Iterator.Next();
    }
  }

  WD_ALWAYS_INLINE bool WorldData::ConstObjectIterator::IsValid() const { return m_Iterator.IsValid(); }

  WD_ALWAYS_INLINE void WorldData::ConstObjectIterator::operator++() { Next(); }

  WD_ALWAYS_INLINE WorldData::ConstObjectIterator::ConstObjectIterator(ObjectStorage::ConstIterator iterator)
    : m_Iterator(iterator)
  {
    while (m_Iterator.IsValid() && m_Iterator->GetHandle().IsInvalidated())
    {
      m_Iterator.Next();
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_ALWAYS_INLINE wdGameObject& WorldData::ObjectIterator::operator*() { return *m_Iterator; }

  WD_ALWAYS_INLINE wdGameObject* WorldData::ObjectIterator::operator->() { return m_Iterator; }

  WD_ALWAYS_INLINE WorldData::ObjectIterator::operator wdGameObject*() { return m_Iterator; }

  WD_ALWAYS_INLINE void WorldData::ObjectIterator::Next()
  {
    m_Iterator.Next();

    while (m_Iterator.IsValid() && m_Iterator->GetHandle().IsInvalidated())
    {
      m_Iterator.Next();
    }
  }

  WD_ALWAYS_INLINE bool WorldData::ObjectIterator::IsValid() const { return m_Iterator.IsValid(); }

  WD_ALWAYS_INLINE void WorldData::ObjectIterator::operator++() { Next(); }

  WD_ALWAYS_INLINE WorldData::ObjectIterator::ObjectIterator(ObjectStorage::Iterator iterator)
    : m_Iterator(iterator)
  {
    while (m_Iterator.IsValid() && m_Iterator->GetHandle().IsInvalidated())
    {
      m_Iterator.Next();
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_FORCE_INLINE WorldData::InitBatch::InitBatch(wdAllocatorBase* pAllocator, wdStringView sName, bool bMustFinishWithinOneFrame)
    : m_bMustFinishWithinOneFrame(bMustFinishWithinOneFrame)
    , m_ComponentsToInitialize(pAllocator)
    , m_ComponentsToStartSimulation(pAllocator)
  {
    m_sName.Assign(sName);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_FORCE_INLINE void WorldData::RegisteredUpdateFunction::FillFromDesc(const wdWorldModule::UpdateFunctionDesc& desc)
  {
    m_Function = desc.m_Function;
    m_sFunctionName = desc.m_sFunctionName;
    m_fPriority = desc.m_fPriority;
    m_uiGranularity = desc.m_uiGranularity;
    m_bOnlyUpdateWhenSimulating = desc.m_bOnlyUpdateWhenSimulating;
  }

  WD_FORCE_INLINE bool WorldData::RegisteredUpdateFunction::operator<(const RegisteredUpdateFunction& other) const
  {
    // higher priority comes first
    if (m_fPriority != other.m_fPriority)
      return m_fPriority > other.m_fPriority;

    // sort by function name to ensure determinism
    wdInt32 iNameComp = wdStringUtils::Compare(m_sFunctionName, other.m_sFunctionName);
    WD_ASSERT_DEV(iNameComp != 0, "An update function with the same name and same priority is already registered. This breaks determinism.");
    return iNameComp < 0;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_ALWAYS_INLINE WorldData::ReadMarker::ReadMarker(const WorldData& data)
    : m_Data(data)
  {
  }

  WD_FORCE_INLINE void WorldData::ReadMarker::Lock()
  {
    WD_ASSERT_DEV(m_Data.m_WriteThreadID == (wdThreadID)0 || m_Data.m_WriteThreadID == wdThreadUtils::GetCurrentThreadID(),
      "World '{0}' cannot be marked for reading because it is already marked for writing by another thread.", m_Data.m_sName);
    m_Data.m_iReadCounter.Increment();
  }

  WD_ALWAYS_INLINE void WorldData::ReadMarker::Unlock() { m_Data.m_iReadCounter.Decrement(); }

  ///////////////////////////////////////////////////////////////////////////////////////////////////

  WD_ALWAYS_INLINE WorldData::WriteMarker::WriteMarker(WorldData& data)
    : m_Data(data)
  {
  }

  WD_FORCE_INLINE void WorldData::WriteMarker::Lock()
  {
    // already locked by this thread?
    if (m_Data.m_WriteThreadID != wdThreadUtils::GetCurrentThreadID())
    {
      WD_ASSERT_DEV(m_Data.m_iReadCounter == 0, "World '{0}' cannot be marked for writing because it is already marked for reading.", m_Data.m_sName);
      WD_ASSERT_DEV(m_Data.m_WriteThreadID == (wdThreadID)0,
        "World '{0}' cannot be marked for writing because it is already marked for writing by another thread.", m_Data.m_sName);

      m_Data.m_WriteThreadID = wdThreadUtils::GetCurrentThreadID();
      m_Data.m_iReadCounter.Increment(); // allow reading as well
    }

    m_Data.m_iWriteCounter++;
  }

  WD_FORCE_INLINE void WorldData::WriteMarker::Unlock()
  {
    m_Data.m_iWriteCounter--;

    if (m_Data.m_iWriteCounter == 0)
    {
      m_Data.m_iReadCounter.Decrement();
      m_Data.m_WriteThreadID = (wdThreadID)0;
    }
  }
} // namespace wdInternal
