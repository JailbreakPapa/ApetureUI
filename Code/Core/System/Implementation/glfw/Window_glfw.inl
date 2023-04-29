#include <Core/System/Window.h>
#include <Foundation/Configuration/Startup.h>

#include <GLFW/glfw3.h>

#if WD_ENABLED(WD_PLATFORM_WINDOWS_DESKTOP)
#  ifdef APIENTRY
#    undef APIENTRY
#endif

# include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#  define GLFW_EXPOSE_NATIVE_WIN32
#  include <GLFW/glfw3native.h>
#endif

namespace
{
  void glfwErrorCallback(int errorCode, const char* msg)
  {
    wdLog::Error("GLFW error {}: {}", errorCode, msg);
  }
}


// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, Window)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    if (!glfwInit())
    {
      const char* szErrorDesc = nullptr;
      int iErrorCode = glfwGetError(&szErrorDesc);
      wdLog::Warning("Failed to initialize glfw. Window and input related functionality will not be available. Error Code {}. GLFW Error Message: {}", iErrorCode, szErrorDesc);
    }
    else
    {
      // Set the error callback after init, so we don't print an error if init fails.
      glfwSetErrorCallback(&glfwErrorCallback);
    }
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    glfwSetErrorCallback(nullptr);
    glfwTerminate();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on

namespace {
  wdResult wdGlfwError(const char* file, size_t line)
  {
    const char* desc;
    int errorCode = glfwGetError(&desc);
    if(errorCode != GLFW_NO_ERROR)
    {
      wdLog::Error("GLFW error {} ({}): {} - {}", file, line, errorCode, desc);
      return WD_FAILURE;
    }
    return WD_SUCCESS;
  }
}

#define WD_GLFW_RETURN_FAILURE_ON_ERROR() do { if(wdGlfwError(__FILE__, __LINE__).Failed()) return WD_FAILURE; } while(false)

