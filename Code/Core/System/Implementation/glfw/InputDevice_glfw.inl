#include <Core/System/Implementation/glfw/InputDevice_glfw.h>
#include <GLFW/glfw3.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdStandardInputDevice, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

namespace
{
  const char* ConvertGLFWKeyToEngineName(int key)
  {
    switch (key)
    {
      case GLFW_KEY_LEFT:
        return wdInputSlot_KeyLeft;
      case GLFW_KEY_RIGHT:
        return wdInputSlot_KeyRight;
      case GLFW_KEY_UP:
        return wdInputSlot_KeyUp;
      case GLFW_KEY_DOWN:
        return wdInputSlot_KeyDown;
      case GLFW_KEY_ESCAPE:
        return wdInputSlot_KeyEscape;
      case GLFW_KEY_SPACE:
        return wdInputSlot_KeySpace;
      case GLFW_KEY_BACKSPACE:
        return wdInputSlot_KeyBackspace;
      case GLFW_KEY_ENTER:
        return wdInputSlot_KeyReturn;
      case GLFW_KEY_TAB:
        return wdInputSlot_KeyTab;
      case GLFW_KEY_LEFT_SHIFT:
        return wdInputSlot_KeyLeftShift;
      case GLFW_KEY_RIGHT_SHIFT:
        return wdInputSlot_KeyRightShift;
      case GLFW_KEY_LEFT_CONTROL:
        return wdInputSlot_KeyLeftCtrl;
      case GLFW_KEY_RIGHT_CONTROL:
        return wdInputSlot_KeyRightCtrl;
      case GLFW_KEY_LEFT_ALT:
        return wdInputSlot_KeyLeftAlt;
      case GLFW_KEY_RIGHT_ALT:
        return wdInputSlot_KeyRightAlt;
      case GLFW_KEY_LEFT_SUPER:
        return wdInputSlot_KeyLeftWin;
      case GLFW_KEY_RIGHT_SUPER:
        return wdInputSlot_KeyRightWin;
      case GLFW_KEY_MENU:
        return wdInputSlot_KeyApps;
      case GLFW_KEY_LEFT_BRACKET:
        return wdInputSlot_KeyBracketOpen;
      case GLFW_KEY_RIGHT_BRACKET:
        return wdInputSlot_KeyBracketClose;
      case GLFW_KEY_SEMICOLON:
        return wdInputSlot_KeySemicolon;
      case GLFW_KEY_APOSTROPHE:
        return wdInputSlot_KeyApostrophe;
      case GLFW_KEY_SLASH:
        return wdInputSlot_KeySlash;
      case GLFW_KEY_EQUAL:
        return wdInputSlot_KeyEquals;
      case GLFW_KEY_GRAVE_ACCENT:
        return wdInputSlot_KeyTilde;
      case GLFW_KEY_MINUS:
        return wdInputSlot_KeyHyphen;
      case GLFW_KEY_COMMA:
        return wdInputSlot_KeyComma;
      case GLFW_KEY_PERIOD:
        return wdInputSlot_KeyPeriod;
      case GLFW_KEY_BACKSLASH:
        return wdInputSlot_KeyBackslash;
      case GLFW_KEY_WORLD_1:
        return wdInputSlot_KeyPipe;
      case GLFW_KEY_1:
        return wdInputSlot_Key1;
      case GLFW_KEY_2:
        return wdInputSlot_Key2;
      case GLFW_KEY_3:
        return wdInputSlot_Key3;
      case GLFW_KEY_4:
        return wdInputSlot_Key4;
      case GLFW_KEY_5:
        return wdInputSlot_Key5;
      case GLFW_KEY_6:
        return wdInputSlot_Key6;
      case GLFW_KEY_7:
        return wdInputSlot_Key7;
      case GLFW_KEY_8:
        return wdInputSlot_Key8;
      case GLFW_KEY_9:
        return wdInputSlot_Key9;
      case GLFW_KEY_0:
        return wdInputSlot_Key0;
      case GLFW_KEY_KP_1:
        return wdInputSlot_KeyNumpad1;
      case GLFW_KEY_KP_2:
        return wdInputSlot_KeyNumpad2;
      case GLFW_KEY_KP_3:
        return wdInputSlot_KeyNumpad3;
      case GLFW_KEY_KP_4:
        return wdInputSlot_KeyNumpad4;
      case GLFW_KEY_KP_5:
        return wdInputSlot_KeyNumpad5;
      case GLFW_KEY_KP_6:
        return wdInputSlot_KeyNumpad6;
      case GLFW_KEY_KP_7:
        return wdInputSlot_KeyNumpad7;
      case GLFW_KEY_KP_8:
        return wdInputSlot_KeyNumpad8;
      case GLFW_KEY_KP_9:
        return wdInputSlot_KeyNumpad9;
      case GLFW_KEY_KP_0:
        return wdInputSlot_KeyNumpad0;
      case GLFW_KEY_A:
        return wdInputSlot_KeyA;
      case GLFW_KEY_B:
        return wdInputSlot_KeyB;
      case GLFW_KEY_C:
        return wdInputSlot_KeyC;
      case GLFW_KEY_D:
        return wdInputSlot_KeyD;
      case GLFW_KEY_E:
        return wdInputSlot_KeyE;
      case GLFW_KEY_F:
        return wdInputSlot_KeyF;
      case GLFW_KEY_G:
        return wdInputSlot_KeyG;
      case GLFW_KEY_H:
        return wdInputSlot_KeyH;
      case GLFW_KEY_I:
        return wdInputSlot_KeyI;
      case GLFW_KEY_J:
        return wdInputSlot_KeyJ;
      case GLFW_KEY_K:
        return wdInputSlot_KeyK;
      case GLFW_KEY_L:
        return wdInputSlot_KeyL;
      case GLFW_KEY_M:
        return wdInputSlot_KeyM;
      case GLFW_KEY_N:
        return wdInputSlot_KeyN;
      case GLFW_KEY_O:
        return wdInputSlot_KeyO;
      case GLFW_KEY_P:
        return wdInputSlot_KeyP;
      case GLFW_KEY_Q:
        return wdInputSlot_KeyQ;
      case GLFW_KEY_R:
        return wdInputSlot_KeyR;
      case GLFW_KEY_S:
        return wdInputSlot_KeyS;
      case GLFW_KEY_T:
        return wdInputSlot_KeyT;
      case GLFW_KEY_U:
        return wdInputSlot_KeyU;
      case GLFW_KEY_V:
        return wdInputSlot_KeyV;
      case GLFW_KEY_W:
        return wdInputSlot_KeyW;
      case GLFW_KEY_X:
        return wdInputSlot_KeyX;
      case GLFW_KEY_Y:
        return wdInputSlot_KeyY;
      case GLFW_KEY_Z:
        return wdInputSlot_KeyZ;
      case GLFW_KEY_F1:
        return wdInputSlot_KeyF1;
      case GLFW_KEY_F2:
        return wdInputSlot_KeyF2;
      case GLFW_KEY_F3:
        return wdInputSlot_KeyF3;
      case GLFW_KEY_F4:
        return wdInputSlot_KeyF4;
      case GLFW_KEY_F5:
        return wdInputSlot_KeyF5;
      case GLFW_KEY_F6:
        return wdInputSlot_KeyF6;
      case GLFW_KEY_F7:
        return wdInputSlot_KeyF7;
      case GLFW_KEY_F8:
        return wdInputSlot_KeyF8;
      case GLFW_KEY_F9:
        return wdInputSlot_KeyF9;
      case GLFW_KEY_F10:
        return wdInputSlot_KeyF10;
      case GLFW_KEY_F11:
        return wdInputSlot_KeyF11;
      case GLFW_KEY_F12:
        return wdInputSlot_KeyF12;
      case GLFW_KEY_HOME:
        return wdInputSlot_KeyHome;
      case GLFW_KEY_END:
        return wdInputSlot_KeyEnd;
      case GLFW_KEY_DELETE:
        return wdInputSlot_KeyDelete;
      case GLFW_KEY_INSERT:
        return wdInputSlot_KeyInsert;
      case GLFW_KEY_PAGE_UP:
        return wdInputSlot_KeyPageUp;
      case GLFW_KEY_PAGE_DOWN:
        return wdInputSlot_KeyPageDown;
      case GLFW_KEY_NUM_LOCK:
        return wdInputSlot_KeyNumLock;
      case GLFW_KEY_KP_ADD:
        return wdInputSlot_KeyNumpadPlus;
      case GLFW_KEY_KP_SUBTRACT:
        return wdInputSlot_KeyNumpadMinus;
      case GLFW_KEY_KP_MULTIPLY:
        return wdInputSlot_KeyNumpadStar;
      case GLFW_KEY_KP_DIVIDE:
        return wdInputSlot_KeyNumpadSlash;
      case GLFW_KEY_KP_DECIMAL:
        return wdInputSlot_KeyNumpadPeriod;
      case GLFW_KEY_KP_ENTER:
        return wdInputSlot_KeyNumpadEnter;
      case GLFW_KEY_CAPS_LOCK:
        return wdInputSlot_KeyCapsLock;
      case GLFW_KEY_PRINT_SCREEN:
        return wdInputSlot_KeyPrint;
      case GLFW_KEY_SCROLL_LOCK:
        return wdInputSlot_KeyScroll;
      case GLFW_KEY_PAUSE:
        return wdInputSlot_KeyPause;
      // TODO wdInputSlot_KeyPrevTrack
      // TODO wdInputSlot_KeyNextTrack
      // TODO wdInputSlot_KeyPlayPause
      // TODO wdInputSlot_KeyStop
      // TODO wdInputSlot_KeyVolumeUp
      // TODO wdInputSlot_KeyVolumeDown
      // TODO wdInputSlot_KeyMute
      default:
        return nullptr;
    }
  }
} // namespace

