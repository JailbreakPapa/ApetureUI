#pragma once

#include <Foundation/Communication/MessageQueue.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Math/Random.h>
#include <Foundation/Memory/FrameAllocator.h>
#include <Foundation/Threading/DelegateTask.h>
#include <Foundation/Time/Clock.h>

#include <Core/World/GameObject.h>
#include <Core/World/WorldDesc.h>
#include <Foundation/Types/SharedPtr.h>

namespace wdInternal
{
  class WD_CORE_DLL WorldData
  {
  private:
    friend class ::wdWorld;
    friend class ::wdComponentManagerBase;

    WorldData(wdWorldDesc& desc);
    ~WorldData();

    void Clear();

    wdHashedString m_sName;
    mutable wdProxyAllocator m_Allocator;
    wdLocalAllocatorWrapper m_AllocatorWrapper;
    wdInternal::WorldLargeBlockAllocator m_BlockAllocator;
    wdDoubleBufferedStackAllocator m_StackAllocator;

    enum
    {
      GAME_OBJECTS_PER_BLOCK = wdDataBlock<wdGameObject, wdInternal::DEFAULT_BLOCK_SIZE>::CAPACITY,
      TRANSFORMATION_DATA_PER_BLOCK = wdDataBlock<wdGameObject::TransformationData, wdInternal::DEFAULT_BLOCK_SIZE>::CAPACITY
    };

    // object storage
    typedef wdBlockStorage<wdGameObject, wdInternal::DEFAULT_BLOCK_SIZE, wdBlockStorageType::Compact> ObjectStorage;
    wdIdTable<wdGameObjectId, wdGameObject*, wdLocalAllocatorWrapper> m_Objects;
    ObjectStorage m_ObjectStorage;

    wdSet<wdGameObject*, wdCompareHelper<wdGameObject*>, wdLocalAllocatorWrapper> m_DeadObjects;
    wdEvent<const wdGameObject*> m_ObjectDeletionEvent;

  public:
    class WD_CORE_DLL ConstObjectIterator
    {
    public:
      const wdGameObject& operator*() const;
      const wdGameObject* operator->() const;

      operator const wdGameObject*() const;

      /// \brief Advances the iterator to the next object. The iterator will not be valid anymore, if the last object is reached.
      void Next();

      /// \brief Checks whether this iterator points to a valid object.
      bool IsValid() const;

      /// \brief Shorthand for 'Next'
      void operator++();

    private:
      friend class ::wdWorld;

      ConstObjectIterator(ObjectStorage::ConstIterator iterator);

      ObjectStorage::ConstIterator m_Iterator;
    };

    class WD_CORE_DLL ObjectIterator
    {
    public:
      wdGameObject& operator*();
      wdGameObject* operator->();

      operator wdGameObject*();

      /// \brief Advances the iterator to the next object. The iterator will not be valid anymore, if the last object is reached.
      void Next();

      /// \brief Checks whether this iterator points to a valid object.
      bool IsValid() const;

      /// \brief Shorthand for 'Next'
      void operator++();

    private:
      friend class ::wdWorld;

      ObjectIterator(ObjectStorage::Iterator iterator);

      ObjectStorage::Iterator m_Iterator;
    };

  private:
    // hierarchy structures
    struct Hierarchy
    {
      typedef wdDataBlock<wdGameObject::TransformationData, wdInternal::DEFAULT_BLOCK_SIZE> DataBlock;
      typedef wdDynamicArray<DataBlock> DataBlockArray;

      wdHybridArray<DataBlockArray*, 8, wdLocalAllocatorWrapper> m_Data;
    };

    struct HierarchyType
    {
      enum Enum
      {
        Static,
        Dynamic,
        COUNT
      };
    };

    Hierarchy m_Hierarchies[HierarchyType::COUNT];

    static HierarchyType::Enum GetHierarchyType(bool bDynamic);

    wdGameObject::TransformationData* CreateTransformationData(bool bDynamic, wdUInt32 uiHierarchyLevel);

    void DeleteTransformationData(bool bDynamic, wdUInt32 uiHierarchyLevel, wdGameObject::TransformationData* pData);

    template <typename VISITOR>
    static wdVisitorExecution::Enum TraverseHierarchyLevel(Hierarchy::DataBlockArray& blocks, void* pUserData = nullptr);
    template <typename VISITOR>
    wdVisitorExecution::Enum TraverseHierarchyLevelMultiThreaded(Hierarchy::DataBlockArray& blocks, void* pUserData = nullptr);

    typedef wdDelegate<wdVisitorExecution::Enum(wdGameObject*)> VisitorFunc;
    void TraverseBreadthFirst(VisitorFunc& func);
    void TraverseDepthFirst(VisitorFunc& func);
    static wdVisitorExecution::Enum TraverseObjectDepthFirst(wdGameObject* pObject, VisitorFunc& func);

    static void UpdateGlobalTransform(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds);
    static void UpdateGlobalTransformWithParent(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds);

    static void UpdateGlobalTransformAndSpatialData(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds, wdSpatialSystem& spatialSystem);
    static void UpdateGlobalTransformWithParentAndSpatialData(wdGameObject::TransformationData* pData, const wdSimdFloat& fInvDeltaSeconds, wdSpatialSystem& spatialSystem);

