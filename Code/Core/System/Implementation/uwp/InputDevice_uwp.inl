#include <Core/Input/InputManager.h>
#include <Core/System/Implementation/uwp/InputDevice_uwp.h>
#include <Foundation/Basics/Platform/uwp/UWPUtils.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/StringConversion.h>
#include <wrl/event.h>

using namespace ABI::Windows::UI::Core;

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdStandardInputDevice, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdStandardInputDevice::wdStandardInputDevice(ICoreWindow* coreWindow)
  : m_coreWindow(coreWindow)
{
  // TODO
  m_ClipCursorMode = wdMouseCursorClipMode::NoClip;
  m_bShowCursor = true;
}

wdStandardInputDevice::~wdStandardInputDevice()
{
  if (m_coreWindow)
  {
    m_coreWindow->remove_KeyDown(m_eventRegistration_keyDown);
    m_coreWindow->remove_KeyUp(m_eventRegistration_keyUp);
    m_coreWindow->remove_CharacterReceived(m_eventRegistration_characterReceived);
    m_coreWindow->remove_PointerMoved(m_eventRegistration_pointerMoved);
    m_coreWindow->remove_PointerEntered(m_eventRegistration_pointerEntered);
    m_coreWindow->remove_PointerExited(m_eventRegistration_pointerExited);
    m_coreWindow->remove_PointerCaptureLost(m_eventRegistration_pointerCaptureLost);
    m_coreWindow->remove_PointerPressed(m_eventRegistration_pointerPressed);
    m_coreWindow->remove_PointerReleased(m_eventRegistration_pointerReleased);
    m_coreWindow->remove_PointerWheelChanged(m_eventRegistration_pointerWheelChanged);
  }

  if (m_mouseDevice)
  {
    m_mouseDevice->remove_MouseMoved(m_eventRegistration_mouseMoved);
  }
}

void wdStandardInputDevice::InitializeDevice()
{
  using KeyHandler = __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CKeyEventArgs;
  using CharacterReceivedHandler =
    __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CCharacterReceivedEventArgs;
  using PointerHander = __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs;

  // Keyboard
  m_coreWindow->add_KeyDown(Callback<KeyHandler>(this, &wdStandardInputDevice::OnKeyEvent).Get(), &m_eventRegistration_keyDown);
  m_coreWindow->add_KeyUp(Callback<KeyHandler>(this, &wdStandardInputDevice::OnKeyEvent).Get(), &m_eventRegistration_keyUp);
  m_coreWindow->add_CharacterReceived(Callback<CharacterReceivedHandler>(this, &wdStandardInputDevice::OnCharacterReceived).Get(),
    &m_eventRegistration_characterReceived);

  // Pointer
  // Note that a pointer may be mouse, pen/stylus or touch!
  // We bundle move/press/enter all in a single callback to update all pointer state - all these cases have in common that pen/touch is
  // pressed now.
  m_coreWindow->add_PointerMoved(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerMovePressEnter).Get(),
    &m_eventRegistration_pointerMoved);
  m_coreWindow->add_PointerEntered(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerMovePressEnter).Get(),
    &m_eventRegistration_pointerEntered);
  m_coreWindow->add_PointerPressed(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerMovePressEnter).Get(),
    &m_eventRegistration_pointerPressed);
  // Changes in the pointer wheel:
  m_coreWindow->add_PointerWheelChanged(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerWheelChange).Get(),
    &m_eventRegistration_pointerWheelChanged);
  // Exit for touch or stylus means that we no longer have a press.
  // However, we presserve mouse button presses.
  m_coreWindow->add_PointerExited(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerReleasedOrExited).Get(),
    &m_eventRegistration_pointerExited);
  m_coreWindow->add_PointerReleased(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerReleasedOrExited).Get(),
    &m_eventRegistration_pointerReleased);
  // Capture loss.
  // From documentation "Occurs when a pointer moves to another app. This event is raised after PointerExited and is the final event
  // received by the app for this pointer." If this happens we want to release all mouse buttons as well.
  m_coreWindow->add_PointerCaptureLost(Callback<PointerHander>(this, &wdStandardInputDevice::OnPointerCaptureLost).Get(),
    &m_eventRegistration_pointerCaptureLost);


  // Mouse
  // The only thing that we get from the MouseDevice class is mouse moved which gives us unfiltered relative mouse position.
  // Everything else is done by WinRt's "Pointer"
  // https://docs.microsoft.com/uwp/api/windows.devices.input.mousedevice
  // Relevant article for mouse move:
  // https://docs.microsoft.com/windows/uwp/gaming/relative-mouse-movement
  {
    ComPtr<ABI::Windows::Devices::Input::IMouseDeviceStatics> mouseDeviceStatics;
    if (SUCCEEDED(ABI::Windows::Foundation::GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Input_MouseDevice).Get(),
          &mouseDeviceStatics)))
    {
      if (SUCCEEDED(mouseDeviceStatics->GetForCurrentView(&m_mouseDevice)))
      {
        using MouseMovedHandler =
          __FITypedEventHandler_2_Windows__CDevices__CInput__CMouseDevice_Windows__CDevices__CInput__CMouseEventArgs;
        m_mouseDevice->add_MouseMoved(Callback<MouseMovedHandler>(this, &wdStandardInputDevice::OnMouseMoved).Get(),
          &m_eventRegistration_mouseMoved);
      }
    }
  }
}

