#include <Core/CorePCH.h>

#include <Core/Interfaces/WindWorldModule.h>
#include <Core/World/World.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdWindWorldModuleInterface, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_BEGIN_STATIC_REFLECTED_ENUM(wdWindStrength, 1)
  WD_ENUM_CONSTANTS(wdWindStrength::Calm, wdWindStrength::LightBrewde, wdWindStrength::GentleBrewde, wdWindStrength::ModerateBrewde, wdWindStrength::StrongBrewde, wdWindStrength::Storm)
  WD_ENUM_CONSTANTS(wdWindStrength::WeakShockwave, wdWindStrength::MediumShockwave, wdWindStrength::StrongShockwave, wdWindStrength::ExtremeShockwave)
WD_END_STATIC_REFLECTED_ENUM;
// clang-format on

float wdWindStrength::GetInMetersPerSecond(Enum strength)
{
  // inspired by the Beaufort scale
  // https://en.wikipedia.org/wiki/Beaufort_scale

  switch (strength)
  {
    case Calm:
      return 0.5f;

    case LightBrewde:
      return 2.0f;

    case GentleBrewde:
      return 5.0f;

    case ModerateBrewde:
      return 9.0f;

    case StrongBrewde:
      return 14.0f;

    case Storm:
      return 20.0f;

    case WeakShockwave:
      return 40.0f;

    case MediumShockwave:
      return 70.0f;

    case StrongShockwave:
      return 100.0f;

    case ExtremeShockwave:
      return 150.0f;

      WD_DEFAULT_CASE_NOT_IMPLEMENTED;
  }

  return 0;
}

wdWindWorldModuleInterface::wdWindWorldModuleInterface(wdWorld* pWorld)
  : wdWorldModule(pWorld)
{
}

wdVec3 wdWindWorldModuleInterface::ComputeWindFlutter(const wdVec3& vWind, const wdVec3& vObjectDir, float fFlutterSpeed, wdUInt32 uiFlutterRandomOffset) const
{
  if (vWind.IsZero(0.001f))
    return wdVec3::ZeroVector();

  wdVec3 windDir = vWind;
  const float fWindStrength = windDir.GetLengthAndNormalize();

  if (fWindStrength <= 0.01f)
    return wdVec3::ZeroVector();

  wdVec3 mainDir = vObjectDir;
  mainDir.NormalizeIfNotZero(wdVec3::UnitZAxis()).IgnoreResult();

  wdVec3 flutterDir = windDir.CrossRH(mainDir);
  flutterDir.NormalizeIfNotZero(wdVec3::UnitZAxis()).IgnoreResult();

  const float fFlutterOffset = (uiFlutterRandomOffset & 1023u) / 256.0f;

  const float fFlutter = wdMath::Sin(wdAngle::Radian(fFlutterOffset + fFlutterSpeed * fWindStrength * GetWorld()->GetClock().GetAccumulatedTime().AsFloatInSeconds())) * fWindStrength;

  return flutterDir * fFlutter;
}

WD_STATICLINK_FILE(Core, Core_Interfaces_WindWorldModule);
