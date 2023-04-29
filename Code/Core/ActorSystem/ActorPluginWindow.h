#pragma once

#include <Core/ActorSystem/ActorPlugin.h>

#include <Core/GameApplication/WindowOutputTargetBase.h>
#include <Core/System/Window.h>

class wdActor;
class wdWindowOutputTargetBase;
class wdWindowBase;

class WD_CORE_DLL wdActorPluginWindow : public wdActorPlugin
{
  WD_ADD_DYNAMIC_REFLECTION(wdActorPluginWindow, wdActorPlugin);

public:
  virtual wdWindowBase* GetWindow() const = 0;
  virtual wdWindowOutputTargetBase* GetOutputTarget() const = 0;

protected:
  virtual void Update() override;
};

class WD_CORE_DLL wdActorPluginWindowOwner : public wdActorPluginWindow
{
  WD_ADD_DYNAMIC_REFLECTION(wdActorPluginWindowOwner, wdActorPluginWindow);

public:
  virtual ~wdActorPluginWindowOwner();
  virtual wdWindowBase* GetWindow() const override;
  virtual wdWindowOutputTargetBase* GetOutputTarget() const override;

  wdUniquePtr<wdWindowBase> m_pWindow;
  wdUniquePtr<wdWindowOutputTargetBase> m_pWindowOutputTarget;
};

class WD_CORE_DLL wdActorPluginWindowShared : public wdActorPluginWindow
{
  WD_ADD_DYNAMIC_REFLECTION(wdActorPluginWindowShared, wdActorPluginWindow);

public:
  virtual wdWindowBase* GetWindow() const override;
  virtual wdWindowOutputTargetBase* GetOutputTarget() const override;

  wdWindowBase* m_pWindow = nullptr;
  wdWindowOutputTargetBase* m_pWindowOutputTarget = nullptr;
};
