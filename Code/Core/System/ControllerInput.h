#pragma once

#include <Core/CoreDLL.h>

class wdInputDeviceController;

class WD_CORE_DLL wdControllerInput
{
public:
  // \brief Returns if a global controller input device exists.
  static bool HasDevice();

  // \brief Returns the global controller input device. May be nullptr.
  static wdInputDeviceController* GetDevice();

  // \brief Set the global controller input device.
  static void SetDevice(wdInputDeviceController* pDevice);
};