HRESULT wdStandardInputDevice::OnKeyEvent(ICoreWindow* coreWindow, IKeyEventArgs* args)
{
  // Closely related to the RawInput implementation in Win32/InputDevice_win32.inl

  CorePhysicalKeyStatus keyStatus;
  WD_SUCCEED_OR_RETURN(args->get_KeyStatus(&keyStatus));

  static bool bWasStupidLeftShift = false;

  if (keyStatus.ScanCode == 42 && keyStatus.IsExtendedKey) // 42 has to be special I guess
  {
    bWasStupidLeftShift = true;
    return S_OK;
  }

  const char* szInputSlotName = wdInputManager::ConvertScanCodeToEngineName(static_cast<wdUInt8>(keyStatus.ScanCode), keyStatus.IsExtendedKey == TRUE);
  if (!szInputSlotName)
    return S_OK;


  // Don't know yet how to handle this in UWP:

  // On Windows this only happens with the Pause key, but it will actually send the 'Right Ctrl' key value
  // so we need to fix this manually
  // if (raw->data.keyboard.Flags & RI_KEY_E1)
  //{
  //  szInputSlotName = wdInputSlot_KeyPause;
  //  bIgnoreNext = true;
  //}


  // The Print key is sent as a two key sequence, first an 'extended left shift' and then the Numpad* key is sent
  // we ignore the first stupid shift key entirely and then modify the following Numpad* key
  // Note that the 'stupid shift' is sent along with several other keys as well (e.g. left/right/up/down arrows)
  // in these cases we can ignore them entirely, as the following key will have an unambiguous key code
  if (wdStringUtils::IsEqual(szInputSlotName, wdInputSlot_KeyNumpadStar) && bWasStupidLeftShift)
    szInputSlotName = wdInputSlot_KeyPrint;

  bWasStupidLeftShift = false;

  m_InputSlotValues[szInputSlotName] = keyStatus.IsKeyReleased ? 0.0f : 1.0f;

  return S_OK;
}

HRESULT wdStandardInputDevice::OnCharacterReceived(ICoreWindow* coreWindow, ICharacterReceivedEventArgs* args)
{
  UINT32 keyCode = 0;
  WD_SUCCEED_OR_RETURN(args->get_KeyCode(&keyCode));
  m_uiLastCharacter = keyCode;

  return S_OK;
}

