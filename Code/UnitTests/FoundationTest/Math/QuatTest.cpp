#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Quat.h>

WD_CREATE_SIMPLE_TEST(Math, Quaternion)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMat3T::ComponentType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdQuatT p;
      WD_TEST_BOOL(wdMath::IsNaN(p.v.x) && wdMath::IsNaN(p.v.y) && wdMath::IsNaN(p.v.z) && wdMath::IsNaN(p.w));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdQuatT::ComponentType testBlock[4] = {
      (wdQuatT::ComponentType)1, (wdQuatT::ComponentType)2, (wdQuatT::ComponentType)3, (wdQuatT::ComponentType)4};
    wdQuatT* p = ::new ((void*)&testBlock[0]) wdQuatT;
    WD_TEST_BOOL(p->v.x == (wdMat3T::ComponentType)1 && p->v.y == (wdMat3T::ComponentType)2 && p->v.z == (wdMat3T::ComponentType)3 &&
                 p->w == (wdMat3T::ComponentType)4);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(x,y,z,w)")
  {
    wdQuatT q(1, 2, 3, 4);

    WD_TEST_VEC3(q.v, wdVec3T(1, 2, 3), 0.0001f);
    WD_TEST_FLOAT(q.w, 4, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IdentityQuaternion")
  {
    wdQuatT q = wdQuatT::IdentityQuaternion();

    WD_TEST_VEC3(q.v, wdVec3T(0, 0, 0), 0.0001f);
    WD_TEST_FLOAT(q.w, 1, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetIdentity")
  {
    wdQuatT q(1, 2, 3, 4);

    q.SetIdentity();

    WD_TEST_VEC3(q.v, wdVec3T(0, 0, 0), 0.0001f);
    WD_TEST_FLOAT(q.w, 1, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetElements")
  {
    wdQuatT q(5, 6, 7, 8);

    q.SetElements(1, 2, 3, 4);

    WD_TEST_VEC3(q.v, wdVec3T(1, 2, 3), 0.0001f);
    WD_TEST_FLOAT(q.w, 4, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromAxisAndAngle / operator* (quat, vec)")
  {
    {
      wdQuatT q;
      q.SetFromAxisAndAngle(wdVec3T(1, 0, 0), wdAngle::Degree(90));

      WD_TEST_VEC3(q * wdVec3T(0, 1, 0), wdVec3T(0, 0, 1), 0.0001f);
    }

    {
      wdQuatT q;
      q.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(90));

      WD_TEST_VEC3(q * wdVec3T(1, 0, 0), wdVec3T(0, 0, -1), 0.0001f);
    }

    {
      wdQuatT q;
      q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));

      WD_TEST_VEC3(q * wdVec3T(0, 1, 0), wdVec3T(-1, 0, 0), 0.0001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetShortestRotation / IsEqualRotation")
  {
    wdQuatT q1, q2, q3;
    q1.SetShortestRotation(wdVec3T(0, 1, 0), wdVec3T(1, 0, 0));
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, -1), wdAngle::Degree(90));
    q3.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(-90));

    WD_TEST_BOOL(q1.IsEqualRotation(q2, wdMath::LargeEpsilon<float>()));
    WD_TEST_BOOL(q1.IsEqualRotation(q3, wdMath::LargeEpsilon<float>()));

    WD_TEST_BOOL(wdQuatT::IdentityQuaternion().IsEqualRotation(wdQuatT::IdentityQuaternion(), wdMath::LargeEpsilon<float>()));
    WD_TEST_BOOL(wdQuatT::IdentityQuaternion().IsEqualRotation(wdQuatT(0, 0, 0, -1), wdMath::LargeEpsilon<float>()));

    wdQuatT q4{0, 0, 0, 1.00000012f};
    wdQuatT q5{0, 0, 0, 1.00000023f};
    WD_TEST_BOOL(q4.IsEqualRotation(q5, wdMath::LargeEpsilon<float>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromMat3")
  {
    wdMat3T m;
    m.SetRotationMatrixZ(wdAngle::Degree(-90));

    wdQuatT q1, q2, q3;
    q1.SetFromMat3(m);
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, -1), wdAngle::Degree(90));
    q3.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(-90));

    WD_TEST_BOOL(q1.IsEqualRotation(q2, wdMath::LargeEpsilon<float>()));
    WD_TEST_BOOL(q1.IsEqualRotation(q3, wdMath::LargeEpsilon<float>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSlerp")
  {
    wdQuatT q1, q2, q3, qr;
    q1.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(45));
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(0));
    q3.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));

    qr.SetSlerp(q2, q3, 0.5f);

    WD_TEST_BOOL(q1.IsEqualRotation(qr, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRotationAxisAndAngle")
  {
    wdQuatT q1, q2, q3;
    q1.SetShortestRotation(wdVec3T(0, 1, 0), wdVec3T(1, 0, 0));
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, -1), wdAngle::Degree(90));
    q3.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(-90));

    wdVec3T axis;
    wdAngle angle;

    WD_TEST_BOOL(q1.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_VEC3(axis, wdVec3T(0, 0, -1), 0.001f);
    WD_TEST_FLOAT(angle.GetDegree(), 90, wdMath::LargeEpsilon<wdMat3T::ComponentType>());

    WD_TEST_BOOL(q2.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_VEC3(axis, wdVec3T(0, 0, -1), 0.001f);
    WD_TEST_FLOAT(angle.GetDegree(), 90, wdMath::LargeEpsilon<wdMat3T::ComponentType>());

    WD_TEST_BOOL(q3.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_VEC3(axis, wdVec3T(0, 0, -1), 0.001f);
    WD_TEST_FLOAT(angle.GetDegree(), 90, wdMath::LargeEpsilon<wdMat3T::ComponentType>());

    WD_TEST_BOOL(wdQuatT::IdentityQuaternion().GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_VEC3(axis, wdVec3T(1, 0, 0), 0.001f);
    WD_TEST_FLOAT(angle.GetDegree(), 0, wdMath::LargeEpsilon<wdMat3T::ComponentType>());

    wdQuatT otherIdentity(0, 0, 0, -1);
    WD_TEST_BOOL(otherIdentity.GetRotationAxisAndAngle(axis, angle) == WD_SUCCESS);
    WD_TEST_VEC3(axis, wdVec3T(1, 0, 0), 0.001f);
    WD_TEST_FLOAT(angle.GetDegree(), 360, wdMath::LargeEpsilon<wdMat3T::ComponentType>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsMat3")
  {
    wdQuatT q;
    q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));

    wdMat3T mr;
    mr.SetRotationMatrixZ(wdAngle::Degree(90));

    wdMat3T m = q.GetAsMat3();

    WD_TEST_BOOL(mr.IsEqual(m, wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsMat4")
  {
    wdQuatT q;
    q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));

    wdMat4T mr;
    mr.SetRotationMatrixZ(wdAngle::Degree(90));

    wdMat4T m = q.GetAsMat4();

    WD_TEST_BOOL(mr.IsEqual(m, wdMath::DefaultEpsilon<wdMat3T::ComponentType>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid / Normalize")
  {
    wdQuatT q(1, 2, 3, 4);
    WD_TEST_BOOL(!q.IsValid(0.001f));

    q.Normalize();
    WD_TEST_BOOL(q.IsValid(0.001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-")
  {
    wdQuatT q, q1;
    q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    q1.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(-90));

    wdQuatT q2 = -q;
    WD_TEST_BOOL(q1.IsEqualRotation(q2, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Dot")
  {
    wdQuatT q, q1, q2;
    q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));
    q1.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(-90));
    q2.SetFromAxisAndAngle(wdVec3T(0, 1, 0), wdAngle::Degree(45));

    WD_TEST_FLOAT(q.Dot(q), 1.0f, 0.0001f);
    WD_TEST_FLOAT(q.Dot(wdQuat::IdentityQuaternion()), cos(wdAngle::DegToRad(90.0f / 2)), 0.0001f);
    WD_TEST_FLOAT(q.Dot(q1), 0.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*(quat, quat)")
  {
    wdQuatT q1, q2, qr, q3;
    q1.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(60));
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(30));
    q3.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90));

    qr = q1 * q2;

    WD_TEST_BOOL(qr.IsEqualRotation(q3, 0.0001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator==/!=")
  {
    wdQuatT q1, q2;
    q1.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(60));
    q2.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(30));
    WD_TEST_BOOL(q1 != q2);

    q2.SetFromAxisAndAngle(wdVec3T(1, 0, 0), wdAngle::Degree(60));
    WD_TEST_BOOL(q1 != q2);

    q2.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(60));
    WD_TEST_BOOL(q1 == q2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdQuatT q;

      q.SetIdentity();
      WD_TEST_BOOL(!q.IsNaN());

      q.SetIdentity();
      q.w = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(q.IsNaN());

      q.SetIdentity();
      q.v.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(q.IsNaN());

      q.SetIdentity();
      q.v.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(q.IsNaN());

      q.SetIdentity();
      q.v.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(q.IsNaN());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "rotation direction")
  {
    wdMat3T m;
    m.SetRotationMatrixZ(wdAngle::Degree(90.0f));

    wdQuatT q;
    q.SetFromAxisAndAngle(wdVec3T(0, 0, 1), wdAngle::Degree(90.0f));

    wdVec3T xAxis(1, 0, 0);

    wdVec3T temp1 = m.TransformDirection(xAxis);
    wdVec3T temp2 = q.GetAsMat3().TransformDirection(xAxis);

    WD_TEST_BOOL(temp1.IsEqual(temp2, 0.01f));
  }
}
