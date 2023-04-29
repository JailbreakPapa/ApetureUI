#pragma once

#include <Core/World/World.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Types/UniquePtr.h>

class wdStringDeduplicationReadContext;
class wdProgress;
class wdProgressRange;

struct wdPrefabInstantiationOptions
{
  wdGameObjectHandle m_hParent;

  wdDynamicArray<wdGameObject*>* m_pCreatedRootObjectsOut = nullptr;
  wdDynamicArray<wdGameObject*>* m_pCreatedChildObjectsOut = nullptr;
  const wdUInt16* m_pOverrideTeamID = nullptr;

  bool m_bForceDynamic = false;

  enum class RandomSeedMode
  {
    DeterministicFromParent,
    CompletelyRandom,
    FixedFromSerialization,
    CustomRootValue,
  };

  RandomSeedMode m_RandomSeedMode = RandomSeedMode::DeterministicFromParent;
  wdUInt32 m_uiCustomRandomSeedRootValue = 0;

  wdTime m_MaxStepTime = wdTime::Zero();

  wdProgress* m_pProgress = nullptr;
};

/// \brief Reads a world description from a stream. Allows to instantiate that world multiple times
///        in different locations and different wdWorld's.
///
/// The reader will ignore unknown component types and skip them during instantiation.
class WD_CORE_DLL wdWorldReader
{
public:
  /// \brief A context object is returned from InstantiateWorld or InstantiatePrefab if a maxStepTime greater than zero is specified.
  ///
  /// Call the Step() function periodically to complete the instantiation.
  /// Each step will try to spend not more than the given maxStepTime.
  /// E.g. this is useful if the instantiation cost of large prefabs needs to be distributed over multiple frames.
  class InstantiationContextBase
  {
  public:
    enum class StepResult
    {
      Continue,          ///< The available time slice is used up. Call Step() again to continue the process.
      ContinueNextFrame, ///< The process has reached a point where you need to call wdWorld::Update(). Otherwise no further progress can be made.
      Finished,          ///< The instantiation is finished and you can delete the context. Don't call 'Step()' on it again.
    };

    virtual ~InstantiationContextBase() {}

    /// \Brief Advance the instantiation by one step
    /// \return Whether the operation is finished or needs to be repeated.
    virtual StepResult Step() = 0;

    /// \Brief Cancel the instantiation. This might lead to inconsistent states and must be used with care.
    virtual void Cancel() = 0;
  };

  wdWorldReader();
  ~wdWorldReader();

  /// \brief Reads all information about the world from the given stream.
  ///
  /// Call this once to populate wdWorldReader with information how to instantiate the world.
  /// Afterwards \a stream can be deleted.
  /// Call InstantiateWorld() or InstantiatePrefab() afterwards as often as you like
  /// to actually get an objects into an wdWorld.
  /// By default, the method will warn if it skips bytes in the stream that are of unknown
  /// types. The warnings can be suppressed by setting warningOnUnkownSkip to false.
  wdResult ReadWorldDescription(wdStreamReader& inout_stream, bool bWarningOnUnkownSkip = true);

  /// \brief Creates one instance of the world that was previously read by ReadWorldDescription().
  ///
  /// This is identical to calling InstantiatePrefab() with identity values, however, it is a bit
  /// more efficient, as unnecessary computations are skipped.
  ///
  /// If pOverrideTeamID is not null, every instantiated game object will get it passed in as its new value.
  /// This can be used to identify that the object belongs to a specific player or team.
  ///
  /// If maxStepTime is not zero the function will return a valid ptr to an InstantiationContextBase.
  /// This context will only spend the given amount of time in its Step() function.
  /// The function has to be periodically called until it returns true to complete the instantiation.
  ///
  /// If pProgress is a valid pointer it is used to track the progress of the instantiation. The wdProgress object
  /// has to be valid as long as the instantiation is in progress.
  wdUniquePtr<InstantiationContextBase> InstantiateWorld(wdWorld& ref_world, const wdUInt16* pOverrideTeamID = nullptr, wdTime maxStepTime = wdTime::Zero(), wdProgress* pProgress = nullptr);

  /// \brief Creates one instance of the world that was previously read by ReadWorldDescription().
  ///
  /// \param rootTransform is an additional transform that is applied to all root objects.
  /// \param hParent allows to attach the newly created objects immediately to a parent
  /// \param out_CreatedRootObjects If this is valid, all pointers the to created root objects are stored in this array
  ///
  /// If pOverrideTeamID is not null, every instantiated game object will get it passed in as its new value.
  /// This can be used to identify that the object belongs to a specific player or team.
  ///
  /// If maxStepTime is not zero the function will return a valid ptr to an InstantiationContextBase.
  /// This context will only spend the given amount of time in its Step() function.
  /// The function has to be periodically called until it returns true to complete the instantiation.
  ///
  /// If pProgress is a valid pointer it is used to track the progress of the instantiation. The wdProgress object
  /// has to be valid as long as the instantiation is in progress.
  wdUniquePtr<InstantiationContextBase> InstantiatePrefab(wdWorld& ref_world, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options);

  /// \brief Gives access to the stream of data. Use this inside component deserialization functions to read data.
  wdStreamReader& GetStream() const { return *m_pStream; }

  /// \brief Used during component deserialization to read a handle to a game object.
  wdGameObjectHandle ReadGameObjectHandle();