HRESULT wdStandardInputDevice::OnPointerMovePressEnter(ICoreWindow* coreWindow, IPointerEventArgs* args)
{
  using namespace ABI::Windows::Devices::Input;

  ComPtr<ABI::Windows::UI::Input::IPointerPoint> pointerPoint;
  WD_SUCCEED_OR_RETURN(args->get_CurrentPoint(&pointerPoint));
  ComPtr<IPointerDevice> pointerDevice;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerDevice(&pointerDevice));
  PointerDeviceType deviceType;
  WD_SUCCEED_OR_RETURN(pointerDevice->get_PointerDeviceType(&deviceType));

  // Pointer position.
  // From the documentation: "The position of the pointer in device-independent pixel (DIP)."
  // Note also, that there is "raw position" which may be free of pointer prediction etc.
  ABI::Windows::Foundation::Point pointerPosition;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_Position(&pointerPosition));
  ABI::Windows::Foundation::Rect windowRectangle;
  WD_SUCCEED_OR_RETURN(coreWindow->get_Bounds(&windowRectangle)); // Bounds are in DIP as well!

  float relativePosX = static_cast<float>(pointerPosition.X) / windowRectangle.Width;
  float relativePosY = static_cast<float>(pointerPosition.Y) / windowRectangle.Height;

  if (deviceType == PointerDeviceType_Mouse)
  {
    // TODO
    // RegisterInputSlot(wdInputSlot_MouseDblClick0, "Left Double Click", wdInputSlotFlags::IsDoubleClick);
    // RegisterInputSlot(wdInputSlot_MouseDblClick1, "Right Double Click", wdInputSlotFlags::IsDoubleClick);
    // RegisterInputSlot(wdInputSlot_MouseDblClick2, "Middle Double Click", wdInputSlotFlags::IsDoubleClick);

    s_iMouseIsOverWindowNumber = 0;
    m_InputSlotValues[wdInputSlot_MousePositionX] = relativePosX;
    m_InputSlotValues[wdInputSlot_MousePositionY] = relativePosY;

    WD_SUCCEED_OR_RETURN(UpdateMouseButtonStates(pointerPoint.Get()));
  }
  else // Touch AND Pen
  {
    // WinRT treats each touch point as unique pointer.
    UINT32 pointerId;
    WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerId(&pointerId));
    if (pointerId > 9)
      return S_OK;

    // All callbacks we subscribed this event to imply that a touch occurs right now.
    m_InputSlotValues[wdInputManager::GetInputSlotTouchPoint(pointerId)] = 1.0f; // Touch strength?
    m_InputSlotValues[wdInputManager::GetInputSlotTouchPointPositionX(pointerId)] = relativePosX;
    m_InputSlotValues[wdInputManager::GetInputSlotTouchPointPositionY(pointerId)] = relativePosY;
  }

  return S_OK;
}

HRESULT wdStandardInputDevice::OnPointerWheelChange(ICoreWindow* coreWindow, IPointerEventArgs* args)
{
  using namespace ABI::Windows::Devices::Input;

  ComPtr<ABI::Windows::UI::Input::IPointerPoint> pointerPoint;
  WD_SUCCEED_OR_RETURN(args->get_CurrentPoint(&pointerPoint));
  ComPtr<IPointerDevice> pointerDevice;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerDevice(&pointerDevice));

  // Only interested in mouse devices.
  PointerDeviceType deviceType;
  WD_SUCCEED_OR_RETURN(pointerDevice->get_PointerDeviceType(&deviceType));
  if (deviceType == PointerDeviceType_Mouse)
  {
    ComPtr<ABI::Windows::UI::Input::IPointerPointProperties> properties;
    WD_SUCCEED_OR_RETURN(pointerPoint->get_Properties(&properties));

    // .. and only vertical wheels.
    boolean isHorizontalWheel;
    WD_SUCCEED_OR_RETURN(properties->get_IsHorizontalMouseWheel(&isHorizontalWheel));
    if (!isHorizontalWheel)
    {
      INT32 delta;
      WD_SUCCEED_OR_RETURN(properties->get_MouseWheelDelta(&delta));

      if (delta > 0)
        m_InputSlotValues[wdInputSlot_MouseWheelUp] = delta / 120.0f;
      else
        m_InputSlotValues[wdInputSlot_MouseWheelDown] = -delta / 120.0f;
    }
  }

  return S_OK;
}

