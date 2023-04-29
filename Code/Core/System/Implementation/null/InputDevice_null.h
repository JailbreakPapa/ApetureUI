#pragma once

#include <Core/Input/DeviceTypes/MouseKeyboard.h>

class WD_CORE_DLL wdStandardInputDevice : public wdInputDeviceMouseKeyboard
{
  WD_ADD_DYNAMIC_REFLECTION(wdStandardInputDevice, wdInputDeviceMouseKeyboard);

public:
  wdStandardInputDevice(wdUInt32 uiWindowNumber);
  ~wdStandardInputDevice();

  virtual void SetShowMouseCursor(bool bShow) override;
  virtual void SetClipMouseCursor(wdMouseCursorClipMode::Enum mode) override;
  virtual wdMouseCursorClipMode::Enum GetClipMouseCursor() const override;

private:
  virtual void InitializeDevice() override;
  virtual void RegisterInputSlots() override;
};
