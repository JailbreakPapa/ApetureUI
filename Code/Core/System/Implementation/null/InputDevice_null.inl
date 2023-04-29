#include <Core/System/Implementation/null/InputDevice_null.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdStandardInputDevice, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdStandardInputDevice::wdStandardInputDevice(wdUInt32 uiWindowNumber) {}
wdStandardInputDevice::~wdStandardInputDevice() = default;

void wdStandardInputDevice::SetShowMouseCursor(bool bShow) {}

void wdStandardInputDevice::SetClipMouseCursor(wdMouseCursorClipMode::Enum mode) {}

wdMouseCursorClipMode::Enum wdStandardInputDevice::GetClipMouseCursor() const
{
  return wdMouseCursorClipMode::Default;
}

void wdStandardInputDevice::InitializeDevice() {}

void wdStandardInputDevice::RegisterInputSlots() {}