  /// \brief Used during component deserialization to read a handle to a component.
  void ReadComponentHandle(wdComponentHandle& out_hComponent);

  /// \brief Used during component deserialization to query the actual version number with which the
  /// given component type was written. The version number is given through the WD_BEGIN_COMPONENT_TYPE
  /// macro. Whenever the serialization of a component changes, that number should be increased.
  wdUInt32 GetComponentTypeVersion(const wdRTTI* pRtti) const;

  /// \brief Clears all data.
  void ClearAndCompact();

  /// \brief Returns the amount of bytes that are currently allocated on the heap.
  wdUInt64 GetHeapMemoryUsage() const;

  using FindComponentTypeCallback = wdDelegate<const wdRTTI*(const char* szTypeName)>;

  /// \brief An optional callback to redirect the lookup of a component type name to an wdRTTI type.
  ///
  /// If specified, this is used by ALL world readers. The intention is to use this either for logging purposes,
  /// or to implement a whitelist or blacklist for specific component types.
  /// E.g. if the callback returns nullptr, the component type is 'unknown' and skipped by the world reader.
  /// Thus one can remove unwanted component types.
  /// Theoretically one could also redirect an old (or renamed) component type to a new one,
  /// given that their deserialization code is compatible.
  static FindComponentTypeCallback s_FindComponentTypeCallback;

  wdUInt32 GetRootObjectCount() const;
  wdUInt32 GetChildObjectCount() const;

  static void SetMaxStepTime(InstantiationContextBase* pContext, wdTime maxStepTime);
  static wdTime GetMaxStepTime(InstantiationContextBase* pContext);

private:
  struct GameObjectToCreate
  {
    wdGameObjectDesc m_Desc;
    wdString m_sGlobalKey;
    wdUInt32 m_uiParentHandleIdx;
  };

  void ReadGameObjectDesc(GameObjectToCreate& godesc);
  void ReadComponentTypeInfo(wdUInt32 uiComponentTypeIdx);
  void ReadComponentDataToMemStream(bool warningOnUnknownSkip = true);
  void ClearHandles();
  wdUniquePtr<InstantiationContextBase> Instantiate(wdWorld& world, bool bUseTransform, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options);

  wdStreamReader* m_pStream = nullptr;
  wdWorld* m_pWorld = nullptr;

  wdUInt8 m_uiVersion = 0;
  wdDynamicArray<wdGameObjectHandle> m_IndexToGameObjectHandle;

  wdDynamicArray<GameObjectToCreate> m_RootObjectsToCreate;
  wdDynamicArray<GameObjectToCreate> m_ChildObjectsToCreate;

  struct ComponentTypeInfo
  {
    const wdRTTI* m_pRtti = nullptr;
    wdDynamicArray<wdComponentHandle> m_ComponentIndexToHandle;
    wdUInt32 m_uiNumComponents = 0;
  };

  wdDynamicArray<ComponentTypeInfo> m_ComponentTypes;
  wdHashTable<const wdRTTI*, wdUInt32> m_ComponentTypeVersions;
  wdDefaultMemoryStreamStorage m_ComponentCreationStream;
  wdDefaultMemoryStreamStorage m_ComponentDataStream;
  wdUInt64 m_uiTotalNumComponents = 0;

  wdUniquePtr<wdStringDeduplicationReadContext> m_pStringDedupReadContext;

  class InstantiationContext : public InstantiationContextBase
  {
  public:
    InstantiationContext(wdWorldReader& ref_worldReader, bool bUseTransform, const wdTransform& rootTransform, const wdPrefabInstantiationOptions& options);
    ~InstantiationContext();

    virtual StepResult Step() override;
    virtual void Cancel() override;

    template <bool UseTransform>
    bool CreateGameObjects(const wdDynamicArray<GameObjectToCreate>& objects, wdGameObjectHandle hParent, wdDynamicArray<wdGameObject*>* out_pCreatedObjects, wdTime endTime);

    bool CreateComponents(wdTime endTime);
    bool DeserializeComponents(wdTime endTime);
    bool AddComponentsToBatch(wdTime endTime);

    void SetMaxStepTime(wdTime stepTime);
    wdTime GetMaxStepTime() const;

  private:
    void BeginNextProgressStep(wdStringView sName);
    void SetSubProgressCompletion(double fCompletion);

    friend class wdWorldReader;
    wdWorldReader& m_WorldReader;

    bool m_bUseTransform = false;
    wdTransform m_RootTransform;

    wdPrefabInstantiationOptions m_Options;

    wdComponentInitBatchHandle m_hComponentInitBatch;

    // Current state
    struct Phase
    {
      enum Enum
      {
        Invalid = -1,
        CreateRootObjects,
        CreateChildObjects,
        CreateComponents,
        DeserializeComponents,
        AddComponentsToBatch,
        InitComponents,

        Count
      };
    };

    Phase::Enum m_Phase = Phase::Invalid;
    wdUInt32 m_uiCurrentIndex = 0; // object or component
    wdUInt32 m_uiCurrentComponentTypeIndex = 0;
    wdUInt64 m_uiCurrentNumComponentsProcessed = 0;
    wdMemoryStreamReader m_CurrentReader;

    wdUniquePtr<wdProgressRange> m_pOverallProgressRange;
    wdUniquePtr<wdProgressRange> m_pSubProgressRange;
  };
};
