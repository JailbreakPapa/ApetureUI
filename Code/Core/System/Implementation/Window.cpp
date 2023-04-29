#include <Core/CorePCH.h>

#include <Core/System/Window.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/IO/OpenDdlReader.h>
#include <Foundation/IO/OpenDdlUtils.h>
#include <Foundation/IO/OpenDdlWriter.h>
#include <Foundation/System/Screen.h>

#if WD_ENABLED(WD_SUPPORTS_GLFW)
#  include <Core/System/Implementation/glfw/InputDevice_glfw.inl>
#  include <Core/System/Implementation/glfw/Window_glfw.inl>
#elif WD_ENABLED(WD_PLATFORM_WINDOWS_DESKTOP)
#  include <Core/System/Implementation/Win/InputDevice_win32.inl>
#  include <Core/System/Implementation/Win/Window_win32.inl>
#elif WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
#  include <Core/System/Implementation/uwp/InputDevice_uwp.inl>
#  include <Core/System/Implementation/uwp/Window_uwp.inl>
#else
#  include <Core/System/Implementation/null/InputDevice_null.inl>
#  include <Core/System/Implementation/null/Window_null.inl>
#endif

wdUInt8 wdWindow::s_uiNextUnusedWindowNumber = 0;

wdResult wdWindowCreationDesc::AdjustWindowSizeAndPosition()
{
  if (m_WindowMode == wdWindowMode::WindowFixedResolution || m_WindowMode == wdWindowMode::WindowResizable)
    return WD_SUCCESS;

  wdHybridArray<wdScreenInfo, 2> screens;
  if (wdScreen::EnumerateScreens(screens).Failed() || screens.IsEmpty())
    return WD_FAILURE;

  wdInt32 iShowOnMonitor = m_iMonitor;

  if (iShowOnMonitor >= (wdInt32)screens.GetCount())
    iShowOnMonitor = -1;

  const wdScreenInfo* pScreen = nullptr;

  // this means 'pick the primary screen'
  if (iShowOnMonitor < 0)
  {
    pScreen = &screens[0];

    for (wdUInt32 i = 0; i < screens.GetCount(); ++i)
    {
      if (screens[i].m_bIsPrimary)
      {
        pScreen = &screens[i];
        break;
      }
    }
  }
  else
  {
    pScreen = &screens[iShowOnMonitor];
  }

  m_Position.Set(pScreen->m_iOffsetX, pScreen->m_iOffsetY);

  if (m_WindowMode == wdWindowMode::FullscreenBorderlessNativeResolution)
  {
    m_Resolution.width = pScreen->m_iResolutionX;
    m_Resolution.height = pScreen->m_iResolutionY;
  }
  else
  {
    // clamp the resolution to the native resolution ?
    // m_ClientAreaSize.width = wdMath::Min<wdUInt32>(m_ClientAreaSize.width, pScreen->m_iResolutionX);
    // m_ClientAreaSize.height= wdMath::Min<wdUInt32>(m_ClientAreaSize.height,pScreen->m_iResolutionY);
  }

  return WD_SUCCESS;
}

void wdWindowCreationDesc::SaveToDDL(wdOpenDdlWriter& ref_writer)
{
  ref_writer.BeginObject("WindowDesc");

  wdOpenDdlUtils::StoreString(ref_writer, m_Title, "Title");

  switch (m_WindowMode.GetValue())
  {
    case wdWindowMode::FullscreenBorderlessNativeResolution:
      wdOpenDdlUtils::StoreString(ref_writer, "Borderless", "Mode");
      break;
    case wdWindowMode::FullscreenFixedResolution:
      wdOpenDdlUtils::StoreString(ref_writer, "Fullscreen", "Mode");
      break;
    case wdWindowMode::WindowFixedResolution:
      wdOpenDdlUtils::StoreString(ref_writer, "Window", "Mode");
      break;
    case wdWindowMode::WindowResizable:
      wdOpenDdlUtils::StoreString(ref_writer, "ResizableWindow", "Mode");
      break;
  }

  if (m_uiWindowNumber != 0)
    wdOpenDdlUtils::StoreUInt8(ref_writer, m_uiWindowNumber, "Index");

  if (m_iMonitor >= 0)
    wdOpenDdlUtils::StoreInt8(ref_writer, m_iMonitor, "Monitor");

  if (m_Position != wdVec2I32(0x80000000, 0x80000000))
  {
    wdOpenDdlUtils::StoreVec2I(ref_writer, m_Position, "Position");
  }

  wdOpenDdlUtils::StoreVec2U(ref_writer, wdVec2U32(m_Resolution.width, m_Resolution.height), "Resolution");

  wdOpenDdlUtils::StoreBool(ref_writer, m_bClipMouseCursor, "ClipMouseCursor");
  wdOpenDdlUtils::StoreBool(ref_writer, m_bShowMouseCursor, "ShowMouseCursor");
  wdOpenDdlUtils::StoreBool(ref_writer, m_bSetForegroundOnInit, "SetForegroundOnInit");

  ref_writer.EndObject();
}


