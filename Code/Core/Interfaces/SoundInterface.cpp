#include <Core/CorePCH.h>

#include <Core/Interfaces/SoundInterface.h>
#include <Foundation/Configuration/Singleton.h>

wdResult wdSoundInterface::PlaySound(wdStringView sResourceID, const wdTransform& globalPosition, float fPitch /*= 1.0f*/, float fVolume /*= 1.0f*/, bool bBlockIfNotLoaded /*= true*/)
{
  if (wdSoundInterface* pSoundInterface = wdSingletonRegistry::GetSingletonInstance<wdSoundInterface>())
  {
    return pSoundInterface->OneShotSound(sResourceID, globalPosition, fPitch, fVolume, bBlockIfNotLoaded);
  }

  return WD_FAILURE;
}
