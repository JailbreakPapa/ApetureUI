#include <Core/CorePCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics.h>
#include <Foundation/Logging/Log.h>

static LRESULT CALLBACK wdWindowsMessageFuncTrampoline(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  wdWindow* pWindow = reinterpret_cast<wdWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

  if (pWindow != nullptr && pWindow->IsInitialized())
  {
    if (pWindow->GetInputDevice())
      pWindow->GetInputDevice()->WindowMessage(wdMinWindows::FromNative(hWnd), msg, wparam, lparam);

    switch (msg)
    {
      case WM_CLOSE:
        pWindow->OnClickClose();
        return 0;

      case WM_SETFOCUS:
        pWindow->OnFocus(true);
        return 0;

      case WM_KILLFOCUS:
        pWindow->OnFocus(false);
        return 0;

      case WM_SIZE:
      {
        wdSizeU32 size(LOWORD(lparam), HIWORD(lparam));
        pWindow->OnResize(size);
      }
      break;

      case WM_SYSKEYDOWN:
      {
        // filter this message out, otherwise pressing ALT will give focus to the system menu, locking out other actions
        // until ALT is pressed again, which is typically not desired
        return 0;
      }

      case WM_MOVE:
      {
        pWindow->OnWindowMove((int)(short)LOWORD(lparam), (int)(short)HIWORD(lparam));
      }
      break;
    }

    pWindow->OnWindowMessage(wdMinWindows::FromNative(hWnd), msg, wparam, lparam);
  }

  return DefWindowProcW(hWnd, msg, wparam, lparam);
}

