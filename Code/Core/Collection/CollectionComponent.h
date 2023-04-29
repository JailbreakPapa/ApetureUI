#pragma once

#include <Core/Collection/CollectionResource.h>
#include <Core/CoreDLL.h>
#include <Core/World/Component.h>
#include <Core/World/World.h>

typedef wdComponentManager<class wdCollectionComponent, wdBlockStorageType::Compact> wdCollectionComponentManager;

/// \brief An wdCollectionComponent references an wdCollectionResource and triggers resource preloading when needed
///
/// Placing an wdCollectionComponent in a scene or a model makes it possible to tell the engine to preload certain resources
/// that are likely to be needed soon.
///
/// If a deactivated wdCollectionComponent is part of the scene, it will not trigger a preload, but will do so once
/// the component is activated.
class WD_CORE_DLL wdCollectionComponent : public wdComponent
{
  WD_DECLARE_COMPONENT_TYPE(wdCollectionComponent, wdComponent, wdCollectionComponentManager);

  //////////////////////////////////////////////////////////////////////////
  // wdComponent
public:
  virtual void SerializeComponent(wdWorldWriter& inout_stream) const override;
  virtual void DeserializeComponent(wdWorldReader& inout_stream) override;

protected:
  virtual void OnSimulationStarted() override;

  //////////////////////////////////////////////////////////////////////////
  // wdCollectionComponent
public:
  wdCollectionComponent();
  ~wdCollectionComponent();

  void SetCollectionFile(const char* szFile); // [ property ]
  const char* GetCollectionFile() const;      // [ property ]

  void SetCollection(const wdCollectionResourceHandle& hPrefab);
  WD_ALWAYS_INLINE const wdCollectionResourceHandle& GetCollection() const { return m_hCollection; }

protected:
  /// \brief Triggers the preload on the referenced wdCollectionResource
  void InitiatePreload();

  wdCollectionResourceHandle m_hCollection;
};