    void UpdateGlobalTransforms(float fInvDeltaSeconds);

    // game object lookups
    wdHashTable<wdUInt64, wdGameObjectId, wdHashHelper<wdUInt64>, wdLocalAllocatorWrapper> m_GlobalKeyToIdTable;
    wdHashTable<wdUInt64, wdHashedString, wdHashHelper<wdUInt64>, wdLocalAllocatorWrapper> m_IdToGlobalKeyTable;

    // modules
    wdDynamicArray<wdWorldModule*, wdLocalAllocatorWrapper> m_Modules;
    wdDynamicArray<wdWorldModule*, wdLocalAllocatorWrapper> m_ModulesToStartSimulation;

    // component management
    wdSet<wdComponent*, wdCompareHelper<wdComponent*>, wdLocalAllocatorWrapper> m_DeadComponents;

    struct InitBatch
    {
      InitBatch(wdAllocatorBase* pAllocator, wdStringView sName, bool bMustFinishWithinOneFrame);

      wdHashedString m_sName;
      bool m_bMustFinishWithinOneFrame = true;
      bool m_bIsReady = false;

      wdUInt32 m_uiNextComponentToInitialize = 0;
      wdUInt32 m_uiNextComponentToStartSimulation = 0;
      wdDynamicArray<wdComponentHandle> m_ComponentsToInitialize;
      wdDynamicArray<wdComponentHandle> m_ComponentsToStartSimulation;
    };

    wdTime m_MaxInitializationTimePerFrame;
    wdIdTable<wdComponentInitBatchId, wdUniquePtr<InitBatch>, wdLocalAllocatorWrapper> m_InitBatches;
    InitBatch* m_pDefaultInitBatch = nullptr;
    InitBatch* m_pCurrentInitBatch = nullptr;

    struct RegisteredUpdateFunction
    {
      wdWorldModule::UpdateFunction m_Function;
      wdHashedString m_sFunctionName;
      float m_fPriority;
      wdUInt16 m_uiGranularity;
      bool m_bOnlyUpdateWhenSimulating;

      void FillFromDesc(const wdWorldModule::UpdateFunctionDesc& desc);
      bool operator<(const RegisteredUpdateFunction& other) const;
    };

    struct UpdateTask final : public wdTask
    {
      virtual void Execute() override;

      wdWorldModule::UpdateFunction m_Function;
      wdUInt32 m_uiStartIndex;
      wdUInt32 m_uiCount;
    };

    wdDynamicArray<RegisteredUpdateFunction, wdLocalAllocatorWrapper> m_UpdateFunctions[wdWorldModule::UpdateFunctionDesc::Phase::COUNT];
    wdDynamicArray<wdWorldModule::UpdateFunctionDesc, wdLocalAllocatorWrapper> m_UpdateFunctionsToRegister;

    wdDynamicArray<wdSharedPtr<UpdateTask>, wdLocalAllocatorWrapper> m_UpdateTasks;

    wdUniquePtr<wdSpatialSystem> m_pSpatialSystem;
    wdSharedPtr<wdCoordinateSystemProvider> m_pCoordinateSystemProvider;
    wdUniquePtr<wdTimeStepSmoothing> m_pTimeStepSmoothing;

    wdClock m_Clock;
    wdRandom m_Random;

    struct QueuedMsgMetaData
    {
      WD_DECLARE_POD_TYPE();

      WD_ALWAYS_INLINE QueuedMsgMetaData()
        : m_uiReceiverData(0)
      {
      }

      union
      {
        struct
        {
          wdUInt64 m_uiReceiverObjectOrComponent : 62;
          wdUInt64 m_uiReceiverIsComponent : 1;
          wdUInt64 m_uiRecursive : 1;
        };

        wdUInt64 m_uiReceiverData;
      };

      wdTime m_Due;
    };

    typedef wdMessageQueue<QueuedMsgMetaData, wdLocalAllocatorWrapper> MessageQueue;
    mutable MessageQueue m_MessageQueues[wdObjectMsgQueueType::COUNT];
    mutable MessageQueue m_TimedMessageQueues[wdObjectMsgQueueType::COUNT];

    wdThreadID m_WriteThreadID;
    wdInt32 m_iWriteCounter;
    mutable wdAtomicInteger32 m_iReadCounter;

    bool m_bSimulateWorld;
    bool m_bReportErrorWhenStaticObjectMoves;

    /// \brief Maps some data (given as void*) to an wdGameObjectHandle. Only available in special situations (e.g. editor use cases).
    wdDelegate<wdGameObjectHandle(const void*, wdComponentHandle, const char*)> m_GameObjectReferenceResolver;

  public:
    class ReadMarker
    {
    public:
      void Lock();
      void Unlock();

    private:
      friend class ::wdInternal::WorldData;

      ReadMarker(const WorldData& data);
      const WorldData& m_Data;
    };

    class WriteMarker
    {
    public:
      void Lock();
      void Unlock();

    private:
      friend class ::wdInternal::WorldData;

      WriteMarker(WorldData& data);
      WorldData& m_Data;
    };

  private:
    mutable ReadMarker m_ReadMarker;
    WriteMarker m_WriteMarker;

    void* m_pUserData;
  };
} // namespace wdInternal

#include <Core/World/Implementation/WorldData_inl.h>
