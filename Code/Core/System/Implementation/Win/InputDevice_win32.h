#pragma once

#include <Core/Input/DeviceTypes/MouseKeyboard.h>
#include <Foundation/Basics/Platform/Win/MinWindows.h>

class WD_CORE_DLL wdStandardInputDevice : public wdInputDeviceMouseKeyboard
{
  WD_ADD_DYNAMIC_REFLECTION(wdStandardInputDevice, wdInputDeviceMouseKeyboard);

public:
  wdStandardInputDevice(wdUInt32 uiWindowNumber);
  ~wdStandardInputDevice();

  /// \brief This function needs to be called by all Windows functions, to pass the input information through to this input device.
  void WindowMessage(wdMinWindows::HWND pWnd, wdMinWindows::UINT msg, wdMinWindows::WPARAM wparam, wdMinWindows::LPARAM lparam);

  /// \brief Calling this function will 'translate' most key names from English to the OS language, by querying that information
  /// from the OS.
  ///
  /// The OS translation might not always be perfect for all keys. The translation can change when the user changes the keyboard layout.
  /// So if he switches from an English layout to a German layout, LocalizeButtonDisplayNames() should be called again, to update
  /// the display names, if that is required.
  static void LocalizeButtonDisplayNames();

  virtual void SetClipMouseCursor(wdMouseCursorClipMode::Enum mode) override;
  virtual wdMouseCursorClipMode::Enum GetClipMouseCursor() const override { return m_ClipCursorMode; }

  virtual void SetShowMouseCursor(bool bShow) override;
  virtual bool GetShowMouseCursor() const override;

protected:
  virtual void InitializeDevice() override;
  virtual void RegisterInputSlots() override;
  virtual void ResetInputSlotValues() override;
  virtual void UpdateInputSlotValues() override;

private:
  void ApplyClipRect(wdMouseCursorClipMode::Enum mode, wdMinWindows::HWND hWnd);
  void OnFocusLost(wdMinWindows::HWND hWnd);

  static bool s_bMainWindowUsed;
  wdUInt32 m_uiWindowNumber = 0;
  bool m_bShowCursor = true;
  wdMouseCursorClipMode::Enum m_ClipCursorMode = wdMouseCursorClipMode::NoClip;
  bool m_bApplyClipRect = false;
  wdUInt8 m_uiMouseButtonReceivedDown[5] = {0, 0, 0, 0, 0};
  wdUInt8 m_uiMouseButtonReceivedUp[5] = {0, 0, 0, 0, 0};
};
