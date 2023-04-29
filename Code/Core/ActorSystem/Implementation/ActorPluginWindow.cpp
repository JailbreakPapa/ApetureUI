#include <Core/CorePCH.h>

#include <Core/ActorSystem/ActorPluginWindow.h>

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActorPluginWindow, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

void wdActorPluginWindow::Update()
{
  if (GetWindow())
  {
    GetWindow()->ProcessWindowMessages();
  }
}

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActorPluginWindowOwner, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdActorPluginWindowOwner::~wdActorPluginWindowOwner()
{
  // The window output target has a dependency to the window, e.g. the swapchain renders to it.
  // Explicitly destroy it first to ensure correct destruction order.
  m_pWindowOutputTarget.Clear();
  m_pWindow.Clear();
}

wdWindowBase* wdActorPluginWindowOwner::GetWindow() const
{
  return m_pWindow.Borrow();
}
wdWindowOutputTargetBase* wdActorPluginWindowOwner::GetOutputTarget() const
{
  return m_pWindowOutputTarget.Borrow();
}

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActorPluginWindowShared, 1, wdRTTINoAllocator);
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdWindowBase* wdActorPluginWindowShared::GetWindow() const
{
  return m_pWindow;
}

wdWindowOutputTargetBase* wdActorPluginWindowShared::GetOutputTarget() const
{
  return m_pWindowOutputTarget;
}


WD_STATICLINK_FILE(Core, Core_ActorSystem_Implementation_ActorPluginWindow);
