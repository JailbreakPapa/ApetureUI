#pragma once

#include <Core/CoreDLL.h>

#include <Foundation/Reflection/Reflection.h>

class WD_CORE_DLL wdActorApiService : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdActorApiService, wdReflectedClass);
  WD_DISALLOW_COPY_AND_ASSIGN(wdActorApiService);

public:
  wdActorApiService();
  ~wdActorApiService();

protected:
  virtual void Activate() = 0;
  virtual void Update() = 0;

private: // directly accessed by wdActorManager
  friend class wdActorManager;

  enum class State
  {
    New,
    Active,
    QueuedForDestruction
  };

  State m_State = State::New;
};
