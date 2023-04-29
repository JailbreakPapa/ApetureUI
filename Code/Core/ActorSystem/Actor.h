#pragma once

#include <Core/ActorSystem/ActorPlugin.h>
#include <Foundation/Types/UniquePtr.h>

struct wdActorImpl;

class WD_CORE_DLL wdActor : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdActor, wdReflectedClass);

  WD_DISALLOW_COPY_AND_ASSIGN(wdActor);

public:
  wdActor(wdStringView sActorName, const void* pCreatedBy);
  ~wdActor();

  /// \brief Returns the name of this actor
  wdStringView GetName() const;

  /// \brief Returns the 'created by' pointer of the actor
  const void* GetCreatedBy() const;

  /// \brief Transfers ownership of the wdActorPlugin to the wdActor
  void AddPlugin(wdUniquePtr<wdActorPlugin>&& pPlugin);

  /// \brief Queries the wdActor for an wdActorPlugin of the given type. Returns null if no such plugin was added to the actor.
  wdActorPlugin* GetPlugin(const wdRTTI* pType) const;

  /// \brief Templated overload of GetPlugin() that automatically casts to the desired class type.
  template <typename Type>
  Type* GetPlugin() const
  {
    return static_cast<Type*>(GetPlugin(wdGetStaticRTTI<Type>()));
  }

  /// \brief Deletes the given plugin from the actor
  void DestroyPlugin(wdActorPlugin* pPlugin);

  /// \brief Fills the list with all plugins that have been added to the actor.
  void GetAllPlugins(wdHybridArray<wdActorPlugin*, 8>& out_allPlugins);

  /// \brief Checks whether the actor is queued for destruction at the end of the frame
  bool IsActorQueuedForDestruction() const
  {
    return m_State == State::QueuedForDestruction;
  }

protected:
  void UpdateAllPlugins();


protected: // directly touched by wdActorManager
  friend class wdActorManager;

  /// \brief Called shortly before the first call to Update()
  virtual void Activate();

  /// \brief Called once per frame to update the actor state.
  ///
  /// By default this calls UpdateAllPlugins() internally.
  virtual void Update();

private: // directly touched by wdActorManager
  enum class State
  {
    New,
    Active,
    QueuedForDestruction
  };

  State m_State = State::New;

private:
  wdUniquePtr<wdActorImpl> m_pImpl;
};