wdResult wdWindow::Initialize()
{
  WD_LOG_BLOCK("wdWindow::Initialize", m_CreationDescription.m_Title.GetData());

  if (m_bInitialized)
  {
    Destroy().IgnoreResult();
  }

  WD_ASSERT_RELEASE(m_CreationDescription.m_Resolution.HasNonZeroArea(), "The client area size can't be zero sized!");

  GLFWmonitor* pMonitor = nullptr; // nullptr for windowed, fullscreen otherwise

  switch (m_CreationDescription.m_WindowMode)
  {
    case wdWindowMode::WindowResizable:
      glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
      WD_GLFW_RETURN_FAILURE_ON_ERROR();
      break;
    case wdWindowMode::WindowFixedResolution:
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      WD_GLFW_RETURN_FAILURE_ON_ERROR();
      break;
    case wdWindowMode::FullscreenFixedResolution:
    case wdWindowMode::FullscreenBorderlessNativeResolution:
      if (m_CreationDescription.m_iMonitor == -1)
      {
        pMonitor = glfwGetPrimaryMonitor();
        WD_GLFW_RETURN_FAILURE_ON_ERROR();
      }
      else
      {
        int iMonitorCount = 0;
        GLFWmonitor** pMonitors = glfwGetMonitors(&iMonitorCount);
        WD_GLFW_RETURN_FAILURE_ON_ERROR();
        if (m_CreationDescription.m_iMonitor >= iMonitorCount)
        {
          wdLog::Error("Can not create window on monitor {} only {} monitors connected", m_CreationDescription.m_iMonitor, iMonitorCount);
          return WD_FAILURE;
        }
        pMonitor = pMonitors[m_CreationDescription.m_iMonitor];
      }

      if (m_CreationDescription.m_WindowMode == wdWindowMode::FullscreenBorderlessNativeResolution)
      {
        const GLFWvidmode* pVideoMode = glfwGetVideoMode(pMonitor);
        WD_GLFW_RETURN_FAILURE_ON_ERROR();
        if(pVideoMode == nullptr)
        {
          wdLog::Error("Failed to get video mode for monitor");
          return WD_FAILURE;
        }
        m_CreationDescription.m_Resolution.width = pVideoMode->width;
        m_CreationDescription.m_Resolution.height = pVideoMode->height;
        m_CreationDescription.m_Position.x = 0;
        m_CreationDescription.m_Position.y = 0;

        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        WD_GLFW_RETURN_FAILURE_ON_ERROR();
      }

      break;
  }


  glfwWindowHint(GLFW_FOCUS_ON_SHOW, m_CreationDescription.m_bSetForegroundOnInit ? GLFW_TRUE : GLFW_FALSE);
  WD_GLFW_RETURN_FAILURE_ON_ERROR();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  WD_GLFW_RETURN_FAILURE_ON_ERROR();

  GLFWwindow* pWindow = glfwCreateWindow(m_CreationDescription.m_Resolution.width, m_CreationDescription.m_Resolution.height, m_CreationDescription.m_Title.GetData(), pMonitor, NULL);
  WD_GLFW_RETURN_FAILURE_ON_ERROR();

  if (pWindow == nullptr)
  {
    wdLog::Error("Failed to create glfw window");
    return WD_FAILURE;
  }
#if WD_ENABLED(WD_PLATFORM_LINUX)
  m_hWindowHandle.type = wdWindowHandle::Type::GLFW;
  m_hWindowHandle.glfwWindow = pWindow;
#else
  m_hWindowHandle = pWindow;
#endif

  if (m_CreationDescription.m_Position != wdVec2I32(0x80000000, 0x80000000))
  {
    glfwSetWindowPos(pWindow, m_CreationDescription.m_Position.x, m_CreationDescription.m_Position.y);
    WD_GLFW_RETURN_FAILURE_ON_ERROR();
  }

  glfwSetWindowUserPointer(pWindow, this);
  glfwSetWindowSizeCallback(pWindow, &wdWindow::SizeCallback);
  glfwSetWindowPosCallback(pWindow, &wdWindow::PositionCallback);
  glfwSetWindowCloseCallback(pWindow, &wdWindow::CloseCallback);
  glfwSetWindowFocusCallback(pWindow, &wdWindow::FocusCallback);
  glfwSetKeyCallback(pWindow, &wdWindow::KeyCallback);
  glfwSetCharCallback(pWindow, &wdWindow::CharacterCallback);
  glfwSetCursorPosCallback(pWindow, &wdWindow::CursorPositionCallback);
  glfwSetMouseButtonCallback(pWindow, &wdWindow::MouseButtonCallback);
  glfwSetScrollCallback(pWindow, &wdWindow::ScrollCallback);
  WD_GLFW_RETURN_FAILURE_ON_ERROR();

#if WD_ENABLED(WD_PLATFORM_LINUX)
  WD_ASSERT_DEV(m_hWindowHandle.type == wdWindowHandle::Type::GLFW, "not a GLFW handle");
  m_pInputDevice = WD_DEFAULT_NEW(wdStandardInputDevice, m_CreationDescription.m_uiWindowNumber, m_hWindowHandle.glfwWindow);
#else
  m_pInputDevice = WD_DEFAULT_NEW(wdStandardInputDevice, m_CreationDescription.m_uiWindowNumber, m_hWindowHandle);
#endif

  m_pInputDevice->SetClipMouseCursor(m_CreationDescription.m_bClipMouseCursor ? wdMouseCursorClipMode::ClipToWindowImmediate : wdMouseCursorClipMode::NoClip);
  m_pInputDevice->SetShowMouseCursor(m_CreationDescription.m_bShowMouseCursor);

  m_bInitialized = true;
  wdLog::Success("Created glfw window successfully. Resolution is {0}*{1}", GetClientAreaSize().width, GetClientAreaSize().height);

  return WD_SUCCESS;
}