wdResult wdWindowCreationDesc::SaveToDDL(wdStringView sFile)
{
  wdFileWriter file;
  WD_SUCCEED_OR_RETURN(file.Open(sFile));

  wdOpenDdlWriter writer;
  writer.SetOutputStream(&file);

  SaveToDDL(writer);

  return WD_SUCCESS;
}

void wdWindowCreationDesc::LoadFromDDL(const wdOpenDdlReaderElement* pParentElement)
{
  if (const wdOpenDdlReaderElement* pDesc = pParentElement->FindChildOfType("WindowDesc"))
  {
    if (const wdOpenDdlReaderElement* pTitle = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::String, "Title"))
      m_Title = pTitle->GetPrimitivesString()[0];

    if (const wdOpenDdlReaderElement* pMode = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::String, "Mode"))
    {
      auto mode = pMode->GetPrimitivesString()[0];

      if (mode == "Borderless")
        m_WindowMode = wdWindowMode::FullscreenBorderlessNativeResolution;
      else if (mode == "Fullscreen")
        m_WindowMode = wdWindowMode::FullscreenFixedResolution;
      else if (mode == "Window")
        m_WindowMode = wdWindowMode::WindowFixedResolution;
      else if (mode == "ResizableWindow")
        m_WindowMode = wdWindowMode::WindowResizable;
    }

    if (const wdOpenDdlReaderElement* pIndex = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::UInt8, "Index"))
    {
      m_uiWindowNumber = pIndex->GetPrimitivesUInt8()[0];
    }

    if (const wdOpenDdlReaderElement* pMonitor = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::Int8, "Monitor"))
    {
      m_iMonitor = pMonitor->GetPrimitivesInt8()[0];
    }

    if (const wdOpenDdlReaderElement* pPosition = pDesc->FindChild("Position"))
    {
      wdOpenDdlUtils::ConvertToVec2I(pPosition, m_Position).IgnoreResult();
    }

    if (const wdOpenDdlReaderElement* pPosition = pDesc->FindChild("Resolution"))
    {
      wdVec2U32 res;
      wdOpenDdlUtils::ConvertToVec2U(pPosition, res).IgnoreResult();
      m_Resolution.width = res.x;
      m_Resolution.height = res.y;
    }

    if (const wdOpenDdlReaderElement* pClipMouseCursor = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::Bool, "ClipMouseCursor"))
      m_bClipMouseCursor = pClipMouseCursor->GetPrimitivesBool()[0];

    if (const wdOpenDdlReaderElement* pShowMouseCursor = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::Bool, "ShowMouseCursor"))
      m_bShowMouseCursor = pShowMouseCursor->GetPrimitivesBool()[0];

    if (const wdOpenDdlReaderElement* pSetForegroundOnInit = pDesc->FindChildOfType(wdOpenDdlPrimitiveType::Bool, "SetForegroundOnInit"))
      m_bSetForegroundOnInit = pSetForegroundOnInit->GetPrimitivesBool()[0];
  }
}


wdResult wdWindowCreationDesc::LoadFromDDL(wdStringView sFile)
{
  wdFileReader file;
  WD_SUCCEED_OR_RETURN(file.Open(sFile));

  wdOpenDdlReader reader;
  WD_SUCCEED_OR_RETURN(reader.ParseDocument(file));

  LoadFromDDL(reader.GetRootElement());

  return WD_SUCCESS;
}

wdWindow::wdWindow()
{
  ++s_uiNextUnusedWindowNumber;
}

wdWindow::~wdWindow()
{
  if (m_bInitialized)
  {
    Destroy().IgnoreResult();
  }
  WD_ASSERT_DEV(m_iReferenceCount == 0, "The window is still being referenced, probably by a swapchain. Make sure to destroy all swapchains and call wdGALDevice::WaitIdle before destroying a window.");
}

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
void wdWindow::OnWindowMessage(wdMinWindows::HWND pWnd, wdMinWindows::UINT msg, wdMinWindows::WPARAM wparam, wdMinWindows::LPARAM lparam)
{
}
#endif

wdUInt8 wdWindow::GetNextUnusedWindowNumber()
{
  return s_uiNextUnusedWindowNumber;
}


WD_STATICLINK_FILE(Core, Core_System_Implementation_Window);
