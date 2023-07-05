#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBox.h>
#include <Foundation/Math/Transform.h>
#include <Foundation/SimdMath/SimdBBox.h>
#include <Foundation/SimdMath/SimdConversion.h>

#define WD_TEST_SIMD_VECTOR_EQUAL(NUM_COMPONENTS, A, B, EPSILON)                                                                                               \
  do                                                                                                                                                           \
  {                                                                                                                                                            \
    auto _wdDiff = B - A;                                                                                                                                      \
    wdTestBool((A).IsEqual((B), EPSILON).AllSet<NUM_COMPONENTS>(), "Test failed: " WD_STRINGIZE(A) ".IsEqual(" WD_STRINGIZE(B) ", " WD_STRINGIZE(EPSILON) ")", \
      WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION,                                                                                                      \
      "Difference %lf %lf %lf %lf", _wdDiff.x(), _wdDiff.y(), _wdDiff.z(), _wdDiff.w());                                                                       \
  } while (false)


WD_CREATE_SIMPLE_TEST(SimdMath, SimdBBox)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSimdBBox b(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(-1, -2, -3)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(1, 2, 3)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid")
  {
    wdSimdBBox b;
    b.SetInvalid();

    WD_TEST_BOOL(!b.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    wdSimdBBox b;

    b.SetInvalid();
    WD_TEST_BOOL(!b.IsNaN());

    b.SetInvalid();
    b.m_Min.SetX(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());

    b.SetInvalid();
    b.m_Min.SetY(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());

    b.SetInvalid();
    b.m_Min.SetZ(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());

    b.SetInvalid();
    b.m_Max.SetX(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());

    b.SetInvalid();
    b.m_Max.SetY(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());

    b.SetInvalid();
    b.m_Max.SetZ(wdMath::NaN<wdMathTestType>());
    WD_TEST_BOOL(b.IsNaN());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCenterAndHalfExtents")
  {
    wdSimdBBox b;
    b.SetCenterAndHalfExtents(wdSimdVec4f(1, 2, 3), wdSimdVec4f(4, 5, 6));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(-3, -3, -3)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(5, 7, 9)).AllSet<3>());

    WD_TEST_BOOL((b.GetCenter() == wdSimdVec4f(1, 2, 3)).AllSet<3>());
    WD_TEST_BOOL((b.GetExtents() == wdSimdVec4f(8, 10, 12)).AllSet<3>());
    WD_TEST_BOOL((b.GetHalfExtents() == wdSimdVec4f(4, 5, 6)).AllSet<3>());
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

    wdSimdBBox b;
    b.SetFromPoints(p, 6);

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(-4, -6, -8)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(5, 7, 9)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (Point)")
  {
    wdSimdBBox b;
    b.SetInvalid();
    b.ExpandToInclude(wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(1, 2, 3)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(1, 2, 3)).AllSet<3>());


    b.ExpandToInclude(wdSimdVec4f(2, 3, 4));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(1, 2, 3)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(2, 3, 4)).AllSet<3>());

    b.ExpandToInclude(wdSimdVec4f(0, 1, 2));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(0, 1, 2)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(2, 3, 4)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (array)")
  {
    wdSimdVec4f v[4] = {wdSimdVec4f(1, 1, 1), wdSimdVec4f(-1, -1, -1), wdSimdVec4f(2, 2, 2), wdSimdVec4f(4, 4, 4)};

    wdSimdBBox b;
    b.SetInvalid();
    b.ExpandToInclude(v, 2, sizeof(wdSimdVec4f) * 2);

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(1, 1, 1)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(2, 2, 2)).AllSet<3>());

    b.ExpandToInclude(v, 4);

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(-1, -1, -1)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(4, 4, 4)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (Box)")
  {
    wdSimdBBox b1(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3));
    wdSimdBBox b2(wdSimdVec4f(0), wdSimdVec4f(4, 5, 6));

    b1.ExpandToInclude(b2);

    WD_TEST_BOOL((b1.m_Min == wdSimdVec4f(-1, -2, -3)).AllSet<3>());
    WD_TEST_BOOL((b1.m_Max == wdSimdVec4f(4, 5, 6)).AllSet<3>());

    wdSimdBBox b3;
    b3.SetInvalid();
    b3.ExpandToInclude(b1);
    WD_TEST_BOOL(b3 == b1);

    b2.m_Min = wdSimdVec4f(-4, -5, -6);
    b2.m_Max.SetZero();

    b1.ExpandToInclude(b2);

    WD_TEST_BOOL((b1.m_Min == wdSimdVec4f(-4, -5, -6)).AllSet<3>());
    WD_TEST_BOOL((b1.m_Max == wdSimdVec4f(4, 5, 6)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToCube")
  {
    wdSimdBBox b;
    b.SetCenterAndHalfExtents(wdSimdVec4f(1, 2, 3), wdSimdVec4f(4, 5, 6));

    b.ExpandToCube();

    WD_TEST_BOOL((b.GetCenter() == wdSimdVec4f(1, 2, 3)).AllSet<3>());
    WD_TEST_BOOL((b.GetHalfExtents() == wdSimdVec4f(6, 6, 6)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Point)")
  {
    wdSimdBBox b(wdSimdVec4f(0), wdSimdVec4f(0));

    WD_TEST_BOOL(b.Contains(wdSimdVec4f(0)));
    WD_TEST_BOOL(!b.Contains(wdSimdVec4f(1, 0, 0)));
    WD_TEST_BOOL(!b.Contains(wdSimdVec4f(-1, 0, 0)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Box)")
  {
    wdSimdBBox b1(wdSimdVec4f(-3), wdSimdVec4f(3));
    wdSimdBBox b2(wdSimdVec4f(-1), wdSimdVec4f(1));
    wdSimdBBox b3(wdSimdVec4f(-1), wdSimdVec4f(4));

    WD_TEST_BOOL(b1.Contains(b1));
    WD_TEST_BOOL(b2.Contains(b2));
    WD_TEST_BOOL(b3.Contains(b3));

    WD_TEST_BOOL(b1.Contains(b2));
    WD_TEST_BOOL(!b1.Contains(b3));

    WD_TEST_BOOL(!b2.Contains(b1));
    WD_TEST_BOOL(!b2.Contains(b3));

    WD_TEST_BOOL(!b3.Contains(b1));
    WD_TEST_BOOL(b3.Contains(b2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Sphere)")
  {
    wdSimdBBox b(wdSimdVec4f(1), wdSimdVec4f(5));

    WD_TEST_BOOL(b.Contains(wdSimdBSphere(wdSimdVec4f(3), 2)));
    WD_TEST_BOOL(!b.Contains(wdSimdBSphere(wdSimdVec4f(3), 2.1f)));
    WD_TEST_BOOL(!b.Contains(wdSimdBSphere(wdSimdVec4f(8), 2)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (box)")
  {
    wdSimdBBox b1(wdSimdVec4f(-3), wdSimdVec4f(3));
    wdSimdBBox b2(wdSimdVec4f(-1), wdSimdVec4f(1));
    wdSimdBBox b3(wdSimdVec4f(1), wdSimdVec4f(4));
    wdSimdBBox b4(wdSimdVec4f(-4, 1, 1), wdSimdVec4f(4, 2, 2));

    WD_TEST_BOOL(b1.Overlaps(b1));
    WD_TEST_BOOL(b2.Overlaps(b2));
    WD_TEST_BOOL(b3.Overlaps(b3));
    WD_TEST_BOOL(b4.Overlaps(b4));

    WD_TEST_BOOL(b1.Overlaps(b2));
    WD_TEST_BOOL(b1.Overlaps(b3));
    WD_TEST_BOOL(b1.Overlaps(b4));

    WD_TEST_BOOL(!b2.Overlaps(b3));
    WD_TEST_BOOL(!b2.Overlaps(b4));

    WD_TEST_BOOL(b3.Overlaps(b4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (Sphere)")
  {
    wdSimdBBox b(wdSimdVec4f(1), wdSimdVec4f(5));

    WD_TEST_BOOL(b.Overlaps(wdSimdBSphere(wdSimdVec4f(3), 2)));
    WD_TEST_BOOL(b.Overlaps(wdSimdBSphere(wdSimdVec4f(3), 2.1f)));
    WD_TEST_BOOL(!b.Overlaps(wdSimdBSphere(wdSimdVec4f(8), 2)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Grow")
  {
    wdSimdBBox b(wdSimdVec4f(1, 2, 3), wdSimdVec4f(4, 5, 6));
    b.Grow(wdSimdVec4f(2, 4, 6));

    WD_TEST_BOOL((b.m_Min == wdSimdVec4f(-1, -2, -3)).AllSet<3>());
    WD_TEST_BOOL((b.m_Max == wdSimdVec4f(6, 9, 12)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform")
  {
    wdSimdBBox b(wdSimdVec4f(3), wdSimdVec4f(5));

    wdSimdTransform t(wdSimdVec4f(4, 5, 6));
    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
    t.m_Scale = wdSimdVec4f(1, -2, -4);

    b.Transform(t);

    WD_TEST_SIMD_VECTOR_EQUAL(3, b.m_Min, wdSimdVec4f(10, 8, -14), 0.00001f);
    WD_TEST_SIMD_VECTOR_EQUAL(3, b.m_Max, wdSimdVec4f(14, 10, -6), 0.00001f);

    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(-30));

    b.m_Min = wdSimdVec4f(3);
    b.m_Max = wdSimdVec4f(5);
    b.Transform(t);

    // reference
    wdBoundingBox referenceBox(wdVec3(3), wdVec3(5));
    {
      wdQuat q;
      q.SetFromAxisAndAngle(wdVec3(0, 0, 1), wdAngle::Degree(-30));

      wdTransform referenceTransform(wdVec3(4, 5, 6), q, wdVec3(1, -2, -4));

      referenceBox.TransformFromOrigin(referenceTransform.GetAsMat4());
    }

    WD_TEST_SIMD_VECTOR_EQUAL(3, b.m_Min, wdSimdConversion::ToVec3(referenceBox.m_vMin), 0.00001f);
    WD_TEST_SIMD_VECTOR_EQUAL(3, b.m_Max, wdSimdConversion::ToVec3(referenceBox.m_vMax), 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetClampedPoint")
  {
    wdSimdBBox b(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(-2, 0, 0)) == wdSimdVec4f(-1, 0, 0)).AllSet<3>());
    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(2, 0, 0)) == wdSimdVec4f(1, 0, 0)).AllSet<3>());

    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(0, -3, 0)) == wdSimdVec4f(0, -2, 0)).AllSet<3>());
    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(0, 3, 0)) == wdSimdVec4f(0, 2, 0)).AllSet<3>());

    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(0, 0, -4)) == wdSimdVec4f(0, 0, -3)).AllSet<3>());
    WD_TEST_BOOL((b.GetClampedPoint(wdSimdVec4f(0, 0, 4)) == wdSimdVec4f(0, 0, 3)).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceSquaredTo (point)")
  {
    wdSimdBBox b(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(-2, 0, 0)) == 1.0f);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(2, 0, 0)) == 1.0f);

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(0, -4, 0)) == 4.0f);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(0, 4, 0)) == 4.0f);

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(0, 0, -6)) == 9.0f);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdSimdVec4f(0, 0, 6)) == 9.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (point)")
  {
    wdSimdBBox b(wdSimdVec4f(-1, -2, -3), wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(-2, 0, 0)) == 1.0f);
    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(2, 0, 0)) == 1.0f);

    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(0, -4, 0)) == 2.0f);
    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(0, 4, 0)) == 2.0f);

    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(0, 0, -6)) == 3.0f);
    WD_TEST_BOOL(b.GetDistanceTo(wdSimdVec4f(0, 0, 6)) == 3.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdBBox b1(wdSimdVec4f(5, 0, 0), wdSimdVec4f(1, 2, 3));
    wdSimdBBox b2(wdSimdVec4f(6, 0, 0), wdSimdVec4f(1, 2, 3));

    WD_TEST_BOOL(b1 == wdSimdBBox(wdSimdVec4f(5, 0, 0), wdSimdVec4f(1, 2, 3)));
    WD_TEST_BOOL(b1 != b2);
  }
}
