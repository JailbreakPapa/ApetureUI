#include <Core/Input/InputManager.h>
#include <Core/System/Implementation/Win/InputDevice_win32.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/StringConversion.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdStandardInputDevice, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

bool wdStandardInputDevice::s_bMainWindowUsed = false;

wdStandardInputDevice::wdStandardInputDevice(wdUInt32 uiWindowNumber)
{
  m_uiWindowNumber = uiWindowNumber;

  if (uiWindowNumber == 0)
  {
    WD_ASSERT_RELEASE(!s_bMainWindowUsed, "You cannot have two devices of Type wdStandardInputDevice with the window number zero.");
    wdStandardInputDevice::s_bMainWindowUsed = true;
  }

  m_DoubleClickTime = wdTime::Milliseconds(GetDoubleClickTime());
}

wdStandardInputDevice::~wdStandardInputDevice()
{
  if (!m_bShowCursor)
  {
    ShowCursor(true);
  }

  if (m_uiWindowNumber == 0)
    wdStandardInputDevice::s_bMainWindowUsed = false;
}

void wdStandardInputDevice::InitializeDevice()
{
  if (m_uiWindowNumber == 0)
  {
    RAWINPUTDEVICE Rid[2];

    // keyboard
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x06;
    Rid[0].dwFlags = RIDEV_NOHOTKEYS; // Disables Windows-Key and Application-Key
    Rid[0].hwndTarget = nullptr;

    // mouse
    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x02;
    Rid[1].dwFlags = 0;
    Rid[1].hwndTarget = nullptr;

    if (RegisterRawInputDevices(&Rid[0], (UINT)2, sizeof(RAWINPUTDEVICE)) == FALSE)
    {
      wdLog::Error("Could not initialize RawInput for Mouse and Keyboard input.");
    }
    else
      wdLog::Success("Initialized RawInput for Mouse and Keyboard input.");
  }
  else
    wdLog::Info("Window {0} does not need to initialize Mouse or Keyboard.", m_uiWindowNumber);
}

