#pragma once

#include <Core/World/Declarations.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Strings/HashedString.h>

class wdWorld;

class WD_CORE_DLL wdWorldModule : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdWorldModule, wdReflectedClass);

protected:
  wdWorldModule(wdWorld* pWorld);
  virtual ~wdWorldModule();

public:
  /// \brief Returns the corresponding world to this module.
  wdWorld* GetWorld();

  /// \brief Returns the corresponding world to this module.
  const wdWorld* GetWorld() const;

  /// \brief Same as GetWorld()->GetIndex(). Needed to break circular include dependencies.
  wdUInt32 GetWorldIndex() const;

protected:
  friend class wdWorld;
  friend class wdInternal::WorldData;
  friend class wdMemoryUtils;

  struct UpdateContext
  {
    wdUInt32 m_uiFirstComponentIndex = 0;
    wdUInt32 m_uiComponentCount = 0;
  };

  /// \brief Update function delegate.
  typedef wdDelegate<void(const UpdateContext&)> UpdateFunction;

  /// \brief Description of an update function that can be registered at the world.
  struct UpdateFunctionDesc
  {
    struct Phase
    {
      typedef wdUInt8 StorageType;

      enum Enum
      {
        PreAsync,
        Async,
        PostAsync,
        PostTransform,
        COUNT,

        Default = PreAsync
      };
    };

    UpdateFunctionDesc(const UpdateFunction& function, wdStringView sFunctionName)
    {
      m_Function = function;
      m_sFunctionName.Assign(sFunctionName);
    }

    UpdateFunction m_Function;                    ///< Delegate to the actual update function.
    wdHashedString m_sFunctionName;               ///< Name of the function. Use the WD_CREATE_MODULE_UPDATE_FUNCTION_DESC macro to create a description
                                                  ///< with the correct name.
    wdHybridArray<wdHashedString, 4> m_DependsOn; ///< Array of other functions on which this function depends on. This function will be
                                                  ///< called after all its dependencies have been called.
    wdEnum<Phase> m_Phase;                        ///< The update phase in which this update function should be called. See wdWorld for a description on the
                                                  ///< different phases.
    bool m_bOnlyUpdateWhenSimulating = false;     ///< The update function is only called when the world simulation is enabled.
    wdUInt16 m_uiGranularity = 0;                 ///< The granularity in which batch updates should happen during the asynchronous phase. Has to be 0 for
                                                  ///< synchronous functions.
    float m_fPriority = 0.0f;                     ///< Higher priority (higher number) means that this function is called earlier than a function with lower priority.
  };

  /// \brief Registers the given update function at the world.
  void RegisterUpdateFunction(const UpdateFunctionDesc& desc);

  /// \brief De-registers the given update function from the world. Note that only the m_Function and the m_Phase of the description have to
  /// be valid for de-registration.
  void DeregisterUpdateFunction(const UpdateFunctionDesc& desc);

  /// \brief Returns the allocator used by the world.
  wdAllocatorBase* GetAllocator();

  /// \brief Returns the block allocator used by the world.
  wdInternal::WorldLargeBlockAllocator* GetBlockAllocator();

  /// \brief Returns whether the world simulation is enabled.
  bool GetWorldSimulationEnabled() const;

protected:
  /// \brief This method is called after the constructor. A derived type can override this method to do initialization work. Typically this
  /// is the method where updates function are registered.
  virtual void Initialize() {}

  /// \brief This method is called before the destructor. A derived type can override this method to do deinitialization work.
  virtual void Deinitialize() {}

  /// \brief This method is called at the start of the next world update when the world is simulated. This method will be called after the
  /// initialization method.
  virtual void OnSimulationStarted() {}

  /// \brief Called by wdWorld::Clear(). Can be used to clear cached data when a world is completely cleared of objects (but not deleted).
  virtual void WorldClear() {}

  wdWorld* m_pWorld;
};

//////////////////////////////////////////////////////////////////////////

/// \brief Helper class to get component type ids and create new instances of world modules from rtti.
class WD_CORE_DLL wdWorldModuleFactory
{
public:
  static wdWorldModuleFactory* GetInstance();

  template <typename ModuleType, typename RTTIType>
  wdWorldModuleTypeId RegisterWorldModule();

  /// \brief Returns the module type id to the given rtti module/component type.
  wdWorldModuleTypeId GetTypeId(const wdRTTI* pRtti);

  /// \brief Creates a new instance of the world module with the given type id and world.
  wdWorldModule* CreateWorldModule(wdUInt16 uiTypeId, wdWorld* pWorld);

  /// \brief Register explicit a mapping of a world module interface to a specific implementation.
  ///
  /// This is necessary if there are multiple implementations of the same interface.
  /// If there is only one implementation for an interface this implementation is registered automatically.
  void RegisterInterfaceImplementation(wdStringView sInterfaceName, wdStringView sImplementationName);

private:
  WD_MAKE_SUBSYSTEM_STARTUP_FRIEND(Core, WorldModuleFactory);

  typedef wdWorldModule* (*CreatorFunc)(wdAllocatorBase*, wdWorld*);

  wdWorldModuleFactory();
  wdWorldModuleTypeId RegisterWorldModule(const wdRTTI* pRtti, CreatorFunc creatorFunc);

  static void PluginEventHandler(const wdPluginEvent& EventData);
  void FillBaseTypeIds();
  void ClearUnloadedTypeToIDs();
  void AdjustBaseTypeId(const wdRTTI* pParentRtti, const wdRTTI* pRtti, wdUInt16 uiParentTypeId);

  wdHashTable<const wdRTTI*, wdWorldModuleTypeId> m_TypeToId;

  struct CreatorFuncContext
  {
    WD_DECLARE_POD_TYPE();

    CreatorFunc m_Func;
    const wdRTTI* m_pRtti;
  };

  wdDynamicArray<CreatorFuncContext> m_CreatorFuncs;

  wdHashTable<wdString, wdString> m_InterfaceImplementations;
};

/// \brief Add this macro to the declaration of your module type.
#define WD_DECLARE_WORLD_MODULE()                                           \
public:                                                                     \
  static WD_ALWAYS_INLINE wdWorldModuleTypeId TypeId() { return s_TypeId; } \
                                                                            \
private:                                                                    \
  static wdWorldModuleTypeId s_TypeId;

/// \brief Implements the given module type. Add this macro to a cpp outside of the type declaration.
#define WD_IMPLEMENT_WORLD_MODULE(moduleType) \
  wdWorldModuleTypeId moduleType::s_TypeId = wdWorldModuleFactory::GetInstance()->RegisterWorldModule<moduleType, moduleType>();

/// \brief Helper macro to create an update function description with proper name
#define WD_CREATE_MODULE_UPDATE_FUNCTION_DESC(func, instance) wdWorldModule::UpdateFunctionDesc(wdWorldModule::UpdateFunction(&func, instance), #func)

#include <Core/World/Implementation/WorldModule_inl.h>