wdResult wdWindow::Destroy()
{
  if (m_bInitialized)
  {
    WD_LOG_BLOCK("wdWindow::Destroy");

    m_pInputDevice = nullptr;

#if WD_ENABLED(WD_PLATFORM_LINUX)
    WD_ASSERT_DEV(m_hWindowHandle.type == wdWindowHandle::Type::GLFW, "GLFW handle expected");
    glfwDestroyWindow(m_hWindowHandle.glfwWindow);
#else
    glfwDestroyWindow(m_hWindowHandle);
#endif
    m_hWindowHandle = INVALID_INTERNAL_WINDOW_HANDLE_VALUE;

    m_bInitialized = false;
  }

  return WD_SUCCESS;
}

wdResult wdWindow::Resize(const wdSizeU32& newWindowSize)
{
  if (!m_bInitialized)
    return WD_FAILURE;

#if WD_ENABLED(WD_PLATFORM_LINUX)
  WD_ASSERT_DEV(m_hWindowHandle.type == wdWindowHandle::Type::GLFW, "Expected GLFW handle");
  glfwSetWindowSize(m_hWindowHandle.glfwWindow, newWindowSize.width, newWindowSize.height);
#else
  glfwSetWindowSize(m_hWindowHandle, newWindowSize.width, newWindowSize.height);
#endif
  WD_GLFW_RETURN_FAILURE_ON_ERROR();

  return WD_SUCCESS;
}

void wdWindow::ProcessWindowMessages()
{
  if (!m_bInitialized)
    return;

  // Only run the global event processing loop for the main window.
  if (m_CreationDescription.m_uiWindowNumber == 0)
  {
    glfwPollEvents();
  }

#if WD_ENABLED(WD_PLATFORM_LINUX)
  WD_ASSERT_DEV(m_hWindowHandle.type == wdWindowHandle::Type::GLFW, "Expected GLFW handle");
  if (glfwWindowShouldClose(m_hWindowHandle.glfwWindow))
  {
    Destroy().IgnoreResult();
  }
#else
  if (glfwWindowShouldClose(m_hWindowHandle))
  {
    Destroy().IgnoreResult();
  }
#endif
}

void wdWindow::OnResize(const wdSizeU32& newWindowSize)
{
  wdLog::Info("Window resized to ({0}, {1})", newWindowSize.width, newWindowSize.height);
}

void wdWindow::SizeCallback(GLFWwindow* window, int width, int height)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && width > 0 && height > 0)
  {
    self->OnResize(wdSizeU32(static_cast<wdUInt32>(width), static_cast<wdUInt32>(height)));
  }
}

void wdWindow::PositionCallback(GLFWwindow* window, int xpos, int ypos)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->OnWindowMove(xpos, ypos);
  }
}

void wdWindow::CloseCallback(GLFWwindow* window)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->OnClickClose();
  }
}

void wdWindow::FocusCallback(GLFWwindow* window, int focused)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self)
  {
    self->OnFocus(focused ? true : false);
  }
}

void wdWindow::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && self->m_pInputDevice)
  {
    self->m_pInputDevice->OnKey(key, scancode, action, mods);
  }
}

void wdWindow::CharacterCallback(GLFWwindow* window, unsigned int codepoint)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && self->m_pInputDevice)
  {
    self->m_pInputDevice->OnCharacter(codepoint);
  }
}

void wdWindow::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && self->m_pInputDevice)
  {
    self->m_pInputDevice->OnCursorPosition(xpos, ypos);
  }
}

void wdWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && self->m_pInputDevice)
  {
    self->m_pInputDevice->OnMouseButton(button, action, mods);
  }
}

void wdWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  auto self = static_cast<wdWindow*>(glfwGetWindowUserPointer(window));
  if (self && self->m_pInputDevice)
  {
    self->m_pInputDevice->OnScroll(xoffset, yoffset);
  }
}

wdWindowHandle wdWindow::GetNativeWindowHandle() const
{
#if WD_ENABLED(WD_PLATFORM_WINDOWS_DESKTOP)
  return wdMinWindows::FromNative<HWND>(glfwGetWin32Window(m_hWindowHandle));
#else
  return m_hWindowHandle;
#endif
}