void wdStandardInputDevice::RegisterInputSlots()
{
  RegisterInputSlot(wdInputSlot_KeyLeft, "Left", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyRight, "Right", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyUp, "Up", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyDown, "Down", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyEscape, "Escape", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeySpace, "Space", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyBackspace, "Backspace", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyReturn, "Return", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyTab, "Tab", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyLeftShift, "Left Shift", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyRightShift, "Right Shift", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyLeftCtrl, "Left Ctrl", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyRightCtrl, "Right Ctrl", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyLeftAlt, "Left Alt", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyRightAlt, "Right Alt", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyLeftWin, "Left Win", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyRightWin, "Right Win", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyBracketOpen, "[", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyBracketClose, "]", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeySemicolon, ";", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyApostrophe, "'", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeySlash, "/", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyEquals, "=", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyTilde, "~", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyHyphen, "-", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyComma, ",", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPeriod, ".", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyBackslash, "\\", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPipe, "|", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_Key1, "1", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key2, "2", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key3, "3", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key4, "4", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key5, "5", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key6, "6", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key7, "7", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key8, "8", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key9, "9", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_Key0, "0", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyNumpad1, "Numpad 1", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad2, "Numpad 2", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad3, "Numpad 3", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad4, "Numpad 4", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad5, "Numpad 5", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad6, "Numpad 6", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad7, "Numpad 7", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad8, "Numpad 8", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad9, "Numpad 9", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpad0, "Numpad 0", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyA, "A", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyB, "B", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyC, "C", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyD, "D", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyE, "E", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF, "F", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyG, "G", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyH, "H", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyI, "I", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyJ, "J", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyK, "K", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyL, "L", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyM, "M", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyN, "N", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyO, "O", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyP, "P", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyQ, "Q", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyR, "R", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyS, "S", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyT, "T", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyU, "U", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyV, "V", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyW, "W", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyX, "X", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyY, "Y", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyZ, "Z", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyF1, "F1", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF2, "F2", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF3, "F3", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF4, "F4", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF5, "F5", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF6, "F6", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF7, "F7", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF8, "F8", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF9, "F9", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF10, "F10", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF11, "F11", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyF12, "F12", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyHome, "Home", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyEnd, "End", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyDelete, "Delete", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyInsert, "Insert", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPageUp, "Page Up", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPageDown, "Page Down", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyNumLock, "Numlock", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadPlus, "Numpad +", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadMinus, "Numpad -", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadStar, "Numpad *", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadSlash, "Numpad /", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadPeriod, "Numpad .", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNumpadEnter, "Enter", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyCapsLock, "Capslock", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPrint, "Print", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyScroll, "Scroll", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPause, "Pause", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyApps, "Application", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_KeyPrevTrack, "Previous Track", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNextTrack, "Next Track", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPlayPause, "Play / Pause", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyStop, "Stop", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyVolumeUp, "Volume Up", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyVolumeDown, "Volume Down", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyMute, "Mute", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_MouseWheelUp, "Mousewheel Up", wdInputSlotFlags::IsMouseWheel);
  RegisterInputSlot(wdInputSlot_MouseWheelDown, "Mousewheel Down", wdInputSlotFlags::IsMouseWheel);

  RegisterInputSlot(wdInputSlot_MouseMoveNegX, "Mouse Move Left", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMovePosX, "Mouse Move Right", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMoveNegY, "Mouse Move Down", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMovePosY, "Mouse Move Up", wdInputSlotFlags::IsMouseAxisMove);

  RegisterInputSlot(wdInputSlot_MouseButton0, "Mousebutton 0", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton1, "Mousebutton 1", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton2, "Mousebutton 2", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton3, "Mousebutton 3", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton4, "Mousebutton 4", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_MouseDblClick0, "Left Double Click", wdInputSlotFlags::IsDoubleClick);
  RegisterInputSlot(wdInputSlot_MouseDblClick1, "Right Double Click", wdInputSlotFlags::IsDoubleClick);
  RegisterInputSlot(wdInputSlot_MouseDblClick2, "Middle Double Click", wdInputSlotFlags::IsDoubleClick);

  RegisterInputSlot(wdInputSlot_MousePositionX, "Mouse Position X", wdInputSlotFlags::IsMouseAxisPosition);
  RegisterInputSlot(wdInputSlot_MousePositionY, "Mouse Position Y", wdInputSlotFlags::IsMouseAxisPosition);


  RegisterInputSlot(wdInputSlot_TouchPoint0, "Touchpoint 1", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint0_PositionX, "Touchpoint 1 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint0_PositionY, "Touchpoint 1 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint1, "Touchpoint 2", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint1_PositionX, "Touchpoint 2 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint1_PositionY, "Touchpoint 2 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint2, "Touchpoint 3", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint2_PositionX, "Touchpoint 3 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint2_PositionY, "Touchpoint 3 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint3, "Touchpoint 4", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint3_PositionX, "Touchpoint 4 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint3_PositionY, "Touchpoint 4 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint4, "Touchpoint 5", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint4_PositionX, "Touchpoint 5 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint4_PositionY, "Touchpoint 5 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint5, "Touchpoint 6", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint5_PositionX, "Touchpoint 6 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint5_PositionY, "Touchpoint 6 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint6, "Touchpoint 7", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint6_PositionX, "Touchpoint 7 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint6_PositionY, "Touchpoint 7 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint7, "Touchpoint 8", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint7_PositionX, "Touchpoint 8 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint7_PositionY, "Touchpoint 8 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint8, "Touchpoint 9", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint8_PositionX, "Touchpoint 9 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint8_PositionY, "Touchpoint 9 Position Y", wdInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(wdInputSlot_TouchPoint9, "Touchpoint 10", wdInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(wdInputSlot_TouchPoint9_PositionX, "Touchpoint 10 Position X", wdInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(wdInputSlot_TouchPoint9_PositionY, "Touchpoint 10 Position Y", wdInputSlotFlags::IsTouchPosition);
}

void wdStandardInputDevice::ResetInputSlotValues()
{
  m_InputSlotValues[wdInputSlot_MouseWheelUp] = 0;
  m_InputSlotValues[wdInputSlot_MouseWheelDown] = 0;
  m_InputSlotValues[wdInputSlot_MouseMoveNegX] = 0;
  m_InputSlotValues[wdInputSlot_MouseMovePosX] = 0;
  m_InputSlotValues[wdInputSlot_MouseMoveNegY] = 0;
  m_InputSlotValues[wdInputSlot_MouseMovePosY] = 0;
  m_InputSlotValues[wdInputSlot_MouseDblClick0] = 0;
  m_InputSlotValues[wdInputSlot_MouseDblClick1] = 0;
  m_InputSlotValues[wdInputSlot_MouseDblClick2] = 0;
}

void wdStandardInputDevice::UpdateInputSlotValues()
{
  const char* slotDown[5] = {wdInputSlot_MouseButton0, wdInputSlot_MouseButton1, wdInputSlot_MouseButton2, wdInputSlot_MouseButton3, wdInputSlot_MouseButton4};

  // don't read uninitialized values
  if (!m_InputSlotValues.Contains(slotDown[4]))
  {
    for (int i = 0; i < 5; ++i)
    {
      m_InputSlotValues[slotDown[i]] = 0;
    }
  }

  for (int i = 0; i < 5; ++i)
  {
    if (m_InputSlotValues[slotDown[i]] > 0)
    {
      if (m_uiMouseButtonReceivedUp[i] > 0)
      {
        --m_uiMouseButtonReceivedUp[i];
        m_InputSlotValues[slotDown[i]] = 0;
      }
    }
    else
    {
      if (m_uiMouseButtonReceivedDown[i] > 0)
      {
        --m_uiMouseButtonReceivedDown[i];
        m_InputSlotValues[slotDown[i]] = 1.0f;
      }
    }
  }

  SUPER::UpdateInputSlotValues();
}

void wdStandardInputDevice::ApplyClipRect(wdMouseCursorClipMode::Enum mode, wdMinWindows::HWND hWnd)
{
  if (!m_bApplyClipRect)
    return;

  m_bApplyClipRect = false;

  if (mode == wdMouseCursorClipMode::NoClip)
  {
    ClipCursor(nullptr);
    return;
  }

  RECT r;
  {
    RECT area;
    GetClientRect(wdMinWindows::ToNative(hWnd), &area);
    POINT p0, p1;
    p0.x = 0;
    p0.y = 0;
    p1.x = area.right;
    p1.y = area.bottom;

    ClientToScreen(wdMinWindows::ToNative(hWnd), &p0);
    ClientToScreen(wdMinWindows::ToNative(hWnd), &p1);

    r.top = p0.y;
    r.left = p0.x;
    r.right = p1.x;
    r.bottom = p1.y;
  }

  if (mode == wdMouseCursorClipMode::ClipToPosition)
  {
    POINT mp;
    if (GetCursorPos(&mp))
    {
      // make sure the position is inside the window rect
      mp.x = wdMath::Clamp(mp.x, r.left, r.right);
      mp.y = wdMath::Clamp(mp.y, r.top, r.bottom);

      r.top = mp.y;
      r.bottom = mp.y;
      r.left = mp.x;
      r.right = mp.x;
    }
  }

  ClipCursor(&r);
}

void wdStandardInputDevice::SetClipMouseCursor(wdMouseCursorClipMode::Enum mode)
{
  if (m_ClipCursorMode == mode)
    return;

  m_ClipCursorMode = mode;
  m_bApplyClipRect = m_ClipCursorMode != wdMouseCursorClipMode::NoClip;

  if (m_ClipCursorMode == wdMouseCursorClipMode::NoClip)
    ClipCursor(nullptr);
}

// WM_INPUT mouse clicks do not work in some VMs.
// When this is enabled, mouse clicks are retrieved via standard WM_LBUTTONDOWN.
#define WD_MOUSEBUTTON_COMPATIBILTY_MODE WD_ON

void wdStandardInputDevice::WindowMessage(
  wdMinWindows::HWND pWnd, wdMinWindows::UINT msg, wdMinWindows::WPARAM wparam, wdMinWindows::LPARAM lparam)
{
#if WD_ENABLED(WD_MOUSEBUTTON_COMPATIBILTY_MODE)
  static wdInt32 s_iMouseCaptureCount = 0;
#endif

  switch (msg)
  {
    case WM_MOUSEWHEEL:
    {
      // The mousewheel does not work with rawinput over touchpads (at least not all)
      // So we handle that one individually

      const wdInt32 iRotated = (wdInt16)HIWORD(wparam);

      if (iRotated > 0)
        m_InputSlotValues[wdInputSlot_MouseWheelUp] = iRotated / 120.0f;
      else
        m_InputSlotValues[wdInputSlot_MouseWheelDown] = iRotated / -120.0f;

      break;
    }

    case WM_MOUSEMOVE:
    {
      RECT area;
      GetClientRect(wdMinWindows::ToNative(pWnd), &area);

      const wdUInt32 uiResX = area.right - area.left;
      const wdUInt32 uiResY = area.bottom - area.top;

      const float fPosX = (float)((short)LOWORD(lparam));
      const float fPosY = (float)((short)HIWORD(lparam));

      s_iMouseIsOverWindowNumber = m_uiWindowNumber;
      m_InputSlotValues[wdInputSlot_MousePositionX] = (fPosX / uiResX);
      m_InputSlotValues[wdInputSlot_MousePositionY] = (fPosY / uiResY);

      if (m_ClipCursorMode == wdMouseCursorClipMode::ClipToPosition || m_ClipCursorMode == wdMouseCursorClipMode::ClipToWindowImmediate)
      {
        ApplyClipRect(m_ClipCursorMode, pWnd);
      }

      break;
    }

    case WM_SETFOCUS:
    {
      m_bApplyClipRect = true;
      ApplyClipRect(m_ClipCursorMode, pWnd);
      break;
    }

    case WM_KILLFOCUS:
    {
      OnFocusLost(pWnd);
      return;
    }

    case WM_CHAR:
      m_uiLastCharacter = (wchar_t)wparam;
      return;

      // these messages would only arrive, if the window had the flag CS_DBLCLKS
      // see https://docs.microsoft.com/windows/win32/inputdev/wm-lbuttondblclk
      // this would add lag and hide single clicks when the user double clicks
      // therefore it is not used
      //case WM_LBUTTONDBLCLK:
      //  m_InputSlotValues[wdInputSlot_MouseDblClick0] = 1.0f;
      //  return;
      //case WM_RBUTTONDBLCLK:
      //  m_InputSlotValues[wdInputSlot_MouseDblClick1] = 1.0f;
      //  return;
      //case WM_MBUTTONDBLCLK:
      //  m_InputSlotValues[wdInputSlot_MouseDblClick2] = 1.0f;
      //  return;

#if WD_ENABLED(WD_MOUSEBUTTON_COMPATIBILTY_MODE)

    case WM_LBUTTONDOWN:
      m_uiMouseButtonReceivedDown[0]++;

      if (s_iMouseCaptureCount == 0)
        SetCapture(wdMinWindows::ToNative(pWnd));
      ++s_iMouseCaptureCount;


      return;

    case WM_LBUTTONUP:
      m_uiMouseButtonReceivedUp[0]++;
      ApplyClipRect(m_ClipCursorMode, pWnd);

      --s_iMouseCaptureCount;
      if (s_iMouseCaptureCount <= 0)
        ReleaseCapture();

      return;

    case WM_RBUTTONDOWN:
      m_uiMouseButtonReceivedDown[1]++;

      if (s_iMouseCaptureCount == 0)
        SetCapture(wdMinWindows::ToNative(pWnd));
      ++s_iMouseCaptureCount;

      return;

    case WM_RBUTTONUP:
      m_uiMouseButtonReceivedUp[1]++;
      ApplyClipRect(m_ClipCursorMode, pWnd);

      --s_iMouseCaptureCount;
      if (s_iMouseCaptureCount <= 0)
        ReleaseCapture();


      return;

    case WM_MBUTTONDOWN:
      m_uiMouseButtonReceivedDown[2]++;

      if (s_iMouseCaptureCount == 0)
        SetCapture(wdMinWindows::ToNative(pWnd));
      ++s_iMouseCaptureCount;
      return;

    case WM_MBUTTONUP:
      m_uiMouseButtonReceivedUp[2]++;

      --s_iMouseCaptureCount;
      if (s_iMouseCaptureCount <= 0)
        ReleaseCapture();

      return;

    case WM_XBUTTONDOWN:
      if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
        m_uiMouseButtonReceivedDown[3]++;
      if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2)
        m_uiMouseButtonReceivedDown[4]++;

      if (s_iMouseCaptureCount == 0)
        SetCapture(wdMinWindows::ToNative(pWnd));
      ++s_iMouseCaptureCount;

      return;

    case WM_XBUTTONUP:
      if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
        m_uiMouseButtonReceivedUp[3]++;
      if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2)
        m_uiMouseButtonReceivedUp[4]++;

      --s_iMouseCaptureCount;
      if (s_iMouseCaptureCount <= 0)
        ReleaseCapture();

      return;

    case WM_CAPTURECHANGED: // Sent to the window that is losing the mouse capture.
      s_iMouseCaptureCount = 0;
      return;

#else

    case WM_LBUTTONUP:
      ApplyClipRect(m_bClipCursor, hWnd);
      return;

#endif

    case WM_INPUT:
    {
      wdUInt32 uiSize = 0;

      GetRawInputData((HRAWINPUT)lparam, RID_INPUT, nullptr, &uiSize, sizeof(RAWINPUTHEADER));

      if (uiSize == 0)
        return;

      wdHybridArray<wdUInt8, sizeof(RAWINPUT)> InputData;
      InputData.SetCountUninitialized(uiSize);

      if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &InputData[0], &uiSize, sizeof(RAWINPUTHEADER)) != uiSize)
        return;

      RAWINPUT* raw = (RAWINPUT*)&InputData[0];

      if (raw->header.dwType == RIM_TYPEKEYBOARD)
      {
        static bool bIgnoreNext = false;

        if (bIgnoreNext)
        {
          bIgnoreNext = false;
          return;
        }

        static bool bWasStupidLeftShift = false;

        const wdUInt8 uiScanCode = static_cast<wdUInt8>(raw->data.keyboard.MakeCode);
        const bool bIsExtended = (raw->data.keyboard.Flags & RI_KEY_E0) != 0;

        if (uiScanCode == 42 && bIsExtended) // 42 has to be special I guess
        {
          bWasStupidLeftShift = true;
          return;
        }

        const char* szInputSlotName = wdInputManager::ConvertScanCodeToEngineName(uiScanCode, bIsExtended);

        // On Windows this only happens with the Pause key, but it will actually send the 'Right Ctrl' key value
        // so we need to fix this manually
        if (raw->data.keyboard.Flags & RI_KEY_E1)
        {
          szInputSlotName = wdInputSlot_KeyPause;
          bIgnoreNext = true;
        }

        // The Print key is sent as a two key sequence, first an 'extended left shift' and then the Numpad* key is sent
        // we ignore the first stupid shift key entirely and then modify the following Numpad* key
        // Note that the 'stupid shift' is sent along with several other keys as well (e.g. left/right/up/down arrows)
        // in these cases we can ignore them entirely, as the following key will have an unambiguous key code
        if (wdStringUtils::IsEqual(szInputSlotName, wdInputSlot_KeyNumpadStar) && bWasStupidLeftShift)
          szInputSlotName = wdInputSlot_KeyPrint;

        bWasStupidLeftShift = false;

        int iRequest = raw->data.keyboard.MakeCode << 16;

        if (raw->data.keyboard.Flags & RI_KEY_E0)
          iRequest |= 1 << 24;

        const bool bPressed = !(raw->data.keyboard.Flags & 0x01);

        m_InputSlotValues[szInputSlotName] = bPressed ? 1.0f : 0.0f;

        if ((m_InputSlotValues[wdInputSlot_KeyLeftCtrl] > 0.1f) && (m_InputSlotValues[wdInputSlot_KeyLeftAlt] > 0.1f) &&
            (m_InputSlotValues[wdInputSlot_KeyNumpadEnter] > 0.1f))
        {
          switch (GetClipMouseCursor())
          {
            case wdMouseCursorClipMode::NoClip:
              SetClipMouseCursor(wdMouseCursorClipMode::ClipToWindow);
              break;

            default:
              SetClipMouseCursor(wdMouseCursorClipMode::NoClip);
              break;
          }
        }
      }
      else if (raw->header.dwType == RIM_TYPEMOUSE)
      {
        const wdUInt32 uiButtons = raw->data.mouse.usButtonFlags;

        // "absolute" positions are only reported by devices such as Pens
        // if at all, we should handle them as touch points, not as mouse positions
        if ((raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0)
        {
          m_InputSlotValues[wdInputSlot_MouseMoveNegX] +=
            ((raw->data.mouse.lLastX < 0) ? (float)-raw->data.mouse.lLastX : 0.0f) * GetMouseSpeed().x;
          m_InputSlotValues[wdInputSlot_MouseMovePosX] +=
            ((raw->data.mouse.lLastX > 0) ? (float)raw->data.mouse.lLastX : 0.0f) * GetMouseSpeed().x;
          m_InputSlotValues[wdInputSlot_MouseMoveNegY] +=
            ((raw->data.mouse.lLastY < 0) ? (float)-raw->data.mouse.lLastY : 0.0f) * GetMouseSpeed().y;
          m_InputSlotValues[wdInputSlot_MouseMovePosY] +=
            ((raw->data.mouse.lLastY > 0) ? (float)raw->data.mouse.lLastY : 0.0f) * GetMouseSpeed().y;

// Mouse input does not always work via WM_INPUT
// e.g. some VMs don't send mouse click input via WM_INPUT when the mouse cursor is visible
// therefore in 'compatibility mode' it is just queried via standard WM_LBUTTONDOWN etc.
// to get 'high performance' mouse clicks, this code would work fine though
// but I doubt it makes much difference in latency
#if WD_DISABLED(WD_MOUSEBUTTON_COMPATIBILTY_MODE)
          for (wdInt32 mb = 0; mb < 5; ++mb)
          {
            char szTemp[32];
            wdStringUtils::snprintf(szTemp, 32, "mouse_button_%i", mb);

            if ((uiButtons & (RI_MOUSE_BUTTON_1_DOWN << (mb * 2))) != 0)
              m_InputSlotValues[szTemp] = 1.0f;

            if ((uiButtons & (RI_MOUSE_BUTTON_1_DOWN << (mb * 2 + 1))) != 0)
              m_InputSlotValues[szTemp] = 0.0f;
          }
#endif
        }
        else if ((raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) != 0)
        {
          if ((raw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) != 0)
          {
            // if this flag is set, we are getting mouse input through a remote desktop session
            // and that means we will not get any relative mouse move events, so we need to emulate them

            static const wdInt32 iVirtualDesktopW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            static const wdInt32 iVirtualDesktopH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

            static wdVec2 vLastPos(wdMath::MaxValue<float>());
            const wdVec2 vNewPos(
              (raw->data.mouse.lLastX / 65535.0f) * iVirtualDesktopW, (raw->data.mouse.lLastY / 65535.0f) * iVirtualDesktopH);

            if (vLastPos.x != wdMath::MaxValue<float>())
            {
              const wdVec2 vDiff = vNewPos - vLastPos;

              m_InputSlotValues[wdInputSlot_MouseMoveNegX] += ((vDiff.x < 0) ? (float)-vDiff.x : 0.0f) * GetMouseSpeed().x;
              m_InputSlotValues[wdInputSlot_MouseMovePosX] += ((vDiff.x > 0) ? (float)vDiff.x : 0.0f) * GetMouseSpeed().x;
              m_InputSlotValues[wdInputSlot_MouseMoveNegY] += ((vDiff.y < 0) ? (float)-vDiff.y : 0.0f) * GetMouseSpeed().y;
              m_InputSlotValues[wdInputSlot_MouseMovePosY] += ((vDiff.y > 0) ? (float)vDiff.y : 0.0f) * GetMouseSpeed().y;
            }

            vLastPos = vNewPos;
          }
          else
          {
            static int iTouchPoint = 0;
            static bool bTouchPointDown = false;

            const char* szSlot = wdInputManager::GetInputSlotTouchPoint(iTouchPoint);
            const char* szSlotX = wdInputManager::GetInputSlotTouchPointPositionX(iTouchPoint);
            const char* szSlotY = wdInputManager::GetInputSlotTouchPointPositionY(iTouchPoint);

            m_InputSlotValues[szSlotX] = (raw->data.mouse.lLastX / 65535.0f) + m_uiWindowNumber;
            m_InputSlotValues[szSlotY] = (raw->data.mouse.lLastY / 65535.0f);

            if ((uiButtons & (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_2_DOWN)) != 0)
            {
              bTouchPointDown = true;
              m_InputSlotValues[szSlot] = 1.0f;
            }

            if ((uiButtons & (RI_MOUSE_BUTTON_1_UP | RI_MOUSE_BUTTON_2_UP)) != 0)
            {
              bTouchPointDown = false;
              m_InputSlotValues[szSlot] = 0.0f;
            }
          }
        }
        else
        {
          wdLog::Info("Unknown Mouse Move: {0} | {1}, Flags = {2}", wdArgF(raw->data.mouse.lLastX, 1), wdArgF(raw->data.mouse.lLastY, 1),
            (wdUInt32)raw->data.mouse.usFlags);
        }
      }
    }
  }
}


