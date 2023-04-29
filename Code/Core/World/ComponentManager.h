#pragma once

#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Memory/BlockStorage.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/Delegate.h>

#include <Core/World/Component.h>
#include <Core/World/Declarations.h>
#include <Core/World/WorldModule.h>

/// \brief Base class for all component managers. Do not derive directly from this class, but derive from wdComponentManager instead.
///
/// Every component type has its corresponding manager type. The manager stores the components in memory blocks to minimize overhead
/// on creation and deletion of components. Each manager can also register update functions to update its components during
/// the different update phases of wdWorld.
/// Use wdWorld::CreateComponentManager to create an instance of a component manager within a specific world.
class WD_CORE_DLL wdComponentManagerBase : public wdWorldModule
{
  WD_ADD_DYNAMIC_REFLECTION(wdComponentManagerBase, wdWorldModule);

protected:
  wdComponentManagerBase(wdWorld* pWorld);
  virtual ~wdComponentManagerBase();

public:
  /// \brief Checks whether the given handle references a valid component.
  bool IsValidComponent(const wdComponentHandle& hComponent) const;

  /// \brief Returns if a component with the given handle exists and if so writes out the corresponding pointer to out_pComponent.
  bool TryGetComponent(const wdComponentHandle& hComponent, wdComponent*& out_pComponent);

  /// \brief Returns if a component with the given handle exists and if so writes out the corresponding pointer to out_pComponent.
  bool TryGetComponent(const wdComponentHandle& hComponent, const wdComponent*& out_pComponent) const;

  /// \brief Returns the number of components managed by this manager.
  wdUInt32 GetComponentCount() const;

  /// \brief Create a new component instance and returns a handle to it.
  wdComponentHandle CreateComponent(wdGameObject* pOwnerObject);

  /// \brief Create a new component instance and returns a handle to it.
  template <typename ComponentType>
  wdComponentHandle CreateComponent(wdGameObject* pOwnerObject, ComponentType*& out_pComponent);

  /// \brief Deletes the given component. Note that the component will be invalidated first and the actual deletion is postponed.
  void DeleteComponent(const wdComponentHandle& hComponent);

  /// \brief Deletes the given component. Note that the component will be invalidated first and the actual deletion is postponed.
  void DeleteComponent(wdComponent* pComponent);

  /// \brief Adds all components that this manager handles to the given array (array is not cleared).
  /// Prefer to use more efficient methods on derived classes, only use this if you need to go through a wdComponentManagerBase pointer.
  virtual void CollectAllComponents(wdDynamicArray<wdComponentHandle>& out_allComponents, bool bOnlyActive) = 0;

  /// \brief Adds all components that this manager handles to the given array (array is not cleared).
  /// Prefer to use more efficient methods on derived classes, only use this if you need to go through a wdComponentManagerBase pointer.
  virtual void CollectAllComponents(wdDynamicArray<wdComponent*>& out_allComponents, bool bOnlyActive) = 0;

protected:
  /// \cond
  // internal methods
  friend class wdWorld;
  friend class wdInternal::WorldData;

  virtual void Deinitialize() override;

protected:
  friend class wdWorldReader;

  wdComponentHandle CreateComponentNoInit(wdGameObject* pOwnerObject, wdComponent*& out_pComponent);
  void InitializeComponent(wdComponent* pComponent);
  void DeinitializeComponent(wdComponent* pComponent);
  void PatchIdTable(wdComponent* pComponent);

  virtual wdComponent* CreateComponentStorage() = 0;
  virtual void DeleteComponentStorage(wdComponent* pComponent, wdComponent*& out_pMovedComponent) = 0;

  /// \endcond

  wdIdTable<wdComponentId, wdComponent*> m_Components;
};

template <typename T, wdBlockStorageType::Enum StorageType>
class wdComponentManager : public wdComponentManagerBase
{
public:
  typedef T ComponentType;
  typedef wdComponentManagerBase SUPER;

  /// \brief Although the constructor is public always use wdWorld::CreateComponentManager to create an instance.
  wdComponentManager(wdWorld* pWorld);
  virtual ~wdComponentManager();

  /// \brief Returns if a component with the given handle exists and if so writes out the corresponding pointer to out_pComponent.
  bool TryGetComponent(const wdComponentHandle& hComponent, ComponentType*& out_pComponent);

  /// \brief Returns if a component with the given handle exists and if so writes out the corresponding pointer to out_pComponent.
  bool TryGetComponent(const wdComponentHandle& hComponent, const ComponentType*& out_pComponent) const;

  /// \brief Returns an iterator over all components.
  typename wdBlockStorage<ComponentType, wdInternal::DEFAULT_BLOCK_SIZE, StorageType>::Iterator GetComponents(wdUInt32 uiStartIndex = 0);

  /// \brief Returns an iterator over all components.
  typename wdBlockStorage<ComponentType, wdInternal::DEFAULT_BLOCK_SIZE, StorageType>::ConstIterator GetComponents(wdUInt32 uiStartIndex = 0) const;

  /// \brief Returns the type id corresponding to the component type managed by this manager.
  static wdWorldModuleTypeId TypeId();

  virtual void CollectAllComponents(wdDynamicArray<wdComponentHandle>& out_allComponents, bool bOnlyActive) override;
  virtual void CollectAllComponents(wdDynamicArray<wdComponent*>& out_allComponents, bool bOnlyActive) override;

protected:
  friend ComponentType;
  friend class wdComponentManagerFactory;