HRESULT wdStandardInputDevice::OnPointerReleasedOrExited(ICoreWindow* coreWindow, IPointerEventArgs* args)
{
  using namespace ABI::Windows::Devices::Input;

  ComPtr<ABI::Windows::UI::Input::IPointerPoint> pointerPoint;
  WD_SUCCEED_OR_RETURN(args->get_CurrentPoint(&pointerPoint));
  ComPtr<IPointerDevice> pointerDevice;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerDevice(&pointerDevice));
  PointerDeviceType deviceType;
  WD_SUCCEED_OR_RETURN(pointerDevice->get_PointerDeviceType(&deviceType));

  if (deviceType == PointerDeviceType_Mouse)
  {
    // Note that the relased event is only fired if the last mouse button is released according to documentation.
    // However, we're also subscribing to exit and depending on the mouse capture this may or may not be a button release.
    WD_SUCCEED_OR_RETURN(UpdateMouseButtonStates(pointerPoint.Get()));
  }
  else // Touch AND Pen
  {
    // WinRT treats each touch point as unique pointer.
    UINT32 pointerId;
    WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerId(&pointerId));
    if (pointerId > 9)
      return S_OK;

    m_InputSlotValues[wdInputManager::GetInputSlotTouchPoint(pointerId)] = 0.0f;
  }

  return S_OK;
}

HRESULT wdStandardInputDevice::OnPointerCaptureLost(ICoreWindow* coreWindow, IPointerEventArgs* args)
{
  using namespace ABI::Windows::Devices::Input;

  ComPtr<ABI::Windows::UI::Input::IPointerPoint> pointerPoint;
  WD_SUCCEED_OR_RETURN(args->get_CurrentPoint(&pointerPoint));
  ComPtr<IPointerDevice> pointerDevice;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerDevice(&pointerDevice));
  PointerDeviceType deviceType;
  WD_SUCCEED_OR_RETURN(pointerDevice->get_PointerDeviceType(&deviceType));

  if (deviceType == PointerDeviceType_Mouse)
  {
    m_InputSlotValues[wdInputSlot_MouseButton0] = 0.0f;
    m_InputSlotValues[wdInputSlot_MouseButton1] = 0.0f;
    m_InputSlotValues[wdInputSlot_MouseButton2] = 0.0f;
    m_InputSlotValues[wdInputSlot_MouseButton3] = 0.0f;
    m_InputSlotValues[wdInputSlot_MouseButton4] = 0.0f;
  }
  else // Touch AND Pen
  {
    // WinRT treats each touch point as unique pointer.
    UINT32 pointerId;
    WD_SUCCEED_OR_RETURN(pointerPoint->get_PointerId(&pointerId));
    if (pointerId > 9)
      return S_OK;

    m_InputSlotValues[wdInputManager::GetInputSlotTouchPoint(pointerId)] = 0.0f;
  }

  return S_OK;
}

