#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBoxSphere.h>

WD_CREATE_SIMPLE_TEST(Math, BoundingBoxSphere)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdBoundingBoxSphereT b(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3), 2);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(b.m_fSphereRadius == 2);

    wdBoundingBoxT box(wdVec3T(1, 1, 1), wdVec3T(3, 3, 3));
    wdBoundingSphereT sphere(wdVec3T(2, 2, 2), 1);

    b = wdBoundingBoxSphereT(box, sphere);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(2, 2, 2));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(1, 1, 1));
    WD_TEST_BOOL(b.m_fSphereRadius == 1);
    WD_TEST_BOOL(b.GetBox() == box);
    WD_TEST_BOOL(b.GetSphere() == sphere);

    b = wdBoundingBoxSphereT(box);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(2, 2, 2));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(1, 1, 1));
    WD_TEST_FLOAT(b.m_fSphereRadius, wdMath::Sqrt(wdMathTestType(3)), 0.00001f);
    WD_TEST_BOOL(b.GetBox() == box);

    b = wdBoundingBoxSphereT(sphere);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(2, 2, 2));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(1, 1, 1));
    WD_TEST_BOOL(b.m_fSphereRadius == 1);
    WD_TEST_BOOL(b.GetSphere() == sphere);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdVec3T p[6] = {
      wdVec3T(-4, 0, 0),
      wdVec3T(5, 0, 0),
      wdVec3T(0, -6, 0),
      wdVec3T(0, 7, 0),
      wdVec3T(0, 0, -8),
      wdVec3T(0, 0, 9),
    };

    wdBoundingBoxSphereT b;
    b.SetFromPoints(p, 6);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(0.5, 0.5, 0.5));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(4.5, 6.5, 8.5));
    WD_TEST_FLOAT(b.m_fSphereRadius, wdVec3T(0.5, 0.5, 8.5).GetLength(), 0.00001f);
    WD_TEST_BOOL(b.m_fSphereRadius <= b.m_vBoxHalfExtends.GetLength());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid")
  {
    wdBoundingBoxSphereT b;
    b.SetInvalid();

    WD_TEST_BOOL(!b.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude")
  {
    wdBoundingBoxSphereT b1;
    b1.SetInvalid();
    wdBoundingBoxSphereT b2(wdBoundingBoxT(wdVec3T(2, 2, 2), wdVec3T(4, 4, 4)));

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 == b2);

    wdBoundingSphereT sphere(wdVec3T(2, 2, 2), 2);
    b2 = wdBoundingBoxSphereT(sphere);

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 != b2);

    WD_TEST_BOOL(b1.m_vCenter == wdVec3T(2, 2, 2));
    WD_TEST_BOOL(b1.m_vBoxHalfExtends == wdVec3T(2, 2, 2));
    WD_TEST_FLOAT(b1.m_fSphereRadius, wdMath::Sqrt(wdMathTestType(3)) * 2, 0.00001f);
    WD_TEST_BOOL(b1.m_fSphereRadius <= b1.m_vBoxHalfExtends.GetLength());

    b1.SetInvalid();
    b2 = wdBoundingBoxT(wdVec3T(0.25, 0.25, 0.25), wdVec3T(0.5, 0.5, 0.5));

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 == b2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform")
  {
    wdBoundingBoxSphereT b(wdVec3T(1), wdVec3T(5), 5);

    wdMat4T m;
    m.SetScalingMatrix(wdVec3T(-2, -3, -2));
    m.SetTranslationVector(wdVec3T(1, 1, 1));

    b.Transform(m);

    WD_TEST_BOOL(b.m_vCenter == wdVec3T(-1, -2, -1));
    WD_TEST_BOOL(b.m_vBoxHalfExtends == wdVec3T(10, 15, 10));
    WD_TEST_BOOL(b.m_fSphereRadius == 15);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdBoundingBoxSphereT b;

      b.SetInvalid();
      WD_TEST_BOOL(!b.IsNaN());

      b.SetInvalid();
      b.m_vCenter.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vCenter.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vCenter.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vBoxHalfExtends.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vBoxHalfExtends.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vBoxHalfExtends.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_fSphereRadius = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());
    }
  }
}
