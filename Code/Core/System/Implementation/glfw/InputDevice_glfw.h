#pragma once

#include <Core/Input/DeviceTypes/MouseKeyboard.h>

extern "C"
{
  typedef struct GLFWwindow GLFWwindow;
}

class WD_CORE_DLL wdStandardInputDevice : public wdInputDeviceMouseKeyboard
{
  WD_ADD_DYNAMIC_REFLECTION(wdStandardInputDevice, wdInputDeviceMouseKeyboard);

public:
  wdStandardInputDevice(wdUInt32 uiWindowNumber, GLFWwindow* windowHandle);
  ~wdStandardInputDevice();

  virtual void SetShowMouseCursor(bool bShow) override;
  virtual bool GetShowMouseCursor() const override;
  virtual void SetClipMouseCursor(wdMouseCursorClipMode::Enum mode) override;
  virtual wdMouseCursorClipMode::Enum GetClipMouseCursor() const override;

  // GLFW callback for key pressed, released, repeated events
  void OnKey(int key, int scancode, int action, int mods);

  // GLFW callback for text input (each UTF32 code point individually)
  void OnCharacter(unsigned int codepoint);

  // GLFW callback on mouse move
  void OnCursorPosition(double xpos, double ypos);

  // GLFW callback on mouse button actions
  void OnMouseButton(int button, int action, int mods);

  // GLFW callback for mouse scroll
  void OnScroll(double xoffset, double yoffset);

private:
  virtual void InitializeDevice() override;
  virtual void RegisterInputSlots() override;
  virtual void ResetInputSlotValues() override;

private:
  wdUInt32 m_uiWindowNumber = 0;
  GLFWwindow* m_pWindow = nullptr;
  wdVec2d m_LastPos = wdVec2d(wdMath::MaxValue<double>());
};
