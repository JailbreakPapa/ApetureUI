#include <Core/CorePCH.h>

#include <Core/World/SpatialSystem_RegularGrid.h>
#include <Core/World/World.h>

#include <Foundation/Time/DefaultTimeStepSmoothing.h>

namespace wdInternal
{
  class DefaultCoordinateSystemProvider : public wdCoordinateSystemProvider
  {
  public:
    DefaultCoordinateSystemProvider()
      : wdCoordinateSystemProvider(nullptr)
    {
    }

    virtual void GetCoordinateSystem(const wdVec3& vGlobalPosition, wdCoordinateSystem& out_coordinateSystem) const override
    {
      out_coordinateSystem.m_vForwardDir = wdVec3(1.0f, 0.0f, 0.0f);
      out_coordinateSystem.m_vRightDir = wdVec3(0.0f, 1.0f, 0.0f);
      out_coordinateSystem.m_vUpDir = wdVec3(0.0f, 0.0f, 1.0f);
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  void WorldData::UpdateTask::Execute()
  {
    wdWorldModule::UpdateContext context;
    context.m_uiFirstComponentIndex = m_uiStartIndex;
    context.m_uiComponentCount = m_uiCount;

    m_Function(context);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  WorldData::WorldData(wdWorldDesc& desc)
    : m_sName(desc.m_sName)
    , m_Allocator(desc.m_sName, wdFoundation::GetDefaultAllocator())
    , m_AllocatorWrapper(&m_Allocator)
    , m_BlockAllocator(desc.m_sName, &m_Allocator)
    , m_StackAllocator(desc.m_sName, wdFoundation::GetAlignedAllocator())
    , m_ObjectStorage(&m_BlockAllocator, &m_Allocator)
    , m_MaxInitializationTimePerFrame(desc.m_MaxComponentInitializationTimePerFrame)
    , m_Clock(desc.m_sName)
    , m_WriteThreadID((wdThreadID)0)
    , m_iWriteCounter(0)
    , m_bSimulateWorld(true)
    , m_bReportErrorWhenStaticObjectMoves(desc.m_bReportErrorWhenStaticObjectMoves)
    , m_ReadMarker(*this)
    , m_WriteMarker(*this)
    , m_pUserData(nullptr)
  {
    m_AllocatorWrapper.Reset();

    if (desc.m_uiRandomNumberGeneratorSeed == 0)
    {
      m_Random.InitializeFromCurrentTime();
    }
    else
    {
      m_Random.Initialize(desc.m_uiRandomNumberGeneratorSeed);
    }

    // insert dummy entry to save some checks
    m_Objects.Insert(nullptr);

#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
    WD_CHECK_AT_COMPILETIME(sizeof(wdGameObject::TransformationData) == 224);
#else
    WD_CHECK_AT_COMPILETIME(sizeof(wdGameObject::TransformationData) == 192);
#endif

    WD_CHECK_AT_COMPILETIME(sizeof(wdGameObject) == 128);
    WD_CHECK_AT_COMPILETIME(sizeof(QueuedMsgMetaData) == 16);
    WD_CHECK_AT_COMPILETIME(WD_COMPONENT_TYPE_INDEX_BITS <= sizeof(wdWorldModuleTypeId) * 8);

    auto pDefaultInitBatch = WD_NEW(&m_Allocator, InitBatch, &m_Allocator, "Default", true);
    pDefaultInitBatch->m_bIsReady = true;
    m_InitBatches.Insert(pDefaultInitBatch);
    m_pDefaultInitBatch = pDefaultInitBatch;
    m_pCurrentInitBatch = pDefaultInitBatch;

    m_pSpatialSystem = std::move(desc.m_pSpatialSystem);
    m_pCoordinateSystemProvider = desc.m_pCoordinateSystemProvider;

    if (m_pSpatialSystem == nullptr && desc.m_bAutoCreateSpatialSystem)
    {
      m_pSpatialSystem = WD_NEW(wdFoundation::GetAlignedAllocator(), wdSpatialSystem_RegularGrid);
    }

    if (m_pCoordinateSystemProvider == nullptr)
    {
      m_pCoordinateSystemProvider = WD_NEW(&m_Allocator, DefaultCoordinateSystemProvider);
    }

    if (m_pTimeStepSmoothing == nullptr)
    {
      m_pTimeStepSmoothing = WD_NEW(&m_Allocator, wdDefaultTimeStepSmoothing);
    }

    m_Clock.SetTimeStepSmoothing(m_pTimeStepSmoothing.Borrow());
  }

  WorldData::~WorldData() = default;

  void WorldData::Clear()
  {
    // allow reading and writing during destruction
    m_WriteThreadID = wdThreadUtils::GetCurrentThreadID();
    m_iReadCounter.Increment();

    // deactivate all objects and components before destroying them
    for (auto it = m_ObjectStorage.GetIterator(); it.IsValid(); it.Next())
    {
      it->SetActiveFlag(false);
    }

    // deinitialize all modules before we invalidate the world. Components can still access the world during deinitialization.
    for (wdWorldModule* pModule : m_Modules)
    {
      if (pModule != nullptr)
      {
        pModule->Deinitialize();
      }
    }

    // now delete all modules
    for (wdWorldModule* pModule : m_Modules)
    {
      if (pModule != nullptr)
      {
        WD_DELETE(&m_Allocator, pModule);
      }
    }
    m_Modules.Clear();

    // this deletes the wdGameObject instances
    m_ObjectStorage.Clear();

    // delete all transformation data
    for (wdUInt32 uiHierarchyIndex = 0; uiHierarchyIndex < HierarchyType::COUNT; ++uiHierarchyIndex)
    {
      Hierarchy& hierarchy = m_Hierarchies[uiHierarchyIndex];

      for (wdUInt32 i = hierarchy.m_Data.GetCount(); i-- > 0;)
      {
        Hierarchy::DataBlockArray* blocks = hierarchy.m_Data[i];
        for (wdUInt32 j = blocks->GetCount(); j-- > 0;)
        {
          m_BlockAllocator.DeallocateBlock((*blocks)[j]);
        }
        WD_DELETE(&m_Allocator, blocks);
      }

      hierarchy.m_Data.Clear();
    }

    // delete task storage
    m_UpdateTasks.Clear();

    // delete queued messages
    for (wdUInt32 i = 0; i < wdObjectMsgQueueType::COUNT; ++i)
    {
      {
        MessageQueue& queue = m_MessageQueues[i];

        // The messages in this queue are allocated through a frame allocator and thus mustn't (and don't need to be) deallocated
        queue.Clear();
      }

      {
        MessageQueue& queue = m_TimedMessageQueues[i];
        while (!queue.IsEmpty())
        {
          MessageQueue::Entry& entry = queue.Peek();
          WD_DELETE(&m_Allocator, entry.m_pMessage);

          queue.Dequeue();
        }
      }
    }
  }

  wdGameObject::TransformationData* WorldData::CreateTransformationData(bool bDynamic, wdUInt32 uiHierarchyLevel)
  {
    Hierarchy& hierarchy = m_Hierarchies[GetHierarchyType(bDynamic)];

    while (uiHierarchyLevel >= hierarchy.m_Data.GetCount())
    {
      hierarchy.m_Data.PushBack(WD_NEW(&m_Allocator, Hierarchy::DataBlockArray, &m_Allocator));
    }

    Hierarchy::DataBlockArray& blocks = *hierarchy.m_Data[uiHierarchyLevel];
    Hierarchy::DataBlock* pBlock = nullptr;

    if (!blocks.IsEmpty())
    {
      pBlock = &blocks.PeekBack();
    }

    if (pBlock == nullptr || pBlock->IsFull())
    {
      blocks.PushBack(m_BlockAllocator.AllocateBlock<wdGameObject::TransformationData>());
      pBlock = &blocks.PeekBack();
    }

    return pBlock->ReserveBack();
  }

  void WorldData::DeleteTransformationData(bool bDynamic, wdUInt32 uiHierarchyLevel, wdGameObject::TransformationData* pData)
  {
    Hierarchy& hierarchy = m_Hierarchies[GetHierarchyType(bDynamic)];
    Hierarchy::DataBlockArray& blocks = *hierarchy.m_Data[uiHierarchyLevel];

    Hierarchy::DataBlock& lastBlock = blocks.PeekBack();
    const wdGameObject::TransformationData* pLast = lastBlock.PopBack();

    if (pData != pLast)
    {
      wdMemoryUtils::Copy(pData, pLast, 1);
      pData->m_pObject->m_pTransformationData = pData;

      // fix parent transform data for children as well
      auto it = pData->m_pObject->GetChildren();
      while (it.IsValid())
      {
        auto pTransformData = it->m_pTransformationData;
        pTransformData->m_pParentData = pData;
        it.Next();
      }
    }

    if (lastBlock.IsEmpty())
    {
      m_BlockAllocator.DeallocateBlock(lastBlock);
      blocks.PopBack();
    }
  }

  void WorldData::TraverseBreadthFirst(VisitorFunc& func)
  {
    struct Helper
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData) { return (*static_cast<VisitorFunc*>(pUserData))(pData->m_pObject); }
    };

    const wdUInt32 uiMaxHierarchyLevel = wdMath::Max(m_Hierarchies[HierarchyType::Static].m_Data.GetCount(), m_Hierarchies[HierarchyType::Dynamic].m_Data.GetCount());

    for (wdUInt32 uiHierarchyLevel = 0; uiHierarchyLevel < uiMaxHierarchyLevel; ++uiHierarchyLevel)
    {
      for (wdUInt32 uiHierarchyIndex = 0; uiHierarchyIndex < HierarchyType::COUNT; ++uiHierarchyIndex)
      {
        Hierarchy& hierarchy = m_Hierarchies[uiHierarchyIndex];
        if (uiHierarchyLevel < hierarchy.m_Data.GetCount())
        {
          wdVisitorExecution::Enum execution = TraverseHierarchyLevel<Helper>(*hierarchy.m_Data[uiHierarchyLevel], &func);
          WD_ASSERT_DEV(execution != wdVisitorExecution::Skip, "Skip is not supported when using breadth first traversal");
          if (execution == wdVisitorExecution::Stop)
            return;
        }
      }
    }
  }

