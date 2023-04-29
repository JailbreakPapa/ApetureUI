#pragma once

#include <Foundation/Math/Vec3.h>

#include <Core/World/Declarations.h>
#include <Foundation/Types/RefCounted.h>

struct WD_CORE_DLL wdCoordinateSystem
{
  WD_DECLARE_POD_TYPE();

  wdVec3 m_vForwardDir;
  wdVec3 m_vRightDir;
  wdVec3 m_vUpDir;
};

class WD_CORE_DLL wdCoordinateSystemProvider : public wdRefCounted
{
public:
  wdCoordinateSystemProvider(const wdWorld* pOwnerWorld)
    : m_pOwnerWorld(pOwnerWorld)
  {
  }

  virtual ~wdCoordinateSystemProvider() {}

  virtual void GetCoordinateSystem(const wdVec3& vGlobalPosition, wdCoordinateSystem& out_coordinateSystem) const = 0;

protected:
  friend class wdWorld;

  const wdWorld* m_pOwnerWorld;
};

/// \brief Helper class to convert between two wdCoordinateSystem spaces.
///
/// All functions will do an identity transform until SetConversion is called to set up
/// the conversion. Afterwards the convert functions can be used to convert between
/// the two systems in both directions.
/// Currently, only uniformly scaled orthogonal coordinate systems are supported.
/// They can however be right handed or left handed.
class WD_CORE_DLL wdCoordinateSystemConversion
{
public:
  /// \brief Creates a new conversion that until set up, does identity conversions.
  wdCoordinateSystemConversion(); // [tested]

  /// \brief Set up the source and target coordinate systems.
  void SetConversion(const wdCoordinateSystem& source, const wdCoordinateSystem& target); // [tested]
  /// \brief Returns the equivalent point in the target coordinate system.
  wdVec3 ConvertSourcePosition(const wdVec3& vPos) const; // [tested]
  /// \brief Returns the equivalent rotation in the target coordinate system.
  wdQuat ConvertSourceRotation(const wdQuat& qOrientation) const; // [tested]
  /// \brief Returns the equivalent length in the target coordinate system.
  float ConvertSourceLength(float fLength) const; // [tested]

  /// \brief Returns the equivalent point in the source coordinate system.
  wdVec3 ConvertTargetPosition(const wdVec3& vPos) const; // [tested]
  /// \brief Returns the equivalent rotation in the source coordinate system.
  wdQuat ConvertTargetRotation(const wdQuat& qOrientation) const; // [tested]
  /// \brief Returns the equivalent length in the source coordinate system.
  float ConvertTargetLength(float fLength) const; // [tested]

private:
  wdMat3 m_mSourceToTarget;
  wdMat3 m_mTargetToSource;
  float m_fWindingSwap = 1.0f;
  float m_fSourceToTargetScale = 1.0f;
  float m_fTargetToSourceScale = 1.0f;
};
