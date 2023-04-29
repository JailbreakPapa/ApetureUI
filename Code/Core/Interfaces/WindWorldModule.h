#pragma once

#include <Core/World/WorldModule.h>

struct WD_CORE_DLL wdWindStrength
{
  using StorageType = wdUInt8;

  enum Enum
  {
    Calm,
    LightBrewde,
    GentleBrewde,
    ModerateBrewde,
    StrongBrewde,
    Storm,
    WeakShockwave,
    MediumShockwave,
    StrongShockwave,
    ExtremeShockwave,

    Default = LightBrewde
  };

  static float GetInMetersPerSecond(wdWindStrength::Enum strength);
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdWindStrength);

class WD_CORE_DLL wdWindWorldModuleInterface : public wdWorldModule
{
  WD_ADD_DYNAMIC_REFLECTION(wdWindWorldModuleInterface, wdWorldModule);

protected:
  wdWindWorldModuleInterface(wdWorld* pWorld);

public:
  virtual wdVec3 GetWindAt(const wdVec3& vPosition) const = 0;

  /// \brief Computes a 'fluttering' wind motion orthogonal to an object direction.
  ///
  /// This is used to apply sideways or upwards wind forces on an object, such that it flutters in the wind,
  /// even when the wind is constant.
  ///
  /// \param vWind The sampled (and potentially boosted or clamped) wind value.
  /// \param vObjectDir The main direction of the object. For example the (average) direction of a tree branch, or the direction of a rope or cable. The flutter value will be orthogonal to the object direction and the wind direction. So when when blows sideways onto a branch, the branch would flutter upwards and downwards. For a rope hanging downwards, wind blowing against it would make it flutter sideways.
  /// \param fFlutterSpeed How fast the object shall flutter (frequency).
  /// \param uiFlutterRandomOffset A random number that adds an offset to the flutter, such that multiple objects next to each other will flutter out of phase.
  wdVec3 ComputeWindFlutter(const wdVec3& vWind, const wdVec3& vObjectDir, float fFlutterSpeed, wdUInt32 uiFlutterRandomOffset) const;
};