  void WorldData::TraverseDepthFirst(VisitorFunc& func)
  {
    struct Helper
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData) { return WorldData::TraverseObjectDepthFirst(pData->m_pObject, *static_cast<VisitorFunc*>(pUserData)); }
    };

    for (wdUInt32 uiHierarchyIndex = 0; uiHierarchyIndex < HierarchyType::COUNT; ++uiHierarchyIndex)
    {
      Hierarchy& hierarchy = m_Hierarchies[uiHierarchyIndex];
      if (!hierarchy.m_Data.IsEmpty())
      {
        if (TraverseHierarchyLevel<Helper>(*hierarchy.m_Data[0], &func) == wdVisitorExecution::Stop)
          return;
      }
    }
  }

  // static
  wdVisitorExecution::Enum WorldData::TraverseObjectDepthFirst(wdGameObject* pObject, VisitorFunc& func)
  {
    wdVisitorExecution::Enum execution = func(pObject);
    if (execution == wdVisitorExecution::Stop)
      return wdVisitorExecution::Stop;

    if (execution != wdVisitorExecution::Skip) // skip all children
    {
      for (auto it = pObject->GetChildren(); it.IsValid(); ++it)
      {
        if (TraverseObjectDepthFirst(it, func) == wdVisitorExecution::Stop)
          return wdVisitorExecution::Stop;
      }
    }

    return wdVisitorExecution::Continue;
  }

  void WorldData::UpdateGlobalTransforms(float fInvDeltaSeconds)
  {
    struct UserData
    {
      wdSimdFloat m_fInvDt;
      wdSpatialSystem* m_pSpatialSystem;
    };

    UserData userData;
    userData.m_fInvDt = fInvDeltaSeconds;
    userData.m_pSpatialSystem = m_pSpatialSystem.Borrow();

    struct RootLevel
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData)
      {
        WorldData::UpdateGlobalTransform(pData, static_cast<UserData*>(pUserData)->m_fInvDt);
        return wdVisitorExecution::Continue;
      }
    };

    struct WithParent
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData)
      {
        WorldData::UpdateGlobalTransformWithParent(pData, static_cast<UserData*>(pUserData)->m_fInvDt);
        return wdVisitorExecution::Continue;
      }
    };

    struct RootLevelWithSpatialData
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData)
      {
        WorldData::UpdateGlobalTransformAndSpatialData(pData, static_cast<UserData*>(pUserData)->m_fInvDt, *static_cast<UserData*>(pUserData)->m_pSpatialSystem);
        return wdVisitorExecution::Continue;
      }
    };

    struct WithParentWithSpatialData
    {
      WD_ALWAYS_INLINE static wdVisitorExecution::Enum Visit(wdGameObject::TransformationData* pData, void* pUserData)
      {
        WorldData::UpdateGlobalTransformWithParentAndSpatialData(pData, static_cast<UserData*>(pUserData)->m_fInvDt, *static_cast<UserData*>(pUserData)->m_pSpatialSystem);
        return wdVisitorExecution::Continue;
      }
    };

    Hierarchy& hierarchy = m_Hierarchies[HierarchyType::Dynamic];
    if (!hierarchy.m_Data.IsEmpty())
    {
      auto dataPtr = hierarchy.m_Data.GetData();

      // If we have no spatial system, we perform multi-threaded update as we do not
      // have to acquire a write lock in the process.
      if (m_pSpatialSystem == nullptr)
      {
        TraverseHierarchyLevelMultiThreaded<RootLevel>(*dataPtr[0], &userData);

        for (wdUInt32 i = 1; i < hierarchy.m_Data.GetCount(); ++i)
        {
          TraverseHierarchyLevelMultiThreaded<WithParent>(*dataPtr[i], &userData);
        }
      }
      else
      {
        TraverseHierarchyLevel<RootLevelWithSpatialData>(*dataPtr[0], &userData);

        for (wdUInt32 i = 1; i < hierarchy.m_Data.GetCount(); ++i)
        {
          TraverseHierarchyLevel<WithParentWithSpatialData>(*dataPtr[i], &userData);
        }
      }
    }
  }

} // namespace wdInternal


WD_STATICLINK_FILE(Core, Core_World_Implementation_WorldData);