  virtual wdComponent* CreateComponentStorage() override;
  virtual void DeleteComponentStorage(wdComponent* pComponent, wdComponent*& out_pMovedComponent) override;

  void RegisterUpdateFunction(UpdateFunctionDesc& desc);

  wdBlockStorage<ComponentType, wdInternal::DEFAULT_BLOCK_SIZE, StorageType> m_ComponentStorage;
};


//////////////////////////////////////////////////////////////////////////

struct wdComponentUpdateType
{
  enum Enum
  {
    Always,
    WhenSimulating
  };
};

/// \brief Simple component manager implementation that calls an update method on all components every frame.
template <typename ComponentType, wdComponentUpdateType::Enum UpdateType, wdBlockStorageType::Enum StorageType = wdBlockStorageType::FreeList>
class wdComponentManagerSimple final : public wdComponentManager<ComponentType, StorageType>
{
public:
  wdComponentManagerSimple(wdWorld* pWorld);

  virtual void Initialize() override;

  /// \brief A simple update function that iterates over all components and calls Update() on every component
  void SimpleUpdate(const wdWorldModule::UpdateContext& context);

private:
  static void SimpleUpdateName(wdStringBuilder& out_sName);
};

//////////////////////////////////////////////////////////////////////////

#define WD_ADD_COMPONENT_FUNCTIONALITY(componentType, baseType, managerType)                        \
public:                                                                                             \
  typedef managerType ComponentManagerType;                                                         \
  virtual wdWorldModuleTypeId GetTypeId() const override { return s_TypeId; }                       \
  static WD_ALWAYS_INLINE wdWorldModuleTypeId TypeId() { return s_TypeId; }                         \
  virtual wdComponentMode::Enum GetMode() const override;                                           \
  static wdComponentHandle CreateComponent(wdGameObject* pOwnerObject, componentType*& pComponent); \
  static void DeleteComponent(componentType* pComponent);                                           \
  void DeleteComponent();                                                                           \
                                                                                                    \
private:                                                                                            \
  friend managerType;                                                                               \
  static wdWorldModuleTypeId s_TypeId

#define WD_ADD_ABSTRACT_COMPONENT_FUNCTIONALITY(componentType, baseType)                     \
public:                                                                                      \
  virtual wdWorldModuleTypeId GetTypeId() const override { return wdWorldModuleTypeId(-1); } \
  static WD_ALWAYS_INLINE wdWorldModuleTypeId TypeId() { return wdWorldModuleTypeId(-1); }

/// \brief Add this macro to a custom component type inside the type declaration.
#define WD_DECLARE_COMPONENT_TYPE(componentType, baseType, managerType) \
  WD_ADD_DYNAMIC_REFLECTION(componentType, baseType);                   \
  WD_ADD_COMPONENT_FUNCTIONALITY(componentType, baseType, managerType);

/// \brief Add this macro to a custom abstract component type inside the type declaration.
#define WD_DECLARE_ABSTRACT_COMPONENT_TYPE(componentType, baseType) \
  WD_ADD_DYNAMIC_REFLECTION(componentType, baseType);               \
  WD_ADD_ABSTRACT_COMPONENT_FUNCTIONALITY(componentType, baseType);


/// \brief Implements rtti and component specific functionality. Add this macro to a cpp file.
///
/// \see WD_BEGIN_DYNAMIC_REFLECTED_TYPE
#define WD_BEGIN_COMPONENT_TYPE(componentType, version, mode)                                                                                  \
  wdWorldModuleTypeId componentType::s_TypeId =                                                                                                \
    wdWorldModuleFactory::GetInstance()->RegisterWorldModule<typename componentType::ComponentManagerType, componentType>();                   \
  wdComponentMode::Enum componentType::GetMode() const { return mode; }                                                                        \
  wdComponentHandle componentType::CreateComponent(wdGameObject* pOwnerObject, componentType*& out_pComponent)                                 \
  {                                                                                                                                            \
    return pOwnerObject->GetWorld()->GetOrCreateComponentManager<ComponentManagerType>()->CreateComponent(pOwnerObject, out_pComponent);       \
  }                                                                                                                                            \
  void componentType::DeleteComponent(componentType* pComponent) { pComponent->GetOwningManager()->DeleteComponent(pComponent->GetHandle()); } \
  void componentType::DeleteComponent() { GetOwningManager()->DeleteComponent(GetHandle()); }                                                  \
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(componentType, version, wdRTTINoAllocator)

/// \brief Implements rtti and abstract component specific functionality. Add this macro to a cpp file.
///
/// \see WD_BEGIN_DYNAMIC_REFLECTED_TYPE
#define WD_BEGIN_ABSTRACT_COMPONENT_TYPE(componentType, version)             \
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(componentType, version, wdRTTINoAllocator) \
    flags.Add(wdTypeFlags::Abstract);

/// \brief Ends the component implementation code block that was opened with WD_BEGIN_COMPONENT_TYPE.
#define WD_END_COMPONENT_TYPE WD_END_DYNAMIC_REFLECTED_TYPE
#define WD_END_ABSTRACT_COMPONENT_TYPE WD_END_DYNAMIC_REFLECTED_TYPE

#include <Core/World/Implementation/ComponentManager_inl.h>
