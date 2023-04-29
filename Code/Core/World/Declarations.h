#pragma once

#include <Foundation/Memory/BlockStorage.h>
#include <Foundation/Memory/LargeBlockAllocator.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/Id.h>

#include <Core/CoreDLL.h>

#ifndef WD_WORLD_INDEX_BITS
#  define WD_WORLD_INDEX_BITS 8
#endif

#define WD_MAX_WORLDS (1 << WD_WORLD_INDEX_BITS)

class wdWorld;
class wdSpatialSystem;
class wdCoordinateSystemProvider;

namespace wdInternal
{
  class WorldData;

  enum
  {
    DEFAULT_BLOCK_SIZE = 1024 * 16
  };

  typedef wdLargeBlockAllocator<DEFAULT_BLOCK_SIZE> WorldLargeBlockAllocator;
} // namespace wdInternal

class wdGameObject;
struct wdGameObjectDesc;

class wdComponentManagerBase;
class wdComponent;

struct wdMsgDeleteGameObject;

/// \brief Internal game object id used by wdGameObjectHandle.
struct wdGameObjectId
{
  typedef wdUInt64 StorageType;

  WD_DECLARE_ID_TYPE(wdGameObjectId, 32, 8);

  static_assert(WD_WORLD_INDEX_BITS > 0 && WD_WORLD_INDEX_BITS <= 24);

  WD_FORCE_INLINE wdGameObjectId(StorageType instanceIndex, wdUInt8 uiGeneration, wdUInt8 uiWorldIndex = 0)
  {
    m_Data = 0;
    m_InstanceIndex = static_cast<wdUInt32>(instanceIndex);
    m_Generation = uiGeneration;
    m_WorldIndex = uiWorldIndex;
  }

  union
  {
    StorageType m_Data;
    struct
    {
      StorageType m_InstanceIndex : 32;
      StorageType m_Generation : 8;
      StorageType m_WorldIndex : WD_WORLD_INDEX_BITS;
    };
  };
};

/// \brief A handle to a game object.
///
/// Never store a direct pointer to a game object. Always store a handle instead. A pointer to a game object can
/// be received by calling wdWorld::TryGetObject with the handle.
/// Note that the object might have been deleted so always check the return value of TryGetObject.
struct wdGameObjectHandle
{
  WD_DECLARE_HANDLE_TYPE(wdGameObjectHandle, wdGameObjectId);

  friend class wdWorld;
  friend class wdGameObject;
};

/// \brief HashHelper implementation so game object handles can be used as key in a hash table.
template <>
struct wdHashHelper<wdGameObjectHandle>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(wdGameObjectHandle value) { return wdHashHelper<wdUInt64>::Hash(value.GetInternalID().m_Data); }

  WD_ALWAYS_INLINE static bool Equal(wdGameObjectHandle a, wdGameObjectHandle b) { return a == b; }
};

/// \brief Currently not implemented as it is not needed for game object handles.
WD_CORE_DLL void operator<<(wdStreamWriter& inout_stream, const wdGameObjectHandle& hValue);
WD_CORE_DLL void operator>>(wdStreamReader& inout_stream, wdGameObjectHandle& ref_hValue);

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdGameObjectHandle);
WD_DECLARE_CUSTOM_VARIANT_TYPE(wdGameObjectHandle);
#define WD_COMPONENT_TYPE_INDEX_BITS (24 - WD_WORLD_INDEX_BITS)
#define WD_MAX_COMPONENT_TYPES (1 << WD_COMPONENT_TYPE_INDEX_BITS)

/// \brief Internal component id used by wdComponentHandle.
struct wdComponentId
{
  typedef wdUInt64 StorageType;

  WD_DECLARE_ID_TYPE(wdComponentId, 32, 8);

  static_assert(WD_COMPONENT_TYPE_INDEX_BITS > 0 && WD_COMPONENT_TYPE_INDEX_BITS <= 16);

  WD_ALWAYS_INLINE wdComponentId(StorageType instanceIndex, wdUInt8 uiGeneration, wdUInt16 uiTypeId = 0, wdUInt8 uiWorldIndex = 0)
  {
    m_Data = 0;
    m_InstanceIndex = static_cast<wdUInt32>(instanceIndex);
    m_Generation = uiGeneration;
    m_TypeId = uiTypeId;
    m_WorldIndex = uiWorldIndex;
  }

  union
  {
    StorageType m_Data;
    struct
    {
      StorageType m_InstanceIndex : 32;
      StorageType m_Generation : 8;
      StorageType m_WorldIndex : WD_WORLD_INDEX_BITS;
      StorageType m_TypeId : WD_COMPONENT_TYPE_INDEX_BITS;
    };
  };
};