HRESULT wdStandardInputDevice::OnMouseMoved(ABI::Windows::Devices::Input::IMouseDevice* mouseDevice,
  ABI::Windows::Devices::Input::IMouseEventArgs* args)
{
  ABI::Windows::Devices::Input::MouseDelta mouseDelta;
  WD_SUCCEED_OR_RETURN(args->get_MouseDelta(&mouseDelta));

  m_InputSlotValues[wdInputSlot_MouseMoveNegX] += ((mouseDelta.X < 0) ? static_cast<float>(-mouseDelta.X) : 0.0f) * GetMouseSpeed().x;
  m_InputSlotValues[wdInputSlot_MouseMovePosX] += ((mouseDelta.X > 0) ? static_cast<float>(mouseDelta.X) : 0.0f) * GetMouseSpeed().x;
  m_InputSlotValues[wdInputSlot_MouseMoveNegY] += ((mouseDelta.Y < 0) ? static_cast<float>(-mouseDelta.Y) : 0.0f) * GetMouseSpeed().y;
  m_InputSlotValues[wdInputSlot_MouseMovePosY] += ((mouseDelta.Y > 0) ? static_cast<float>(mouseDelta.Y) : 0.0f) * GetMouseSpeed().y;

  return S_OK;
}

HRESULT wdStandardInputDevice::UpdateMouseButtonStates(ABI::Windows::UI::Input::IPointerPoint* pointerPoint)
{
  ComPtr<ABI::Windows::UI::Input::IPointerPointProperties> properties;
  WD_SUCCEED_OR_RETURN(pointerPoint->get_Properties(&properties));

  boolean isPressed;
  WD_SUCCEED_OR_RETURN(properties->get_IsLeftButtonPressed(&isPressed));
  m_InputSlotValues[wdInputSlot_MouseButton0] = isPressed ? 1.0f : 0.0f;
  WD_SUCCEED_OR_RETURN(properties->get_IsRightButtonPressed(&isPressed));
  m_InputSlotValues[wdInputSlot_MouseButton1] = isPressed ? 1.0f : 0.0f;
  WD_SUCCEED_OR_RETURN(properties->get_IsMiddleButtonPressed(&isPressed));
  m_InputSlotValues[wdInputSlot_MouseButton2] = isPressed ? 1.0f : 0.0f;
  WD_SUCCEED_OR_RETURN(properties->get_IsXButton1Pressed(&isPressed));
  m_InputSlotValues[wdInputSlot_MouseButton3] = isPressed ? 1.0f : 0.0f;
  WD_SUCCEED_OR_RETURN(properties->get_IsXButton2Pressed(&isPressed));
  m_InputSlotValues[wdInputSlot_MouseButton4] = isPressed ? 1.0f : 0.0f;

  return S_OK;
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

  // RegisterInputSlot(wdInputSlot_KeyPrevTrack, "Previous Track", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyNextTrack, "Next Track", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyPlayPause, "Play / Pause", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyStop, "Stop", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyVolumeUp, "Volume Up", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyVolumeDown, "Volume Down", wdInputSlotFlags::IsButton);
  // RegisterInputSlot(wdInputSlot_KeyMute, "Mute", wdInputSlotFlags::IsButton);

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


  // Not yet supported
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

void SetClipRect(bool bClip, HWND hWnd)
{
  // NOT IMPLEMENTED. TODO
}

void wdStandardInputDevice::SetClipMouseCursor(wdMouseCursorClipMode::Enum mode)
{
  if (m_ClipCursorMode == mode)
    return;

  if (mode != wdMouseCursorClipMode::NoClip)
    m_coreWindow->SetPointerCapture();
  else
    m_coreWindow->ReleasePointerCapture();

  m_ClipCursorMode = mode;
}

void wdStandardInputDevice::SetShowMouseCursor(bool bShow)
{
  if (m_bShowCursor == bShow)
    return;

  // Hide
  if (!bShow)
  {
    // Save cursor to reinstantiate it.
    m_coreWindow->get_PointerCursor(&m_cursorBeforeHide);
    m_coreWindow->put_PointerCursor(nullptr);
  }

  // Show
  else
  {
    WD_ASSERT_DEV(m_cursorBeforeHide, "There should be a ICoreCursor backup that can be put back.");
    m_coreWindow->put_PointerCursor(m_cursorBeforeHide.Get());
  }

  m_bShowCursor = bShow;
}

bool wdStandardInputDevice::GetShowMouseCursor() const
{
  return m_bShowCursor;
}
