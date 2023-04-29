#include <Core/CorePCH.h>

#include <Core/World/SettingsComponent.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdSettingsComponent, 1, wdRTTINoAllocator)
{
  WD_BEGIN_ATTRIBUTES
  {
    new wdCategoryAttribute("Settings"),
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdSettingsComponent::wdSettingsComponent()
{
  SetModified();
}

wdSettingsComponent::~wdSettingsComponent() = default;


WD_STATICLINK_FILE(Core, Core_World_Implementation_SettingsComponent);