/// \brief A handle to a component.
///
/// Never store a direct pointer to a component. Always store a handle instead. A pointer to a component can
/// be received by calling wdWorld::TryGetComponent or TryGetComponent on the corresponding component manager.
/// Note that the component might have been deleted so always check the return value of TryGetComponent.
struct wdComponentHandle
{
  WD_DECLARE_HANDLE_TYPE(wdComponentHandle, wdComponentId);

  friend class wdWorld;
  friend class wdComponentManagerBase;
  friend class wdComponent;
};

/// \brief HashHelper implementation so component handles can be used as key in a hashtable.
template <>
struct wdHashHelper<wdComponentHandle>
{
  WD_ALWAYS_INLINE static wdUInt32 Hash(wdComponentHandle value)
  {
    wdComponentId id = value.GetInternalID();
    wdUInt64 data = *reinterpret_cast<wdUInt64*>(&id);
    return wdHashHelper<wdUInt64>::Hash(data);
  }

  WD_ALWAYS_INLINE static bool Equal(wdComponentHandle a, wdComponentHandle b) { return a == b; }
};

/// \brief Currently not implemented as it is not needed for component handles.
WD_CORE_DLL void operator<<(wdStreamWriter& inout_stream, const wdComponentHandle& hValue);
WD_CORE_DLL void operator>>(wdStreamReader& inout_stream, wdComponentHandle& ref_hValue);

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdComponentHandle);
WD_DECLARE_CUSTOM_VARIANT_TYPE(wdComponentHandle);

/// \brief Internal flags of game objects or components.
struct wdObjectFlags
{
  typedef wdUInt32 StorageType;

  enum Enum
  {
    None = 0,
    Dynamic = WD_BIT(0),                 ///< Usually detected automatically. A dynamic object will not cache render data across frames.
    ForceDynamic = WD_BIT(1),            ///< Set by the user to enforce the 'Dynamic' mode. Necessary when user code (or scripts) should change
                                         ///< objects, and the automatic detection cannot know that.
    ActiveFlag = WD_BIT(2),              ///< The object/component has the 'active flag' set
    ActiveState = WD_BIT(3),             ///< The object/component and all its parents have the active flag
    Initialized = WD_BIT(4),             ///< The object/component has been initialized
    Initializing = WD_BIT(5),            ///< The object/component is currently initializing. Used to prevent recursions during initialization.
    SimulationStarted = WD_BIT(6),       ///< OnSimulationStarted() has been called on the component
    SimulationStarting = WD_BIT(7),      ///< Used to prevent recursion during OnSimulationStarted()
    UnhandledMessageHandler = WD_BIT(8), ///< For components, when a message is not handled, a virtual function is called

    ChildChangesNotifications = WD_BIT(9),            ///< The object should send a notification message when children are added or removed.
    ComponentChangesNotifications = WD_BIT(10),       ///< The object should send a notification message when components are added or removed.
    StaticTransformChangesNotifications = WD_BIT(11), ///< The object should send a notification message if it is static and its transform changes.
    ParentChangesNotifications = WD_BIT(12),          ///< The object should send a notification message when the parent is changes.

    UserFlag0 = WD_BIT(24),
    UserFlag1 = WD_BIT(25),
    UserFlag2 = WD_BIT(26),
    UserFlag3 = WD_BIT(27),
    UserFlag4 = WD_BIT(28),
    UserFlag5 = WD_BIT(29),
    UserFlag6 = WD_BIT(30),
    UserFlag7 = WD_BIT(31),

    Default = None
  };

  struct Bits
  {
    StorageType Dynamic : 1;                             //< 0
    StorageType ForceDynamic : 1;                        //< 1
    StorageType ActiveFlag : 1;                          //< 2
    StorageType ActiveState : 1;                         //< 3
    StorageType Initialized : 1;                         //< 4
    StorageType Initializing : 1;                        //< 5
    StorageType SimulationStarted : 1;                   //< 6
    StorageType SimulationStarting : 1;                  //< 7
    StorageType UnhandledMessageHandler : 1;             //< 8
    StorageType ChildChangesNotifications : 1;           //< 9
    StorageType ComponentChangesNotifications : 1;       //< 10
    StorageType StaticTransformChangesNotifications : 1; //< 11
    StorageType ParentChangesNotifications : 1;          //< 12

    StorageType Padding : 11; // 13 - 23