static void SetKeyNameForScanCode(int iScanCode, bool bExtended, const char* szInputSlot)
{
  const wdUInt32 uiKeyCode = (iScanCode << 16) | (bExtended ? (1 << 24) : 0);

  wchar_t szKeyName[32] = {0};
  GetKeyNameTextW(uiKeyCode, szKeyName, 30);

  wdStringUtf8 sName(szKeyName);

  wdLog::Dev("Translated '{0}' to '{1}'", wdInputManager::GetInputSlotDisplayName(szInputSlot), sName.GetData());

  wdInputManager::SetInputSlotDisplayName(szInputSlot, sName.GetData());
}

void wdStandardInputDevice::LocalizeButtonDisplayNames()
{
  WD_LOG_BLOCK("wdStandardInputDevice::LocalizeButtonDisplayNames");

  SetKeyNameForScanCode(1, false, wdInputSlot_KeyEscape);
  SetKeyNameForScanCode(2, false, wdInputSlot_Key1);
  SetKeyNameForScanCode(3, false, wdInputSlot_Key2);
  SetKeyNameForScanCode(4, false, wdInputSlot_Key3);
  SetKeyNameForScanCode(5, false, wdInputSlot_Key4);
  SetKeyNameForScanCode(6, false, wdInputSlot_Key5);
  SetKeyNameForScanCode(7, false, wdInputSlot_Key6);
  SetKeyNameForScanCode(8, false, wdInputSlot_Key7);
  SetKeyNameForScanCode(9, false, wdInputSlot_Key8);
  SetKeyNameForScanCode(10, false, wdInputSlot_Key9);
  SetKeyNameForScanCode(11, false, wdInputSlot_Key0);

  SetKeyNameForScanCode(12, false, wdInputSlot_KeyHyphen);
  SetKeyNameForScanCode(13, false, wdInputSlot_KeyEquals);
  SetKeyNameForScanCode(14, false, wdInputSlot_KeyBackspace);

  SetKeyNameForScanCode(15, false, wdInputSlot_KeyTab);
  SetKeyNameForScanCode(16, false, wdInputSlot_KeyQ);
  SetKeyNameForScanCode(17, false, wdInputSlot_KeyW);
  SetKeyNameForScanCode(18, false, wdInputSlot_KeyE);
  SetKeyNameForScanCode(19, false, wdInputSlot_KeyR);
  SetKeyNameForScanCode(20, false, wdInputSlot_KeyT);
  SetKeyNameForScanCode(21, false, wdInputSlot_KeyY);
  SetKeyNameForScanCode(22, false, wdInputSlot_KeyU);
  SetKeyNameForScanCode(23, false, wdInputSlot_KeyI);
  SetKeyNameForScanCode(24, false, wdInputSlot_KeyO);
  SetKeyNameForScanCode(25, false, wdInputSlot_KeyP);
  SetKeyNameForScanCode(26, false, wdInputSlot_KeyBracketOpen);
  SetKeyNameForScanCode(27, false, wdInputSlot_KeyBracketClose);
  SetKeyNameForScanCode(28, false, wdInputSlot_KeyReturn);

  SetKeyNameForScanCode(29, false, wdInputSlot_KeyLeftCtrl);
  SetKeyNameForScanCode(30, false, wdInputSlot_KeyA);
  SetKeyNameForScanCode(31, false, wdInputSlot_KeyS);
  SetKeyNameForScanCode(32, false, wdInputSlot_KeyD);
  SetKeyNameForScanCode(33, false, wdInputSlot_KeyF);
  SetKeyNameForScanCode(34, false, wdInputSlot_KeyG);
  SetKeyNameForScanCode(35, false, wdInputSlot_KeyH);
  SetKeyNameForScanCode(36, false, wdInputSlot_KeyJ);
  SetKeyNameForScanCode(37, false, wdInputSlot_KeyK);
  SetKeyNameForScanCode(38, false, wdInputSlot_KeyL);
  SetKeyNameForScanCode(39, false, wdInputSlot_KeySemicolon);
  SetKeyNameForScanCode(40, false, wdInputSlot_KeyApostrophe);

  SetKeyNameForScanCode(41, false, wdInputSlot_KeyTilde);
  SetKeyNameForScanCode(42, false, wdInputSlot_KeyLeftShift);
  SetKeyNameForScanCode(43, false, wdInputSlot_KeyBackslash);

  SetKeyNameForScanCode(44, false, wdInputSlot_KeyZ);
  SetKeyNameForScanCode(45, false, wdInputSlot_KeyX);
  SetKeyNameForScanCode(46, false, wdInputSlot_KeyC);
  SetKeyNameForScanCode(47, false, wdInputSlot_KeyV);
  SetKeyNameForScanCode(48, false, wdInputSlot_KeyB);
  SetKeyNameForScanCode(49, false, wdInputSlot_KeyN);
  SetKeyNameForScanCode(50, false, wdInputSlot_KeyM);
  SetKeyNameForScanCode(51, false, wdInputSlot_KeyComma);
  SetKeyNameForScanCode(52, false, wdInputSlot_KeyPeriod);
  SetKeyNameForScanCode(53, false, wdInputSlot_KeySlash);
  SetKeyNameForScanCode(54, false, wdInputSlot_KeyRightShift);

  SetKeyNameForScanCode(55, false, wdInputSlot_KeyNumpadStar); // Overlaps with Print

  SetKeyNameForScanCode(56, false, wdInputSlot_KeyLeftAlt);
  SetKeyNameForScanCode(57, false, wdInputSlot_KeySpace);
  SetKeyNameForScanCode(58, false, wdInputSlot_KeyCapsLock);

  SetKeyNameForScanCode(59, false, wdInputSlot_KeyF1);
  SetKeyNameForScanCode(60, false, wdInputSlot_KeyF2);
  SetKeyNameForScanCode(61, false, wdInputSlot_KeyF3);
  SetKeyNameForScanCode(62, false, wdInputSlot_KeyF4);
  SetKeyNameForScanCode(63, false, wdInputSlot_KeyF5);
  SetKeyNameForScanCode(64, false, wdInputSlot_KeyF6);
  SetKeyNameForScanCode(65, false, wdInputSlot_KeyF7);
  SetKeyNameForScanCode(66, false, wdInputSlot_KeyF8);
  SetKeyNameForScanCode(67, false, wdInputSlot_KeyF9);
  SetKeyNameForScanCode(68, false, wdInputSlot_KeyF10);

  SetKeyNameForScanCode(69, true, wdInputSlot_KeyNumLock); // Prints 'Pause' if it is not 'extended'
  SetKeyNameForScanCode(70, false, wdInputSlot_KeyScroll); // This overlaps with Pause

  SetKeyNameForScanCode(71, false, wdInputSlot_KeyNumpad7); // This overlaps with Home
  SetKeyNameForScanCode(72, false, wdInputSlot_KeyNumpad8); // This overlaps with Arrow Up
  SetKeyNameForScanCode(73, false, wdInputSlot_KeyNumpad9); // This overlaps with Page Up
  SetKeyNameForScanCode(74, false, wdInputSlot_KeyNumpadMinus);

  SetKeyNameForScanCode(75, false, wdInputSlot_KeyNumpad4); // This overlaps with Arrow Left
  SetKeyNameForScanCode(76, false, wdInputSlot_KeyNumpad5);
  SetKeyNameForScanCode(77, false, wdInputSlot_KeyNumpad6); // This overlaps with Arrow Right
  SetKeyNameForScanCode(78, false, wdInputSlot_KeyNumpadPlus);

  SetKeyNameForScanCode(79, false, wdInputSlot_KeyNumpad1);      // This overlaps with End
  SetKeyNameForScanCode(80, false, wdInputSlot_KeyNumpad2);      // This overlaps with Arrow Down
  SetKeyNameForScanCode(81, false, wdInputSlot_KeyNumpad3);      // This overlaps with Page Down
  SetKeyNameForScanCode(82, false, wdInputSlot_KeyNumpad0);      // This overlaps with Insert
  SetKeyNameForScanCode(83, false, wdInputSlot_KeyNumpadPeriod); // This overlaps with Insert

  SetKeyNameForScanCode(86, false, wdInputSlot_KeyPipe);

  SetKeyNameForScanCode(87, false, "keyboard_f11");
  SetKeyNameForScanCode(88, false, "keyboard_f12");

  SetKeyNameForScanCode(91, true, wdInputSlot_KeyLeftWin);  // Prints '' if it is not 'extended'
  SetKeyNameForScanCode(92, true, wdInputSlot_KeyRightWin); // Prints '' if it is not 'extended'
  SetKeyNameForScanCode(93, true, wdInputSlot_KeyApps);     // Prints '' if it is not 'extended'

  // 'Extended' keys
  SetKeyNameForScanCode(28, true, wdInputSlot_KeyNumpadEnter);
  SetKeyNameForScanCode(29, true, wdInputSlot_KeyRightCtrl);
  SetKeyNameForScanCode(53, true, wdInputSlot_KeyNumpadSlash);
  SetKeyNameForScanCode(55, true, wdInputSlot_KeyPrint);
  SetKeyNameForScanCode(56, true, wdInputSlot_KeyRightAlt);
  SetKeyNameForScanCode(70, true, wdInputSlot_KeyPause);
  SetKeyNameForScanCode(71, true, wdInputSlot_KeyHome);
  SetKeyNameForScanCode(72, true, wdInputSlot_KeyUp);
  SetKeyNameForScanCode(73, true, wdInputSlot_KeyPageUp);

  SetKeyNameForScanCode(75, true, wdInputSlot_KeyLeft);
  SetKeyNameForScanCode(77, true, wdInputSlot_KeyRight);

  SetKeyNameForScanCode(79, true, wdInputSlot_KeyEnd);
  SetKeyNameForScanCode(80, true, wdInputSlot_KeyDown);
  SetKeyNameForScanCode(81, true, wdInputSlot_KeyPageDown);
  SetKeyNameForScanCode(82, true, wdInputSlot_KeyInsert);
  SetKeyNameForScanCode(83, true, wdInputSlot_KeyDelete);
}

