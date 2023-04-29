#include <Core/Input/DeviceTypes/Controller.h>
#include <Core/Input/InputManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/UniquePtr.h>

#include <GLFW/glfw3.h>

class wdControllerInputGlfw : public wdInputDeviceController
{
public:
  virtual void InitializeDevice() override;

  virtual void UpdateInputSlotValues() override;

  virtual void ResetInputSlotValues() override;

  virtual void RegisterInputSlots() override;

  virtual bool IsControllerConnected(wdUInt8 uiPhysical) const override;

private:
  virtual void ApplyVibration(wdUInt8 uiPhysicalController, Motor::Enum eMotor, float fStrength) override;

  void RegisterControllerButton(const char* szButton, const char* szName, wdBitflags<wdInputSlotFlags> SlotFlags);
  void SetDeadZone(const char* szButton);
  void SetControllerValue(wdStringBuilder& tmp, wdUInt8 controllerIndex, const char* inputSlotName, float value);

  bool m_bInitialized = false;
};

namespace
{
  wdUniquePtr<wdControllerInputGlfw> g_pControllerInputGlfw;
}

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, ControllerInput)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "InputManager",
    "Window"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    g_pControllerInputGlfw = WD_DEFAULT_NEW(wdControllerInputGlfw);
    wdControllerInput::SetDevice(g_pControllerInputGlfw.Borrow());
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    if (wdControllerInput::GetDevice() == g_pControllerInputGlfw.Borrow())
    {
      wdControllerInput::SetDevice(nullptr);
    }
    g_pControllerInputGlfw.Clear();
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on

namespace
{
  struct ControllerButtonMapping
  {
    const char* wdName;
    int glfwIndex;
  };

  const ControllerButtonMapping g_ControllerButtonMappings[] = {
    {"button_a", GLFW_GAMEPAD_BUTTON_A},
    {"button_b", GLFW_GAMEPAD_BUTTON_B},
    {"button_x", GLFW_GAMEPAD_BUTTON_X},
    {"button_y", GLFW_GAMEPAD_BUTTON_Y},
    {"button_start", GLFW_GAMEPAD_BUTTON_START},
    {"button_back", GLFW_GAMEPAD_BUTTON_BACK},
    {"left_shoulder", GLFW_GAMEPAD_BUTTON_LEFT_BUMPER},
    {"right_shoulder", GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER},
    {"pad_up", GLFW_GAMEPAD_BUTTON_DPAD_UP},
    {"pad_down", GLFW_GAMEPAD_BUTTON_DPAD_DOWN},
    {"pad_left", GLFW_GAMEPAD_BUTTON_DPAD_LEFT},
    {"pad_right", GLFW_GAMEPAD_BUTTON_DPAD_RIGHT},
    {"left_stick", GLFW_GAMEPAD_BUTTON_LEFT_THUMB},
    {"right_stick", GLFW_GAMEPAD_BUTTON_RIGHT_THUMB}};
} // namespace

void wdControllerInputGlfw::RegisterControllerButton(const char* szButton, const char* szName, wdBitflags<wdInputSlotFlags> SlotFlags)
{
  wdStringBuilder s, s2;

  for (wdInt32 i = 0; i < MaxControllers; ++i)
  {
    s.Format("controller{0}_{1}", i, szButton);
    s2.Format("Cont {0}: {1}", i + 1, szName);
    RegisterInputSlot(s.GetData(), s2.GetData(), SlotFlags);
  }
}

void wdControllerInputGlfw::SetDeadZone(const char* szButton)
{
  wdStringBuilder s;

  for (wdInt32 i = 0; i < MaxControllers; ++i)
  {
    s.Format("controller{0}_{1}", i, szButton);
    wdInputManager::SetInputSlotDeadZone(s.GetData(), 0.23f);
  }
}

void wdControllerInputGlfw::SetControllerValue(wdStringBuilder& tmp, wdUInt8 controllerIndex, const char* inputSlotName, float value)
{
  tmp.Format("controller{0}_{1}", controllerIndex, inputSlotName);
  m_InputSlotValues[tmp] = value;
}

void wdControllerInputGlfw::InitializeDevice()
{
  // Make a arbitrary call into glfw so that we can check if the library is properly initialized
  glfwJoystickPresent(0);

  // Check for errors during the previous call
  const char* desc;
  int errorCode = glfwGetError(&desc);
  if(errorCode != GLFW_NO_ERROR)
  {
    wdLog::Warning("glfw joystick and gamepad input not avaiable: {} - {}", errorCode, desc);
    return;
  }
  m_bInitialized = true;
}

