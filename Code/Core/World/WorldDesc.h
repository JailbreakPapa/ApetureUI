#pragma once

#include <Foundation/Strings/HashedString.h>
#include <Foundation/Types/SharedPtr.h>
#include <Foundation/Types/UniquePtr.h>

#include <Core/World/CoordinateSystem.h>
#include <Core/World/SpatialSystem.h>

class wdTimeStepSmoothing;

/// \brief Describes the initial state of a world.
struct wdWorldDesc
{
  WD_DECLARE_POD_TYPE();

  wdWorldDesc(wdStringView sWorldName) { m_sName.Assign(sWorldName); }

  wdHashedString m_sName;
  wdUInt64 m_uiRandomNumberGeneratorSeed = 0;

  wdUniquePtr<wdSpatialSystem> m_pSpatialSystem;
  bool m_bAutoCreateSpatialSystem = true; ///< automatically create a default spatial system if none is set

  wdSharedPtr<wdCoordinateSystemProvider> m_pCoordinateSystemProvider;
  wdUniquePtr<wdTimeStepSmoothing> m_pTimeStepSmoothing; ///< if nullptr, wdDefaultTimeStepSmoothing will be used

  bool m_bReportErrorWhenStaticObjectMoves = true;

  wdTime m_MaxComponentInitializationTimePerFrame = wdTime::Hours(10000); // max time to spend on component initialization per frame
};
