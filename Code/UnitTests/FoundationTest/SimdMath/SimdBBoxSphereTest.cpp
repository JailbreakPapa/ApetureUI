#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBoxSphere.h>
#include <Foundation/Math/Random.h>
#include <Foundation/Math/Transform.h>
#include <Foundation/SimdMath/SimdBBoxSphere.h>
#include <Foundation/SimdMath/SimdConversion.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdBBoxSphere)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSimdBBoxSphere b(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3), 2);

    WD_TEST_BOOL((b.m_CenterAndRadius == wdSimdVec4f(-1, -2, -3, 2)).AllSet<4>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(1, 2, 3)).AllSet<3>());

    wdSimdBBox box(wdSimdVec4f(1, 1, 1), wdSimdVec4f(3, 3, 3));
    wdSimdBSphere sphere(wdSimdVec4f(2, 2, 2), 1);

    b = wdSimdBBoxSphere(box, sphere);

    WD_TEST_BOOL((b.m_CenterAndRadius == wdSimdVec4f(2, 2, 2, 1)).AllSet<4>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(1, 1, 1)).AllSet<3>());
    WD_TEST_BOOL(b.GetBox() == box);
    WD_TEST_BOOL(b.GetSphere() == sphere);

    b = wdSimdBBoxSphere(box);

    WD_TEST_BOOL(b.m_CenterAndRadius.IsEqual(wdSimdVec4f(2, 2, 2, wdMath::Sqrt(3.0f)), 0.00001f).AllSet<4>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(1, 1, 1)).AllSet<3>());
    WD_TEST_BOOL(b.GetBox() == box);

    b = wdSimdBBoxSphere(sphere);

    WD_TEST_BOOL((b.m_CenterAndRadius == wdSimdVec4f(2, 2, 2, 1)).AllSet<4>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(1, 1, 1)).AllSet<3>());
    WD_TEST_BOOL(b.GetSphere() == sphere);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid")
  {
    wdSimdBBoxSphere b;
    b.SetInvalid();

    WD_TEST_BOOL(!b.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<float>())
    {
      wdSimdBBoxSphere b;

      b.SetInvalid();
      WD_TEST_BOOL(!b.IsNaN());

      b.SetInvalid();
      b.m_CenterAndRadius.SetX(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_CenterAndRadius.SetY(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_CenterAndRadius.SetZ(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_CenterAndRadius.SetW(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_BoxHalfExtents.SetX(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_BoxHalfExtents.SetY(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_BoxHalfExtents.SetZ(wdMath::NaN<float>());
      WD_TEST_BOOL(b.IsNaN());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdSimdVec4f p[6] = {
      wdSimdVec4f(-4, 0, 0),
      wdSimdVec4f(5, 0, 0),
      wdSimdVec4f(0, -6, 0),
      wdSimdVec4f(0, 7, 0),
      wdSimdVec4f(0, 0, -8),
      wdSimdVec4f(0, 0, 9),
    };

    wdSimdBBoxSphere b;
    b.SetFromPoints(p, 6);

    WD_TEST_BOOL((b.m_CenterAndRadius == wdSimdVec4f(0.5, 0.5, 0.5)).AllSet<3>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(4.5, 6.5, 8.5)).AllSet<3>());
    WD_TEST_BOOL(b.m_CenterAndRadius.w().IsEqual(wdSimdVec4f(0.5, 0.5, 8.5).GetLength<3>(), 0.00001f));
    WD_TEST_BOOL(b.m_CenterAndRadius.w() <= b.m_BoxHalfExtents.GetLength<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude")
  {
    wdSimdBBoxSphere b1;
    b1.SetInvalid();
    wdSimdBBoxSphere b2(wdSimdBBox(wdSimdVec4f(2, 2, 2), wdSimdVec4f(4, 4, 4)));

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 == b2);

    wdSimdBSphere sphere(wdSimdVec4f(2, 2, 2), 2);
    b2 = wdSimdBBoxSphere(sphere);

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 != b2);

    WD_TEST_BOOL((b1.m_CenterAndRadius == wdSimdVec4f(2, 2, 2)).AllSet<3>());
    WD_TEST_BOOL((b1.m_BoxHalfExtents == wdSimdVec4f(2, 2, 2)).AllSet<3>());
    WD_TEST_FLOAT(b1.m_CenterAndRadius.w(), wdMath::Sqrt(3.0f) * 2.0f, 0.00001f);
    WD_TEST_BOOL(b1.m_CenterAndRadius.w() <= b1.m_BoxHalfExtents.GetLength<3>());

    b1.SetInvalid();
    b2 = wdSimdBBox(wdSimdVec4f(0.25, 0.25, 0.25), wdSimdVec4f(0.5, 0.5, 0.5));

    b1.ExpandToInclude(b2);
    WD_TEST_BOOL(b1 == b2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform")
  {
    wdSimdBBoxSphere b(wdSimdVec4f(1), wdSimdVec4f(5), 5);

    wdSimdTransform t(wdSimdVec4f(1, 1, 1), wdSimdQuat::IdentityQuaternion(), wdSimdVec4f(2, 3, -2));

    b.Transform(t);

    WD_TEST_BOOL((b.m_CenterAndRadius == wdSimdVec4f(3, 4, -1, 15)).AllSet<4>());
    WD_TEST_BOOL((b.m_BoxHalfExtents == wdSimdVec4f(10, 15, 10)).AllSet<3>());

    // verification
    wdRandom rnd;
    rnd.Initialize(0x736454);

    wdDynamicArray<wdSimdVec4f, wdAlignedAllocatorWrapper> points;
    points.SetCountUninitialized(10);
    float fSize = 10;

    for (wdUInt32 i = 0; i < points.GetCount(); ++i)
    {
      float x = (float)rnd.DoubleMinMax(-fSize, fSize);
      float y = (float)rnd.DoubleMinMax(-fSize, fSize);
      float z = (float)rnd.DoubleMinMax(-fSize, fSize);
      points[i] = wdSimdVec4f(x, y, z);
    }

    b.SetFromPoints(points.GetData(), points.GetCount());

    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(-30));
    b.Transform(t);

    for (wdUInt32 i = 0; i < points.GetCount(); ++i)
    {
      wdSimdVec4f tp = t.TransformPosition(points[i]);

      wdSimdFloat boxDist = b.GetBox().GetDistanceTo(tp);
      WD_TEST_BOOL(boxDist < wdMath::DefaultEpsilon<float>());

      wdSimdFloat sphereDist = b.GetSphere().GetDistanceTo(tp);
      WD_TEST_BOOL(sphereDist < wdMath::DefaultEpsilon<float>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdBBoxSphere b1(wdSimdBBox(wdSimdVec4f(5, 0, 0), wdSimdVec4f(1, 2, 3)));
    wdSimdBBoxSphere b2(wdSimdBBox(wdSimdVec4f(6, 0, 0), wdSimdVec4f(1, 2, 3)));

    WD_TEST_BOOL(b1 == wdSimdBBoxSphere(wdSimdBBox(wdSimdVec4f(5, 0, 0), wdSimdVec4f(1, 2, 3))));
    WD_TEST_BOOL(b1 != b2);
  }
}