void wdStandardInputDevice::SetShowMouseCursor(bool bShow)
{
  if (m_bShowCursor == bShow)
    return;

  m_bShowCursor = bShow;
  ShowCursor(m_bShowCursor);
}

bool wdStandardInputDevice::GetShowMouseCursor() const
{
  return m_bShowCursor;
}

void wdStandardInputDevice::OnFocusLost(wdMinWindows::HWND hWnd)
{
  m_bApplyClipRect = true;
  ApplyClipRect(wdMouseCursorClipMode::NoClip, hWnd);

  auto it = m_InputSlotValues.GetIterator();

  while (it.IsValid())
  {
    it.Value() = 0.0f;
    it.Next();
  }


  const char* slotDown[5] = {wdInputSlot_MouseButton0, wdInputSlot_MouseButton1, wdInputSlot_MouseButton2, wdInputSlot_MouseButton3, wdInputSlot_MouseButton4};

  static_assert(WD_ARRAY_SIZE(m_uiMouseButtonReceivedDown) == WD_ARRAY_SIZE(slotDown));

  for (int i = 0; i < WD_ARRAY_SIZE(m_uiMouseButtonReceivedDown); ++i)
  {
    m_uiMouseButtonReceivedDown[i] = 0;
    m_uiMouseButtonReceivedUp[i] = 0;

    m_InputSlotValues[slotDown[i]] = 0;
  }
}
