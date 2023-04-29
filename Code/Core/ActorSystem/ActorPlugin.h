#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Reflection/Reflection.h>

class wdActor;

class WD_CORE_DLL wdActorPlugin : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(wdActorPlugin, wdReflectedClass);

public:
  wdActorPlugin();
  ~wdActorPlugin();

  wdActor* GetActor() const;

protected:
  friend class wdActor;
  virtual void Update() {}

private:
  wdActor* m_pOwningActor = nullptr;
};