wdStandardInputDevice::wdStandardInputDevice(wdUInt32 uiWindowNumber, GLFWwindow* windowHandle)
  : m_uiWindowNumber(uiWindowNumber)
  , m_pWindow(windowHandle)
{
}

wdStandardInputDevice::~wdStandardInputDevice()
{
}

void wdStandardInputDevice::SetShowMouseCursor(bool bShow)
{
  glfwSetInputMode(m_pWindow, GLFW_CURSOR, bShow ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool wdStandardInputDevice::GetShowMouseCursor() const
{
  return (glfwGetInputMode(m_pWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED);
}

void wdStandardInputDevice::SetClipMouseCursor(wdMouseCursorClipMode::Enum mode)
{
}

wdMouseCursorClipMode::Enum wdStandardInputDevice::GetClipMouseCursor() const
{
  return wdMouseCursorClipMode::Default;
}

void wdStandardInputDevice::InitializeDevice() {}

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
  // TODO RegisterInputSlot(wdInputSlot_KeyTilde, "~", wdInputSlotFlags::IsButton);
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

  /* TODO
  RegisterInputSlot(wdInputSlot_KeyPrevTrack, "Previous Track", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyNextTrack, "Next Track", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyPlayPause, "Play / Pause", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyStop, "Stop", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyVolumeUp, "Volume Up", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyVolumeDown, "Volume Down", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_KeyMute, "Mute", wdInputSlotFlags::IsButton);
  */

  RegisterInputSlot(wdInputSlot_MousePositionX, "Mouse Position X", wdInputSlotFlags::IsMouseAxisPosition);
  RegisterInputSlot(wdInputSlot_MousePositionY, "Mouse Position Y", wdInputSlotFlags::IsMouseAxisPosition);

  RegisterInputSlot(wdInputSlot_MouseMoveNegX, "Mouse Move Left", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMovePosX, "Mouse Move Right", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMoveNegY, "Mouse Move Down", wdInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(wdInputSlot_MouseMovePosY, "Mouse Move Up", wdInputSlotFlags::IsMouseAxisMove);

  RegisterInputSlot(wdInputSlot_MouseButton0, "Mousebutton 0", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton1, "Mousebutton 1", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton2, "Mousebutton 2", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton3, "Mousebutton 3", wdInputSlotFlags::IsButton);
  RegisterInputSlot(wdInputSlot_MouseButton4, "Mousebutton 4", wdInputSlotFlags::IsButton);

  RegisterInputSlot(wdInputSlot_MouseWheelUp, "Mousewheel Up", wdInputSlotFlags::IsMouseWheel);
  RegisterInputSlot(wdInputSlot_MouseWheelDown, "Mousewheel Down", wdInputSlotFlags::IsMouseWheel);
}

void wdStandardInputDevice::ResetInputSlotValues()
{
  m_InputSlotValues[wdInputSlot_MouseWheelUp] = 0;
  m_InputSlotValues[wdInputSlot_MouseWheelDown] = 0;
  m_InputSlotValues[wdInputSlot_MouseMoveNegX] = 0;
  m_InputSlotValues[wdInputSlot_MouseMovePosX] = 0;
  m_InputSlotValues[wdInputSlot_MouseMoveNegY] = 0;
  m_InputSlotValues[wdInputSlot_MouseMovePosY] = 0;
}

void wdStandardInputDevice::OnKey(int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    m_uiLastCharacter = 0x00000008;
  }

  const char* szInputSlotName = ConvertGLFWKeyToEngineName(key);
  if (szInputSlotName)
  {
    m_InputSlotValues[szInputSlotName] = (action == GLFW_RELEASE) ? 0.0f : 1.0f;
  }
  else
  {
    wdLog::Warning("Unhandeled glfw keyboard key {} {}", key, (action == GLFW_RELEASE) ? "released" : "pressed");
  }
}

void wdStandardInputDevice::OnCharacter(unsigned int codepoint)
{
  m_uiLastCharacter = codepoint;
}

void wdStandardInputDevice::OnCursorPosition(double xpos, double ypos)
{
  s_iMouseIsOverWindowNumber = m_uiWindowNumber;

  int width;
  int height;
  glfwGetWindowSize(m_pWindow, &width, &height);

  m_InputSlotValues[wdInputSlot_MousePositionX] = static_cast<float>(xpos / width);
  m_InputSlotValues[wdInputSlot_MousePositionY] = static_cast<float>(ypos / height);

  if (m_LastPos.x != wdMath::MaxValue<double>())
  {
    wdVec2d diff = wdVec2d(xpos, ypos) - m_LastPos;

    m_InputSlotValues[wdInputSlot_MouseMoveNegX] += ((diff.x < 0) ? (float)-diff.x : 0.0f) * GetMouseSpeed().x;
    m_InputSlotValues[wdInputSlot_MouseMovePosX] += ((diff.x > 0) ? (float)diff.x : 0.0f) * GetMouseSpeed().x;
    m_InputSlotValues[wdInputSlot_MouseMoveNegY] += ((diff.y < 0) ? (float)-diff.y : 0.0f) * GetMouseSpeed().y;
    m_InputSlotValues[wdInputSlot_MouseMovePosY] += ((diff.y > 0) ? (float)diff.y : 0.0f) * GetMouseSpeed().y;
  }
  m_LastPos = wdVec2d(xpos, ypos);
}

void wdStandardInputDevice::OnMouseButton(int button, int action, int mods)
{
  const char* inputSlot = nullptr;
  switch (button)
  {
    case GLFW_MOUSE_BUTTON_1:
      inputSlot = wdInputSlot_MouseButton0;
      break;
    case GLFW_MOUSE_BUTTON_2:
      inputSlot = wdInputSlot_MouseButton1;
      break;
    case GLFW_MOUSE_BUTTON_3:
      inputSlot = wdInputSlot_MouseButton2;
      break;
    case GLFW_MOUSE_BUTTON_4:
      inputSlot = wdInputSlot_MouseButton3;
      break;
    case GLFW_MOUSE_BUTTON_5:
      inputSlot = wdInputSlot_MouseButton4;
      break;
  }

  if (inputSlot)
  {
    m_InputSlotValues[inputSlot] = (action == GLFW_PRESS) ? 1.0f : 0.0f;
  }
}

void wdStandardInputDevice::OnScroll(double xoffset, double yoffset)
{
  if (yoffset > 0)
  {
    m_InputSlotValues[wdInputSlot_MouseWheelUp] = static_cast<float>(yoffset);
  }
  else
  {
    m_InputSlotValues[wdInputSlot_MouseWheelDown] = static_cast<float>(-yoffset);
  }
}
