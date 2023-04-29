#include <Core/CorePCH.h>

#include <Core/System/ControllerInput.h>
#include <Foundation/System/PlatformFeatures.h>

namespace
{
  wdInputDeviceController* g_pInputDeviceController = nullptr;
}

bool wdControllerInput::HasDevice()
{
  return g_pInputDeviceController != nullptr;
}

wdInputDeviceController* wdControllerInput::GetDevice()
{
  return g_pInputDeviceController;
}

void wdControllerInput::SetDevice(wdInputDeviceController* pDevice)
{
  g_pInputDeviceController = pDevice;
}

#if WD_ENABLED(WD_SUPPORTS_GLFW)
#  include <Core/System/Implementation/glfw/ControllerInput_glfw.inl>
#endif


WD_STATICLINK_FILE(Core, Core_System_Implementation_ControllerInput);