    StorageType UserFlag0 : 1; //< 24
    StorageType UserFlag1 : 1; //< 25
    StorageType UserFlag2 : 1; //< 26
    StorageType UserFlag3 : 1; //< 27
    StorageType UserFlag4 : 1; //< 28
    StorageType UserFlag5 : 1; //< 29
    StorageType UserFlag6 : 1; //< 30
    StorageType UserFlag7 : 1; //< 31
  };
};

WD_DECLARE_FLAGS_OPERATORS(wdObjectFlags);

/// \brief Specifies the mode of an object. This enum is only used in the editor.
///
/// \sa wdObjectFlags
struct wdObjectMode
{
  typedef wdUInt8 StorageType;

  enum Enum : wdUInt8
  {
    Automatic,
    ForceDynamic,

    Default = Automatic
  };
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdObjectMode);

/// \brief Specifies the mode of a component. Dynamic components may change an object's transform, static components must not.
///
/// \sa wdObjectFlags
struct wdComponentMode
{
  enum Enum
  {
    Static,
    Dynamic
  };
};

/// \brief Specifies at which phase the queued message should be processed.
struct wdObjectMsgQueueType
{
  enum Enum
  {
    PostAsync,        ///< Process the message in the PostAsync phase.
    PostTransform,    ///< Process the message in the PostTransform phase.
    NextFrame,        ///< Process the message in the PreAsync phase of the next frame.
    AfterInitialized, ///< Process the message after new components have been initialized.
    COUNT
  };
};

/// \brief Certain components may delete themselves or their owner when they are finished with their main purpose
struct WD_CORE_DLL wdOnComponentFinishedAction
{
  using StorageType = wdUInt8;

  enum Enum : StorageType
  {
    None,
    DeleteComponent,
    DeleteGameObject,

    Default = None
  };

  /// \brief Call this when a component is 'finished' with its work.
  ///
  /// Pass in the desired action (usually configured by the user) and the 'this' pointer of the component.
  /// The helper function will delete this component and maybe also attempt to delete the entire object.
  /// For that it will coordinate with other components, and delay the object deletion, if necessary,
  /// until the last component has finished it's work.
  static void HandleFinishedAction(wdComponent* pComponent, wdOnComponentFinishedAction::Enum action);

  /// \brief Call this function in a message handler for wdMsgDeleteGameObject messages.
  ///
  /// This is needed to coordinate object deletion across multiple components that use the
  /// wdOnComponentFinishedAction mechanism.
  /// Depending on the state of this component, the function will either execute the object deletion,
  /// or delay it, until its own work is done.
  static void HandleDeleteObjectMsg(wdMsgDeleteGameObject& ref_msg, wdEnum<wdOnComponentFinishedAction>& ref_action);
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdOnComponentFinishedAction);

/// \brief Same as wdOnComponentFinishedAction, but additionally includes 'Restart'
struct WD_CORE_DLL wdOnComponentFinishedAction2
{
  typedef wdUInt8 StorageType;

  enum Enum
  {
    None,
    DeleteComponent,
    DeleteGameObject,
    Restart,

    Default = None
  };

  /// \brief See wdOnComponentFinishedAction::HandleFinishedAction()
  static void HandleFinishedAction(wdComponent* pComponent, wdOnComponentFinishedAction2::Enum action);

  /// \brief See wdOnComponentFinishedAction::HandleDeleteObjectMsg()
  static void HandleDeleteObjectMsg(wdMsgDeleteGameObject& ref_msg, wdEnum<wdOnComponentFinishedAction2>& ref_action);
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdOnComponentFinishedAction2);

/// \brief Used as return value of visitor functions to define whether calling function should stop or continue visiting.
struct wdVisitorExecution
{
  enum Enum
  {
    Continue, ///< Continue regular iteration
    Skip,     ///< In a depth-first iteration mode this will skip the entire sub-tree below the current object
    Stop      ///< Stop the entire iteration
  };
};

typedef wdGenericId<24, 8> wdSpatialDataId;
class wdSpatialDataHandle
{
  WD_DECLARE_HANDLE_TYPE(wdSpatialDataHandle, wdSpatialDataId);
};

#define WD_MAX_WORLD_MODULE_TYPES WD_MAX_COMPONENT_TYPES
typedef wdUInt16 wdWorldModuleTypeId;
static_assert(wdMath::MaxValue<wdWorldModuleTypeId>() >= WD_MAX_WORLD_MODULE_TYPES - 1);

typedef wdGenericId<24, 8> wdComponentInitBatchId;
class wdComponentInitBatchHandle
{
  WD_DECLARE_HANDLE_TYPE(wdComponentInitBatchHandle, wdComponentInitBatchId);
};
