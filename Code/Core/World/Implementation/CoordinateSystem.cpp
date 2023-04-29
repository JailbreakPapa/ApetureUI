#include <Core/CorePCH.h>

#include <Core/World/CoordinateSystem.h>


wdCoordinateSystemConversion::wdCoordinateSystemConversion()
{
  m_mSourceToTarget.SetIdentity();
  m_mTargetToSource.SetIdentity();
}

void wdCoordinateSystemConversion::SetConversion(const wdCoordinateSystem& source, const wdCoordinateSystem& target)
{
  float fSourceScale = source.m_vForwardDir.GetLengthSquared();
  WD_ASSERT_DEV(wdMath::IsEqual(fSourceScale, source.m_vRightDir.GetLengthSquared(), wdMath::DefaultEpsilon<float>()),
    "Only uniformly scaled coordinate systems are supported");
  WD_ASSERT_DEV(wdMath::IsEqual(fSourceScale, source.m_vUpDir.GetLengthSquared(), wdMath::DefaultEpsilon<float>()),
    "Only uniformly scaled coordinate systems are supported");
  wdMat3 mSourceFromId;
  mSourceFromId.SetColumn(0, source.m_vRightDir);
  mSourceFromId.SetColumn(1, source.m_vUpDir);
  mSourceFromId.SetColumn(2, source.m_vForwardDir);

  float fTargetScale = target.m_vForwardDir.GetLengthSquared();
  WD_ASSERT_DEV(wdMath::IsEqual(fTargetScale, target.m_vRightDir.GetLengthSquared(), wdMath::DefaultEpsilon<float>()),
    "Only uniformly scaled coordinate systems are supported");
  WD_ASSERT_DEV(wdMath::IsEqual(fTargetScale, target.m_vUpDir.GetLengthSquared(), wdMath::DefaultEpsilon<float>()),
    "Only uniformly scaled coordinate systems are supported");
  wdMat3 mTargetFromId;
  mTargetFromId.SetColumn(0, target.m_vRightDir);
  mTargetFromId.SetColumn(1, target.m_vUpDir);
  mTargetFromId.SetColumn(2, target.m_vForwardDir);

  m_mSourceToTarget = mTargetFromId * mSourceFromId.GetInverse();
  m_mSourceToTarget.SetColumn(0, m_mSourceToTarget.GetColumn(0).GetNormalized());
  m_mSourceToTarget.SetColumn(1, m_mSourceToTarget.GetColumn(1).GetNormalized());
  m_mSourceToTarget.SetColumn(2, m_mSourceToTarget.GetColumn(2).GetNormalized());

  m_fWindingSwap = m_mSourceToTarget.GetDeterminant() < 0 ? -1.0f : 1.0f;
  m_fSourceToTargetScale = 1.0f / wdMath::Sqrt(fSourceScale) * wdMath::Sqrt(fTargetScale);
  m_mTargetToSource = m_mSourceToTarget.GetInverse();
  m_fTargetToSourceScale = 1.0f / m_fSourceToTargetScale;
}

wdVec3 wdCoordinateSystemConversion::ConvertSourcePosition(const wdVec3& vPos) const
{
  return m_mSourceToTarget * vPos * m_fSourceToTargetScale;
}

wdQuat wdCoordinateSystemConversion::ConvertSourceRotation(const wdQuat& qOrientation) const
{
  wdVec3 axis = m_mSourceToTarget * qOrientation.v;
  wdQuat rr(axis.x, axis.y, axis.z, qOrientation.w * m_fWindingSwap);
  return rr;
}

float wdCoordinateSystemConversion::ConvertSourceLength(float fLength) const
{
  return fLength * m_fSourceToTargetScale;
}

wdVec3 wdCoordinateSystemConversion::ConvertTargetPosition(const wdVec3& vPos) const
{
  return m_mTargetToSource * vPos * m_fTargetToSourceScale;
}

wdQuat wdCoordinateSystemConversion::ConvertTargetRotation(const wdQuat& qOrientation) const
{
  wdVec3 axis = m_mTargetToSource * qOrientation.v;
  wdQuat rr(axis.x, axis.y, axis.z, qOrientation.w * m_fWindingSwap);
  return rr;
}

float wdCoordinateSystemConversion::ConvertTargetLength(float fLength) const
{
  return fLength * m_fTargetToSourceScale;
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_CoordinateSystem);