wdResult wdWindow::Initialize()
{
  WD_LOG_BLOCK("wdWindow::Initialize", m_CreationDescription.m_Title.GetData());

  if (m_bInitialized)
  {
    Destroy().IgnoreResult();
  }

  WD_ASSERT_RELEASE(m_CreationDescription.m_Resolution.HasNonZeroArea(), "The client area size can't be zero sized!");

  // Initialize window class
  WNDCLASSEXW windowClass = {};
  windowClass.cbSize = sizeof(WNDCLASSEXW);
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.hInstance = GetModuleHandleW(nullptr);
  windowClass.hIcon = LoadIcon(GetModuleHandleW(nullptr), MAKEINTRESOURCE(101)); /// \todo Expose icon functionality somehow (101 == IDI_ICON1, see resource.h)
  windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
  windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
  windowClass.lpszClassName = L"wdWin32Window";
  windowClass.lpfnWndProc = wdWindowsMessageFuncTrampoline;

  if (!RegisterClassExW(&windowClass)) /// \todo test & support for multiple windows
  {
    DWORD error = GetLastError();

    if (error != ERROR_CLASS_ALREADY_EXISTS)
    {
      wdLog::Error("Failed to create wdWindow window class! (error code '{0}')", wdArgU(error));
      return WD_FAILURE;
    }
  }

  // setup fullscreen mode
  if (m_CreationDescription.m_WindowMode == wdWindowMode::FullscreenFixedResolution)
  {
    wdLog::Dev("Changing display resolution for fullscreen mode to {0}*{1}", m_CreationDescription.m_Resolution.width, m_CreationDescription.m_Resolution.height);

    DEVMODEW dmScreenSettings = {};
    dmScreenSettings.dmSize = sizeof(DEVMODEW);
    dmScreenSettings.dmPelsWidth = m_CreationDescription.m_Resolution.width;
    dmScreenSettings.dmPelsHeight = m_CreationDescription.m_Resolution.height;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    if (ChangeDisplaySettingsW(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
      m_CreationDescription.m_WindowMode = wdWindowMode::FullscreenBorderlessNativeResolution;
      WD_SUCCEED_OR_RETURN(m_CreationDescription.AdjustWindowSizeAndPosition());

      wdLog::Error("Failed to change display resolution for fullscreen window. Falling back to borderless window.");
    }
  }


  // setup window style
  DWORD dwExStyle = WS_EX_APPWINDOW;
  DWORD dwWindowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

  if (m_CreationDescription.m_WindowMode == wdWindowMode::WindowFixedResolution || m_CreationDescription.m_WindowMode == wdWindowMode::WindowResizable)
  {
    wdLog::Dev("Window is not fullscreen.");
    dwWindowStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
  }
  else
  {
    wdLog::Dev("Window is fullscreen.");
    dwWindowStyle |= WS_POPUP;
  }

  if (m_CreationDescription.m_WindowMode == wdWindowMode::WindowResizable)
  {
    wdLog::Dev("Window is resizable.");
    dwWindowStyle |= WS_MAXIMIZEBOX | WS_THICKFRAME;
  }


  // Create rectangle for window
  RECT Rect = {0, 0, (LONG)m_CreationDescription.m_Resolution.width, (LONG)m_CreationDescription.m_Resolution.height};

  // Account for left or top placed task bars
  if (m_CreationDescription.m_WindowMode == wdWindowMode::WindowFixedResolution || m_CreationDescription.m_WindowMode == wdWindowMode::WindowResizable)
  {
    // Adjust for borders and bars etc.
    AdjustWindowRectEx(&Rect, dwWindowStyle, FALSE, dwExStyle);

    // top left position now may be negative (due to AdjustWindowRectEx)
    // move
    Rect.right -= Rect.left;
    Rect.bottom -= Rect.top;
    // apply user translation
    Rect.left = m_CreationDescription.m_Position.x;
    Rect.top = m_CreationDescription.m_Position.y;
    Rect.right += m_CreationDescription.m_Position.x;
    Rect.bottom += m_CreationDescription.m_Position.y;

    // move into work area
    RECT RectWorkArea = {0};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &RectWorkArea, 0);

    Rect.left += RectWorkArea.left;
    Rect.right += RectWorkArea.left;
    Rect.top += RectWorkArea.top;
    Rect.bottom += RectWorkArea.top;
  }

  const int iWidth = Rect.right - Rect.left;
  const int iHeight = Rect.bottom - Rect.top;

  wdLog::Info("Window Dimensions: {0}*{1} at left/top origin ({2}, {3}).", iWidth, iHeight, m_CreationDescription.m_Position.x, m_CreationDescription.m_Position.y);


  // create window
  wdStringWChar sTitelWChar(m_CreationDescription.m_Title.GetData());
  const wchar_t* sTitelWCharRaw = sTitelWChar.GetData();
  m_hWindowHandle = wdMinWindows::FromNative(CreateWindowExW(dwExStyle, windowClass.lpszClassName, sTitelWCharRaw, dwWindowStyle, m_CreationDescription.m_Position.x, m_CreationDescription.m_Position.y, iWidth, iHeight, nullptr, nullptr, windowClass.hInstance, nullptr));

  if (m_hWindowHandle == INVALID_HANDLE_VALUE)
  {
    wdLog::Error("Failed to create window.");
    return WD_FAILURE;
  }

  auto windowHandle = wdMinWindows::ToNative(m_hWindowHandle);

  // safe window pointer for lookup in wdWindowsMessageFuncTrampoline
  SetWindowLongPtrW(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  // show window and activate if required
  ShowWindow(windowHandle, m_CreationDescription.m_bSetForegroundOnInit ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE);
  if (m_CreationDescription.m_bSetForegroundOnInit)
  {
    SetActiveWindow(windowHandle);
    SetFocus(windowHandle);
    SetForegroundWindow(windowHandle);
  }

  RECT r;
  GetClientRect(windowHandle, &r);

  // Force size change to the desired size if CreateWindowExW 'fixed' the size to fit into your current monitor.
  if (m_CreationDescription.m_WindowMode == wdWindowMode::WindowFixedResolution && (m_CreationDescription.m_Resolution.width != r.right - r.left || m_CreationDescription.m_Resolution.height != r.bottom - r.top))
  {
    ::SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, iWidth, iHeight, SWP_NOSENDCHANGING | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
    GetClientRect(windowHandle, &r);
  }

  m_CreationDescription.m_Resolution.width = r.right - r.left;
  m_CreationDescription.m_Resolution.height = r.bottom - r.top;



  m_bInitialized = true;
  wdLog::Success("Created window successfully. Resolution is {0}*{1}", GetClientAreaSize().width, GetClientAreaSize().height);

  m_pInputDevice = WD_DEFAULT_NEW(wdStandardInputDevice, m_CreationDescription.m_uiWindowNumber);
  m_pInputDevice->SetClipMouseCursor(m_CreationDescription.m_bClipMouseCursor ? wdMouseCursorClipMode::ClipToWindowImmediate : wdMouseCursorClipMode::NoClip);
  m_pInputDevice->SetShowMouseCursor(m_CreationDescription.m_bShowMouseCursor);

  return WD_SUCCESS;
}

wdResult wdWindow::Destroy()
{
  if (!m_bInitialized)
    return WD_SUCCESS;

  if (GetInputDevice() && GetInputDevice()->GetClipMouseCursor())
    GetInputDevice()->SetClipMouseCursor(wdMouseCursorClipMode::NoClip);

  WD_LOG_BLOCK("wdWindow::Destroy");

  wdResult Res = WD_SUCCESS;

  m_pInputDevice = nullptr;

  if (m_CreationDescription.m_WindowMode == wdWindowMode::FullscreenFixedResolution)
    ChangeDisplaySettingsW(nullptr, 0);

  HWND hWindow = wdMinWindows::ToNative(GetNativeWindowHandle());
  // the following line of code is a work around, because 'LONG_PTR pNull = reinterpret_cast<LONG_PTR>(nullptr)' crashes the VS 2010 32 Bit
  // compiler :-(
  LONG_PTR pNull = 0;
  // Set the window ptr to null before calling DestroyWindow as it might trigger callbacks and we are potentially already in the destructor, making any virtual function call unsafe.
  SetWindowLongPtrW(hWindow, GWLP_USERDATA, pNull);

  if (!DestroyWindow(hWindow))
  {
    wdLog::SeriousWarning("DestroyWindow failed.");
    Res = WD_FAILURE;
  }



  // actually nobody cares about this, all Window Classes are cleared when the application closes
  // in the mean time, having multiple windows will just result in errors when one is closed,
  // as the Window Class must not be in use anymore when one calls UnregisterClassW
  // if (!UnregisterClassW(L"wdWin32Window", GetModuleHandleW(nullptr)))
  //{
  //  wdLog::SeriousWarning("UnregisterClassW failed.");
  //  Res = WD_FAILURE;
  //}

  m_bInitialized = false;
  m_hWindowHandle = INVALID_WINDOW_HANDLE_VALUE;

  if (Res == WD_SUCCESS)
    wdLog::Success("Window destroyed.");
  else
    wdLog::SeriousWarning("There were problems to destroy the window properly.");

  return Res;
}

wdResult wdWindow::Resize(const wdSizeU32& newWindowSize)
{
  auto windowHandle = wdMinWindows::ToNative(m_hWindowHandle);
  BOOL res = ::SetWindowPos(windowHandle, HWND_NOTOPMOST, 0, 0, newWindowSize.width, newWindowSize.height, SWP_NOSENDCHANGING | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
  return res == TRUE ? WD_SUCCESS : WD_FAILURE;
}

void wdWindow::ProcessWindowMessages()
{
  if (!m_bInitialized)
    return;

  MSG msg = {0};
  while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      Destroy().IgnoreResult();
      return;
    }

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

void wdWindow::OnResize(const wdSizeU32& newWindowSize)
{
  wdLog::Info("Window resized to ({0}, {1})", newWindowSize.width, newWindowSize.height);
}

wdWindowHandle wdWindow::GetNativeWindowHandle() const
{
  return m_hWindowHandle;
}
