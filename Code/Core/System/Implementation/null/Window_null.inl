#include <Core/System/Window.h>

wdResult wdWindow::Initialize()
{
  WD_ASSERT_NOT_IMPLEMENTED;
  return WD_FAILURE;
}

wdResult wdWindow::Destroy()
{
  WD_ASSERT_NOT_IMPLEMENTED;
  return WD_FAILURE;
}

wdResult wdWindow::Resize(const wdSizeU32& newWindowSize)
{
  WD_ASSERT_NOT_IMPLEMENTED;
  return WD_FAILURE;
}

void wdWindow::ProcessWindowMessages()
{
  WD_ASSERT_NOT_IMPLEMENTED;
}

void wdWindow::OnResize(const wdSizeU32& newWindowSize)
{
  WD_ASSERT_NOT_IMPLEMENTED;
}

wdWindowHandle wdWindow::GetNativeWindowHandle() const
{
  return m_hWindowHandle;
}
