#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdQuat.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdQuat)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    // In debug the default constructor initializes everything with NaN.
    wdSimdQuat vDefCtor;
    WD_TEST_BOOL(vDefCtor.IsNaN());
#else

#  if WD_DISABLED(WD_COMPILER_GCC)
    // Placement new of the default constructor should not have any effect on the previous data.
    alignas(16) float testBlock[4] = {1, 2, 3, 4};
    wdSimdQuat* pDefCtor = ::new ((void*)&testBlock[0]) wdSimdQuat;
    WD_TEST_BOOL(pDefCtor->m_v.x() == 1.0f && pDefCtor->m_v.y() == 2.0f && pDefCtor->m_v.z() == 3.0f && pDefCtor->m_v.w() == 4.0f);
#  endif

#endif

    // Make sure the class didn't accidentally change in size.
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE
    WD_CHECK_AT_COMPILETIME(sizeof(wdSimdQuat) == 16);
    WD_CHECK_AT_COMPILETIME(WD_ALIGNMENT_OF(wdSimdQuat) == 16);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IdentityQuaternion")
  {
    wdSimdQuat q = wdSimdQuat::IdentityQuaternion();

    WD_TEST_BOOL(q.m_v.x() == 0.0f && q.m_v.y() == 0.0f && q.m_v.z() == 0.0f && q.m_v.w() == 1.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdSimdQuat q(wdSimdVec4f(1, 2, 3, 4));

    q.SetIdentity();

    WD_TEST_BOOL(q.m_v.x() == 0.0f && q.m_v.y() == 0.0f && q.m_v.z() == 0.0f && q.m_v.w() == 1.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromAxisAndAngle / operator* (quat, vec)")
  {
    {
      wdSimdQuat q;
      q.SetFromAxisAndAngle(wdSimdVec4f(1, 0, 0), wdAngle::Degree(90));

      WD_TEST_BOOL((q * wdSimdVec4f(0, 1, 0)).IsEqual(wdSimdVec4f(0, 0, 1), 0.0001f).AllSet());
    }

    {
      wdSimdQuat q;
      q.SetFromAxisAndAngle(wdSimdVec4f(0, 1, 0), wdAngle::Degree(90));

      WD_TEST_BOOL((q * wdSimdVec4f(1, 0, 0)).IsEqual(wdSimdVec4f(0, 0, -1), 0.0001f).AllSet());
    }

    {
      wdSimdQuat q;
      q.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));

      WD_TEST_BOOL((q * wdSimdVec4f(0, 1, 0)).IsEqual(wdSimdVec4f(-1, 0, 0), 0.0001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetShortestRotation / IsEqualRotation")
  {
    wdSimdQuat q1, q2, q3;
    q1.SetShortestRotation(wdSimdVec4f(0, 1, 0), wdSimdVec4f(1, 0, 0));
    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, -1), wdAngle::Degree(90));
    q3.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(-90));

    WD_TEST_BOOL(q1.IsEqualRotation(q2, wdMath::LargeEpsilon<float>()));
    WD_TEST_BOOL(q1.IsEqualRotation(q3, wdMath::LargeEpsilon<float>()));

    WD_TEST_BOOL(wdSimdQuat::IdentityQuaternion().IsEqualRotation(wdSimdQuat::IdentityQuaternion(), wdMath::LargeEpsilon<float>()));
    WD_TEST_BOOL(wdSimdQuat::IdentityQuaternion().IsEqualRotation(wdSimdQuat(wdSimdVec4f(0, 0, 0, -1)), wdMath::LargeEpsilon<float>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSlerp")
  {
    wdSimdQuat q1, q2, q3, qr;
    q1.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(45));
    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(0));
    q3.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));

    qr.SetSlerp(q2, q3, 0.5f);

    WD_TEST_BOOL(q1.IsEqualRotation(qr, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRotationAxisAndAngle")
  {
    wdSimdQuat q1, q2, q3;
    q1.SetShortestRotation(wdSimdVec4f(0, 1, 0), wdSimdVec4f(1, 0, 0));
    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, -1), wdAngle::Degree(90));
    q3.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(-90));

    wdSimdVec4f axis;
    wdSimdFloat angle;

    WD_TEST_BOOL(q1.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_BOOL(axis.IsEqual(wdSimdVec4f(0, 0, -1), 0.001f).AllSet<3>());
    WD_TEST_FLOAT(wdAngle::RadToDeg((float)angle), 90, wdMath::LargeEpsilon<float>());

    WD_TEST_BOOL(q2.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_BOOL(axis.IsEqual(wdSimdVec4f(0, 0, -1), 0.001f).AllSet<3>());
    WD_TEST_FLOAT(wdAngle::RadToDeg((float)angle), 90, wdMath::LargeEpsilon<float>());

    WD_TEST_BOOL(q3.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_BOOL(axis.IsEqual(wdSimdVec4f(0, 0, -1), 0.001f).AllSet<3>());
    WD_TEST_FLOAT(wdAngle::RadToDeg((float)angle), 90, wdMath::LargeEpsilon<float>());

    WD_TEST_BOOL(wdSimdQuat::IdentityQuaternion().GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_BOOL(axis.IsEqual(wdSimdVec4f(1, 0, 0), 0.001f).AllSet<3>());
    WD_TEST_FLOAT(wdAngle::RadToDeg((float)angle), 0, wdMath::LargeEpsilon<float>());

    wdSimdQuat otherIdentity(wdSimdVec4f(0, 0, 0, -1));
    WD_TEST_BOOL(otherIdentity.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_BOOL(axis.IsEqual(wdSimdVec4f(1, 0, 0), 0.001f).AllSet<3>());
    WD_TEST_FLOAT(wdAngle::RadToDeg((float)angle), 360, wdMath::LargeEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid / Normalize")
  {
    wdSimdQuat q(wdSimdVec4f(1, 2, 3, 4));
    WD_TEST_BOOL(!q.IsValid(0.001f));

    q.Normalize();
    WD_TEST_BOOL(q.IsValid(0.001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-")
  {
    wdSimdQuat q, q1;
    q.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
    q1.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(-90));

    wdSimdQuat q2 = -q;
    WD_TEST_BOOL(q1.IsEqualRotation(q2, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(quat, quat)")
  {
    wdSimdQuat q1, q2, qr, q3;
    q1.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(60));
    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(30));
    q3.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));

    qr = q1 * q2;

    WD_TEST_BOOL(qr.IsEqualRotation(q3, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator==/!=")
  {
    wdSimdQuat q1, q2;
    q1.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(60));
    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(30));
    WD_TEST_BOOL(q1 != q2);

    q2.SetFromAxisAndAngle(wdSimdVec4f(1, 0, 0), wdAngle::Degree(60));
    WD_TEST_BOOL(q1 != q2);

    q2.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(60));
    WD_TEST_BOOL(q1 == q2);
  }
}