void wdControllerInputGlfw::UpdateInputSlotValues()
{
  if(!m_bInitialized)
  {
    return;
  }

  wdStringBuilder inputSlotName;

  // update all virtual controllers
  for (wdUInt8 uiVirtual = 0; uiVirtual < MaxControllers; ++uiVirtual)
  {
    // check from which physical device to take the input data
    const wdInt8 iPhysical = GetControllerMapping(uiVirtual);

    // if the mapping is negative (which means 'deactivated'), ignore this controller
    if ((iPhysical < 0) || (iPhysical >= MaxControllers))
      continue;

    int glfwId = GLFW_JOYSTICK_1 + iPhysical;
    if (glfwJoystickPresent(glfwId))
    {
      if (glfwJoystickIsGamepad(glfwId))
      {
        GLFWgamepadstate state = {};
        if (glfwGetGamepadState(glfwId, &state))
        {
          for (size_t buttonIndex = 0; buttonIndex < WD_ARRAY_SIZE(g_ControllerButtonMappings); buttonIndex++)
          {
            const ControllerButtonMapping mapping = g_ControllerButtonMappings[buttonIndex];
            SetControllerValue(inputSlotName, uiVirtual, mapping.wdName, (state.buttons[mapping.glfwIndex] == GLFW_PRESS) ? 1.0f : 0.0f);
          }

          SetControllerValue(inputSlotName, uiVirtual, "left_trigger", state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]);
          SetControllerValue(inputSlotName, uiVirtual, "right_trigger", state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]);

          SetControllerValue(inputSlotName, uiVirtual, "leftstick_negx", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * -1.0f));
          SetControllerValue(inputSlotName, uiVirtual, "leftstick_posx", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]));
          SetControllerValue(inputSlotName, uiVirtual, "leftstick_negy", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]));
          SetControllerValue(inputSlotName, uiVirtual, "leftstick_posy", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * -1.0f));

          SetControllerValue(inputSlotName, uiVirtual, "rightstick_negx", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * -1.0f));
          SetControllerValue(inputSlotName, uiVirtual, "rightstick_posx", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]));
          SetControllerValue(inputSlotName, uiVirtual, "rightstick_negy", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]));
          SetControllerValue(inputSlotName, uiVirtual, "rightstick_posy", wdMath::Max(0.0f, state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * -1.0f));
        }
      }
    }
  }
}

void wdControllerInputGlfw::ResetInputSlotValues()
{
}

void wdControllerInputGlfw::RegisterInputSlots()
{
  if(!m_bInitialized)
  {
    return;
  }

  RegisterControllerButton("button_a", "Button A", wdInputSlotFlags::IsButton);
  RegisterControllerButton("button_b", "Button B", wdInputSlotFlags::IsButton);
  RegisterControllerButton("button_x", "Button X", wdInputSlotFlags::IsButton);
  RegisterControllerButton("button_y", "Button Y", wdInputSlotFlags::IsButton);
  RegisterControllerButton("button_start", "Start", wdInputSlotFlags::IsButton);
  RegisterControllerButton("button_back", "Back", wdInputSlotFlags::IsButton);
  RegisterControllerButton("left_shoulder", "Left Shoulder", wdInputSlotFlags::IsButton);
  RegisterControllerButton("right_shoulder", "Right Shoulder", wdInputSlotFlags::IsButton);
  RegisterControllerButton("left_trigger", "Left Trigger", wdInputSlotFlags::IsAnalogTrigger);
  RegisterControllerButton("right_trigger", "Right Trigger", wdInputSlotFlags::IsAnalogTrigger);
  RegisterControllerButton("pad_up", "Pad Up", wdInputSlotFlags::IsDPad);
  RegisterControllerButton("pad_down", "Pad Down", wdInputSlotFlags::IsDPad);
  RegisterControllerButton("pad_left", "Pad Left", wdInputSlotFlags::IsDPad);
  RegisterControllerButton("pad_right", "Pad Right", wdInputSlotFlags::IsDPad);
  RegisterControllerButton("left_stick", "Left Stick", wdInputSlotFlags::IsButton);
  RegisterControllerButton("right_stick", "Right Stick", wdInputSlotFlags::IsButton);

  RegisterControllerButton("leftstick_negx", "Left Stick Left", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("leftstick_posx", "Left Stick Right", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("leftstick_negy", "Left Stick Down", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("leftstick_posy", "Left Stick Up", wdInputSlotFlags::IsAnalogStick);

  RegisterControllerButton("rightstick_negx", "Right Stick Left", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("rightstick_posx", "Right Stick Right", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("rightstick_negy", "Right Stick Down", wdInputSlotFlags::IsAnalogStick);
  RegisterControllerButton("rightstick_posy", "Right Stick Up", wdInputSlotFlags::IsAnalogStick);

  SetDeadZone("left_trigger");
  SetDeadZone("right_trigger");
  SetDeadZone("leftstick_negx");
  SetDeadZone("leftstick_posx");
  SetDeadZone("leftstick_negy");
  SetDeadZone("leftstick_posy");
  SetDeadZone("rightstick_negx");
  SetDeadZone("rightstick_posx");
  SetDeadZone("rightstick_negy");
  SetDeadZone("rightstick_posy");
}

bool wdControllerInputGlfw::IsControllerConnected(wdUInt8 uiPhysical) const
{
  if(!m_bInitialized)
  {
    return false;
  }

  int glfwId = GLFW_JOYSTICK_1 + uiPhysical;
  return glfwJoystickPresent(glfwId) && glfwJoystickIsGamepad(glfwId);
}

void wdControllerInputGlfw::ApplyVibration(wdUInt8 uiPhysicalController, Motor::Enum eMotor, float fStrength)
{
  // Unfortunately GLFW does not have vibration support
}
