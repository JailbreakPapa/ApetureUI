#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBox.h>
#include <Foundation/Math/BoundingSphere.h>
#include <Foundation/Math/Plane.h>
#include <Foundation/Math/Random.h>

WD_CREATE_SIMPLE_TEST(Math, Plane)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMat3T::ComponentType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdPlaneT p;
      WD_TEST_BOOL(wdMath::IsNaN(p.m_vNormal.x) && wdMath::IsNaN(p.m_vNormal.y) && wdMath::IsNaN(p.m_vNormal.z) && wdMath::IsNaN(p.m_fNegDistance));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdPlaneT::ComponentType testBlock[4] = {(wdPlaneT::ComponentType)1, (wdPlaneT::ComponentType)2, (wdPlaneT::ComponentType)3, (wdPlaneT::ComponentType)4};
    wdPlaneT* p = ::new ((void*)&testBlock[0]) wdPlaneT;
    WD_TEST_BOOL(p->m_vNormal.x == (wdPlaneT::ComponentType)1 && p->m_vNormal.y == (wdPlaneT::ComponentType)2 && p->m_vNormal.z == (wdPlaneT::ComponentType)3 && p->m_fNegDistance == (wdPlaneT::ComponentType)4);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(Normal, Point)")
  {
    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(5, 3, 1));

    WD_TEST_BOOL(p.m_vNormal == wdVec3T(1, 0, 0));
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(Point, Point, Point)")
  {
    wdPlaneT p(wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5));

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(Points)")
  {
    wdVec3T v[3] = {wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5)};

    wdPlaneT p(v);

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(Points, numpoints)")
  {
    wdVec3T v[6] = {wdVec3T(-1, 5, 1), wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5), wdVec3T(0, 5, -5)};

    wdPlaneT p(v, 6);

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromNormalAndPoint")
  {
    wdPlaneT p;
    p.SetFromNormalAndPoint(wdVec3T(1, 0, 0), wdVec3T(5, 3, 1));

    WD_TEST_BOOL(p.m_vNormal == wdVec3T(1, 0, 0));
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdPlaneT p;
    p.SetFromPoints(wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5)).IgnoreResult();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdVec3T v[3] = {wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5)};

    wdPlaneT p;
    p.SetFromPoints(v).IgnoreResult();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdVec3T v[6] = {wdVec3T(-1, 5, 1), wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5), wdVec3T(0, 5, -5)};

    wdPlaneT p;
    p.SetFromPoints(v, 6).IgnoreResult();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromDirections")
  {
    wdPlaneT p;
    p.SetFromDirections(wdVec3T(1, 0, 0), wdVec3T(1, 0, -1), wdVec3T(3, 5, 9)).IgnoreResult();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -5.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid")
  {
    wdPlaneT p;
    p.SetFromDirections(wdVec3T(1, 0, 0), wdVec3T(1, 0, -1), wdVec3T(3, 5, 9)).IgnoreResult();

    p.SetInvalid();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 0, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, 0.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo")
  {
    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(5, 0, 0));

    WD_TEST_FLOAT(p.GetDistanceTo(wdVec3T(10, 3, 5)), 5.0f, 0.0001f);
    WD_TEST_FLOAT(p.GetDistanceTo(wdVec3T(0, 7, 123)), -5.0f, 0.0001f);
    WD_TEST_FLOAT(p.GetDistanceTo(wdVec3T(5, 12, 23)), 0.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetMinimumDistanceTo")
  {
    wdVec3T v1[3] = {wdVec3T(15, 3, 5), wdVec3T(6, 7, 123), wdVec3T(10, 12, 23)};
    wdVec3T v2[3] = {wdVec3T(3, 3, 5), wdVec3T(5, 7, 123), wdVec3T(10, 12, 23)};

    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(5, 0, 0));

    WD_TEST_FLOAT(p.GetMinimumDistanceTo(v1, 3), 1.0f, 0.0001f);
    WD_TEST_FLOAT(p.GetMinimumDistanceTo(v2, 3), -2.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetMinMaxDistanceTo")
  {
    wdVec3T v1[3] = {wdVec3T(15, 3, 5), wdVec3T(5, 7, 123), wdVec3T(0, 12, 23)};
    wdVec3T v2[3] = {wdVec3T(8, 3, 5), wdVec3T(6, 7, 123), wdVec3T(10, 12, 23)};

    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(5, 0, 0));

    wdMathTestType fmin, fmax;

    p.GetMinMaxDistanceTo(fmin, fmax, v1, 3);
    WD_TEST_FLOAT(fmin, -5.0f, 0.0001f);
    WD_TEST_FLOAT(fmax, 10.0f, 0.0001f);

    p.GetMinMaxDistanceTo(fmin, fmax, v2, 3);
    WD_TEST_FLOAT(fmin, 1, 0.0001f);
    WD_TEST_FLOAT(fmax, 5, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetPointPosition")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_BOOL(p.GetPointPosition(wdVec3T(0, 15, 0)) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetPointPosition(wdVec3T(0, 5, 0)) == wdPositionOnPlane::Back);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetPointPosition(planewidth)")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_BOOL(p.GetPointPosition(wdVec3T(0, 15, 0), 0.01f) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetPointPosition(wdVec3T(0, 5, 0), 0.01f) == wdPositionOnPlane::Back);
    WD_TEST_BOOL(p.GetPointPosition(wdVec3T(0, 10, 0), 0.01f) == wdPositionOnPlane::OnPlane);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetObjectPosition")
  {
    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(10, 0, 0));

    wdVec3T v0[3] = {wdVec3T(12, 0, 0), wdVec3T(15, 0, 0), wdVec3T(20, 0, 0)};
    wdVec3T v1[3] = {wdVec3T(8, 0, 0), wdVec3T(6, 0, 0), wdVec3T(4, 0, 0)};
    wdVec3T v2[3] = {wdVec3T(12, 0, 0), wdVec3T(6, 0, 0), wdVec3T(4, 0, 0)};

    WD_TEST_BOOL(p.GetObjectPosition(v0, 3) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetObjectPosition(v1, 3) == wdPositionOnPlane::Back);
    WD_TEST_BOOL(p.GetObjectPosition(v2, 3) == wdPositionOnPlane::Spanning);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetObjectPosition(fPlaneHalfWidth)")
  {
    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(10, 0, 0));

    wdVec3T v0[3] = {wdVec3T(12, 0, 0), wdVec3T(15, 0, 0), wdVec3T(20, 0, 0)};
    wdVec3T v1[3] = {wdVec3T(8, 0, 0), wdVec3T(6, 0, 0), wdVec3T(4, 0, 0)};
    wdVec3T v2[3] = {wdVec3T(12, 0, 0), wdVec3T(6, 0, 0), wdVec3T(4, 0, 0)};
    wdVec3T v3[3] = {wdVec3T(10, 1, 0), wdVec3T(10, 5, 7), wdVec3T(10, 3, -5)};

    WD_TEST_BOOL(p.GetObjectPosition(v0, 3, 0.001f) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetObjectPosition(v1, 3, 0.001f) == wdPositionOnPlane::Back);
    WD_TEST_BOOL(p.GetObjectPosition(v2, 3, 0.001f) == wdPositionOnPlane::Spanning);
    WD_TEST_BOOL(p.GetObjectPosition(v3, 3, 0.001f) == wdPositionOnPlane::OnPlane);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetObjectPosition(sphere)")
  {
    wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(10, 0, 0));

    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(15, 2, 3), 3.0f)) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(5, 2, 3), 3.0f)) == wdPositionOnPlane::Back);
    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(15, 2, 4.999f), 3.0f)) == wdPositionOnPlane::Front);
    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(5, 2, 3), 4.999f)) == wdPositionOnPlane::Back);
    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(8, 2, 3), 3.0f)) == wdPositionOnPlane::Spanning);
    WD_TEST_BOOL(p.GetObjectPosition(wdBoundingSphereT(wdVec3T(12, 2, 3), 3.0f)) == wdPositionOnPlane::Spanning);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetObjectPosition(box)")
  {
    {
      wdPlaneT p(wdVec3T(1, 0, 0), wdVec3T(10, 0, 0));
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(10.1f), wdVec3T(15))) == wdPositionOnPlane::Front);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(9.9f))) == wdPositionOnPlane::Back);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(15))) == wdPositionOnPlane::Spanning);
    }
    {
      wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(10.1f), wdVec3T(15))) == wdPositionOnPlane::Front);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(9.9f))) == wdPositionOnPlane::Back);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(15))) == wdPositionOnPlane::Spanning);
    }
    {
      wdPlaneT p(wdVec3T(0, 0, 1), wdVec3T(0, 0, 10));
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(10.1f), wdVec3T(15))) == wdPositionOnPlane::Front);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(9.9f))) == wdPositionOnPlane::Back);
      WD_TEST_BOOL(p.GetObjectPosition(wdBoundingBoxT(wdVec3T(7), wdVec3T(15))) == wdPositionOnPlane::Spanning);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ProjectOntoPlane")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_VEC3(p.ProjectOntoPlane(wdVec3T(3, 15, 2)), wdVec3T(3, 10, 2), 0.001f);
    WD_TEST_VEC3(p.ProjectOntoPlane(wdVec3T(-1, 5, -5)), wdVec3T(-1, 10, -5), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Mirror")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_VEC3(p.Mirror(wdVec3T(3, 15, 2)), wdVec3T(3, 5, 2), 0.001f);
    WD_TEST_VEC3(p.Mirror(wdVec3T(-1, 5, -5)), wdVec3T(-1, 15, -5), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCoplanarDirection")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_VEC3(p.GetCoplanarDirection(wdVec3T(0, 1, 0)), wdVec3T(0, 0, 0), 0.001f);
    WD_TEST_VEC3(p.GetCoplanarDirection(wdVec3T(1, 1, 0)).GetNormalized(), wdVec3T(1, 0, 0), 0.001f);
    WD_TEST_VEC3(p.GetCoplanarDirection(wdVec3T(-1, 1, 0)).GetNormalized(), wdVec3T(-1, 0, 0), 0.001f);
    WD_TEST_VEC3(p.GetCoplanarDirection(wdVec3T(0, 1, 1)).GetNormalized(), wdVec3T(0, 0, 1), 0.001f);
    WD_TEST_VEC3(p.GetCoplanarDirection(wdVec3T(0, 1, -1)).GetNormalized(), wdVec3T(0, 0, -1), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical / operator== / operator!=")
  {
    wdPlaneT p1(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
    wdPlaneT p2(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
    wdPlaneT p3(wdVec3T(0, 1, 0), wdVec3T(0, 10.00001f, 0));

    WD_TEST_BOOL(p1.IsIdentical(p1));
    WD_TEST_BOOL(p2.IsIdentical(p2));
    WD_TEST_BOOL(p3.IsIdentical(p3));

    WD_TEST_BOOL(p1.IsIdentical(p2));
    WD_TEST_BOOL(p2.IsIdentical(p1));

    WD_TEST_BOOL(!p1.IsIdentical(p3));
    WD_TEST_BOOL(!p2.IsIdentical(p3));


    WD_TEST_BOOL(p1 == p2);
    WD_TEST_BOOL(p2 == p1);

    WD_TEST_BOOL(p1 != p3);
    WD_TEST_BOOL(p2 != p3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdPlaneT p1(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
    wdPlaneT p2(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
    wdPlaneT p3(wdVec3T(0, 1, 0), wdVec3T(0, 10.00001f, 0));

    WD_TEST_BOOL(p1.IsEqual(p1));
    WD_TEST_BOOL(p2.IsEqual(p2));
    WD_TEST_BOOL(p3.IsEqual(p3));

    WD_TEST_BOOL(p1.IsEqual(p2));
    WD_TEST_BOOL(p2.IsEqual(p1));

    WD_TEST_BOOL(p1.IsEqual(p3));
    WD_TEST_BOOL(p2.IsEqual(p3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
  {
    wdPlaneT p1(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_BOOL(p1.IsValid());

    p1.SetInvalid();
    WD_TEST_BOOL(!p1.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform(Mat3)")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    wdMat3T m;
    m.SetRotationMatrixX(wdAngle::Degree(90));

    p.Transform(m);

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 0, 1), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform(Mat4)")
  {
    {
      wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

      wdMat4T m;
      m.SetRotationMatrixX(wdAngle::Degree(90));
      m.SetTranslationVector(wdVec3T(0, 5, 0));

      p.Transform(m);

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 0, 1), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);
    }

    {
      wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

      wdMat4T m;
      m.SetRotationMatrixX(wdAngle::Degree(90));
      m.SetTranslationVector(wdVec3T(0, 0, 5));

      p.Transform(m);

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 0, 1), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, -15.0f, 0.0001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Flip")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);

    p.Flip();

    WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, -1, 0), 0.0001f);
    WD_TEST_FLOAT(p.m_fNegDistance, 10.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FlipIfNecessary")
  {
    {
      wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);

      WD_TEST_BOOL(p.FlipIfNecessary(wdVec3T(0, 11, 0), true) == false);

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);
    }

    {
      wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, 1, 0), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, -10.0f, 0.0001f);

      WD_TEST_BOOL(p.FlipIfNecessary(wdVec3T(0, 11, 0), false) == true);

      WD_TEST_VEC3(p.m_vNormal, wdVec3T(0, -1, 0), 0.0001f);
      WD_TEST_FLOAT(p.m_fNegDistance, 10.0f, 0.0001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRayIntersection")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    wdMathTestType f;
    wdVec3T v;

    WD_TEST_BOOL(p.GetRayIntersection(wdVec3T(3, 1, 7), wdVec3T(0, 1, 0), &f, &v));
    WD_TEST_FLOAT(f, 9, 0.0001f);
    WD_TEST_VEC3(v, wdVec3T(3, 10, 7), 0.0001f);

    WD_TEST_BOOL(p.GetRayIntersection(wdVec3T(3, 20, 7), wdVec3T(0, -1, 0), &f, &v));
    WD_TEST_FLOAT(f, 10, 0.0001f);
    WD_TEST_VEC3(v, wdVec3T(3, 10, 7), 0.0001f);

    WD_TEST_BOOL(!p.GetRayIntersection(wdVec3T(3, 1, 7), wdVec3T(1, 0, 0), &f, &v));
    WD_TEST_BOOL(!p.GetRayIntersection(wdVec3T(3, 1, 7), wdVec3T(0, -1, 0), &f, &v));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRayIntersectionBiDirectional")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    wdMathTestType f;
    wdVec3T v;

    WD_TEST_BOOL(p.GetRayIntersectionBiDirectional(wdVec3T(3, 1, 7), wdVec3T(0, 1, 0), &f, &v));
    WD_TEST_FLOAT(f, 9, 0.0001f);
    WD_TEST_VEC3(v, wdVec3T(3, 10, 7), 0.0001f);

    WD_TEST_BOOL(!p.GetRayIntersectionBiDirectional(wdVec3T(3, 1, 7), wdVec3T(1, 0, 0), &f, &v));

    WD_TEST_BOOL(p.GetRayIntersectionBiDirectional(wdVec3T(3, 1, 7), wdVec3T(0, -1, 0), &f, &v));
    WD_TEST_FLOAT(f, -9, 0.0001f);
    WD_TEST_VEC3(v, wdVec3T(3, 10, 7), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLineSegmentIntersection")
  {
    wdPlaneT p(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));

    wdMathTestType f;
    wdVec3T v;

    WD_TEST_BOOL(p.GetLineSegmentIntersection(wdVec3T(3, 5, 7), wdVec3T(3, 15, 7), &f, &v));
    WD_TEST_FLOAT(f, 0.5f, 0.0001f);
    WD_TEST_VEC3(v, wdVec3T(3, 10, 7), 0.0001f);

    WD_TEST_BOOL(!p.GetLineSegmentIntersection(wdVec3T(3, 5, 7), wdVec3T(13, 5, 7), &f, &v));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetPlanesIntersectionPoint")
  {
    wdPlaneT p1(wdVec3T(1, 0, 0), wdVec3T(0, 10, 0));
    wdPlaneT p2(wdVec3T(0, 1, 0), wdVec3T(0, 10, 0));
    wdPlaneT p3(wdVec3T(0, 0, 1), wdVec3T(0, 10, 0));

    wdVec3T r;

    WD_TEST_BOOL(wdPlaneT::GetPlanesIntersectionPoint(p1, p2, p3, r) == WD_SUCCESS);
    WD_TEST_VEC3(r, wdVec3T(0, 10, 0), 0.0001f);

    WD_TEST_BOOL(wdPlaneT::GetPlanesIntersectionPoint(p1, p1, p3, r) == WD_FAILURE);
    WD_TEST_BOOL(wdPlaneT::GetPlanesIntersectionPoint(p1, p2, p2, r) == WD_FAILURE);
    WD_TEST_BOOL(wdPlaneT::GetPlanesIntersectionPoint(p3, p2, p3, r) == WD_FAILURE);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindSupportPoints")
  {
    wdVec3T v[6] = {wdVec3T(-1, 5, 1), wdVec3T(-1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(1, 5, 1), wdVec3T(0, 5, -5), wdVec3T(0, 5, -5)};

    wdInt32 i1, i2, i3;

    wdPlaneT::FindSupportPoints(v, 6, i1, i2, i3).IgnoreResult();

    WD_TEST_INT(i1, 0);
    WD_TEST_INT(i2, 2);
    WD_TEST_INT(i3, 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdPlaneT p;

      p.SetInvalid();
      WD_TEST_BOOL(!p.IsNaN());

      p.SetInvalid();
      p.m_fNegDistance = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(p.IsNaN());

      p.SetInvalid();
      p.m_vNormal.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(p.IsNaN());

      p.SetInvalid();
      p.m_vNormal.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(p.IsNaN());

      p.SetInvalid();
      p.m_vNormal.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(p.IsNaN());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsFinite")
  {
    if (wdMath::SupportsInfinity<wdMathTestType>())
    {
      wdPlaneT p;

      p.m_vNormal = wdVec3(1, 2, 3).GetNormalized();
      p.m_fNegDistance = 42;
      WD_TEST_BOOL(p.IsValid());
      WD_TEST_BOOL(p.IsFinite());

      p.SetInvalid();
      p.m_vNormal = wdVec3(1, 2, 3).GetNormalized();
      p.m_fNegDistance = wdMath::Infinity<wdMathTestType>();
      WD_TEST_BOOL(p.IsValid());
      WD_TEST_BOOL(!p.IsFinite());

      p.SetInvalid();
      p.m_vNormal.x = wdMath::NaN<wdMathTestType>();
      p.m_fNegDistance = wdMath::Infinity<wdMathTestType>();
      WD_TEST_BOOL(!p.IsValid());
      WD_TEST_BOOL(!p.IsFinite());

      p.SetInvalid();
      p.m_vNormal = wdVec3(1, 2, 3);
      p.m_fNegDistance = 42;
      WD_TEST_BOOL(!p.IsValid());
      WD_TEST_BOOL(p.IsFinite());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetMinimumDistanceTo/GetMaximumDistanceTo")
  {
    const wdUInt32 numTestLoops = 1000 * 1000;

    wdRandom randomGenerator;
    randomGenerator.Initialize(0x83482343);

    const auto randomNonZeroVec3T = [&randomGenerator]() -> wdVec3T {
      const float extent = 1000.f;
      const wdVec3T v(randomGenerator.FloatMinMax(-extent, extent), randomGenerator.FloatMinMax(-extent, extent), randomGenerator.FloatMinMax(-extent, extent));
      return v.GetLength() > 0.001f ? v : wdVec3T::UnitXAxis();
    };

    for (wdUInt32 loopIndex = 0; loopIndex < numTestLoops; ++loopIndex)
    {
      const wdPlaneT plane(randomNonZeroVec3T().GetNormalized(), randomNonZeroVec3T());

      wdVec3T boxCorners[8];
      wdBoundingBoxT box;
      {
        const wdVec3T boxPoint0 = randomNonZeroVec3T();
        const wdVec3T boxPoint1 = randomNonZeroVec3T();
        const wdVec3T boxMins(wdMath::Min(boxPoint0.x, boxPoint1.x), wdMath::Min(boxPoint0.y, boxPoint1.y), wdMath::Min(boxPoint0.z, boxPoint1.z));
        const wdVec3T boxMaxs(wdMath::Max(boxPoint0.x, boxPoint1.x), wdMath::Max(boxPoint0.y, boxPoint1.y), wdMath::Max(boxPoint0.z, boxPoint1.z));
        box = wdBoundingBoxT(boxMins, boxMaxs);
        box.GetCorners(boxCorners);
      }

      float distanceMin;
      float distanceMax;
      {
        distanceMin = plane.GetMinimumDistanceTo(box);
        distanceMax = plane.GetMaximumDistanceTo(box);
      }

      float referenceDistanceMin = FLT_MAX;
      float referenceDistanceMax = -FLT_MAX;
      {
        for (wdUInt32 cornerIndex = 0; cornerIndex < WD_ARRAY_SIZE(boxCorners); ++cornerIndex)
        {
          const float cornerDist = plane.GetDistanceTo(boxCorners[cornerIndex]);
          referenceDistanceMin = wdMath::Min(referenceDistanceMin, cornerDist);
          referenceDistanceMax = wdMath::Max(referenceDistanceMax, cornerDist);
        }
      }

      // Break at first error to not spam the log with other potential error (the loop here is very long)
      {
        bool currIterSucceeded = true;
        currIterSucceeded = currIterSucceeded && WD_TEST_FLOAT(distanceMin, referenceDistanceMin, 0.0001f);
        currIterSucceeded = currIterSucceeded && WD_TEST_FLOAT(distanceMax, referenceDistanceMax, 0.0001f);
        if (!currIterSucceeded)
        {
          break;
        }
      }
    }
  }
}
