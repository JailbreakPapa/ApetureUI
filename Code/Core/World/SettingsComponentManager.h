#pragma once

#include <Core/World/ComponentManager.h>
#include <Foundation/Types/UniquePtr.h>

/// \brief A component manager that does no update at all on components and expects only a single instance to be created per world.
///
/// Easy access to this single component is provided through the GetSingletonComponent() function.
/// If a second component is created, the manager will log an error. The first created component will be used as the 'singleton',
/// all other components are ignored.
/// Use this for components derived from wdSettingsComponent, of which one should only have zero or one per world.
template <typename ComponentType>
class wdSettingsComponentManager : public wdComponentManagerBase
{
  WD_DISALLOW_COPY_AND_ASSIGN(wdSettingsComponentManager);

public:
  wdSettingsComponentManager(wdWorld* pWorld);
  ~wdSettingsComponentManager();

  /// \brief Returns the first component of this type that has been created.
  ComponentType* GetSingletonComponent();
  const ComponentType* GetSingletonComponent() const;

  static wdWorldModuleTypeId TypeId();

  // wdComponentManagerBase implementation
  virtual void CollectAllComponents(wdDynamicArray<wdComponentHandle>& out_allComponents, bool bOnlyActive) override;
  virtual void CollectAllComponents(wdDynamicArray<wdComponent*>& out_allComponents, bool bOnlyActive) override;

private:
  friend class wdComponentManagerFactory;

  virtual wdComponent* CreateComponentStorage() override;
  virtual void DeleteComponentStorage(wdComponent* pComponent, wdComponent*& out_pMovedComponent) override;

  wdHybridArray<wdUniquePtr<ComponentType>, 2> m_Components;
};

#include <Core/World/Implementation/SettingsComponentManager_inl.h>
