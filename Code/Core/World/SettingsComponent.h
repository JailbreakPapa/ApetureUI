#pragma once

#include <Core/World/Component.h>

/// \brief Base class for settings components, of which only one per type should exist in each world.
///
/// Settings components are used to store global scene specific settings, e.g. for physics it would be the scene gravity,
/// for rendering it might be the time of day, fog settings, etc.
///
/// Components of this type should be managed by an wdSettingsComponentManager, which makes it easy to query for the one instance
/// in the world.
///
///
class WD_CORE_DLL wdSettingsComponent : public wdComponent
{
  WD_ADD_DYNAMIC_REFLECTION(wdSettingsComponent, wdComponent);

  //////////////////////////////////////////////////////////////////////////
  // wdSettingsComponent

public:
  /// \brief The constructor marks the component as modified.
  wdSettingsComponent();
  ~wdSettingsComponent();

  /// \brief Marks the component as modified. Individual bits can be used to mark only specific settings (groups) as modified.
  void SetModified(wdUInt32 uiBits = 0xFFFFFFFF) { m_uiSettingsModified |= uiBits; }

  /// \brief Checks whether the component (or some settings group) was marked as modified.
  bool IsModified(wdUInt32 uiBits = 0xFFFFFFFF) const { return (m_uiSettingsModified & uiBits) != 0; }

  /// \brief Marks the settings as not-modified.
  void ResetModified(wdUInt32 uiBits = 0xFFFFFFFF) { m_uiSettingsModified &= ~uiBits; }

private:
  wdUInt32 m_uiSettingsModified = 0xFFFFFFFF;
};
