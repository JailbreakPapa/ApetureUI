#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdBSphere.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdBSphere)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdSimdBSphere s(wdSimdVec4f(1, 2, 3), 4);

    WD_TEST_BOOL((s.m_CenterAndRadius == wdSimdVec4f(1, 2, 3, 4)).AllSet());

    WD_TEST_BOOL((s.GetCenter() == wdSimdVec4f(1, 2, 3)).AllSet<3>());
    WD_TEST_BOOL(s.GetRadius() == 4.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid / IsValid")
  {
    wdSimdBSphere s(wdSimdVec4f(1, 2, 3), 4);

    WD_TEST_BOOL(s.IsValid());

    s.SetInvalid();

    WD_TEST_BOOL(!s.IsValid());
    WD_TEST_BOOL(!s.IsNaN());

    s = wdSimdBSphere(wdSimdVec4f(1, 2, 3), wdMath::NaN<float>());
    WD_TEST_BOOL(s.IsNaN());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude(Point)")
  {
    wdSimdBSphere s(wdSimdVec4f::ZeroVector(), 0.0f);

    s.ExpandToInclude(wdSimdVec4f(3, 0, 0));

    WD_TEST_BOOL((s.m_CenterAndRadius == wdSimdVec4f(0, 0, 0, 3)).AllSet());

    s.SetInvalid();

    s.ExpandToInclude(wdSimdVec4f(0.25, 0, 0));

    WD_TEST_BOOL((s.m_CenterAndRadius == wdSimdVec4f(0, 0, 0, 0.25)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude(array)")
  {
    wdSimdBSphere s(wdSimdVec4f(2, 2, 0), 0.0f);

    wdSimdVec4f p[4] = {wdSimdVec4f(0, 2, 0), wdSimdVec4f(4, 2, 0), wdSimdVec4f(2, 0, 0), wdSimdVec4f(2, 4, 0)};

    s.ExpandToInclude(p, 4);

    WD_TEST_BOOL((s.m_CenterAndRadius == wdSimdVec4f(2, 2, 0, 2)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (sphere)")
  {
    wdSimdBSphere s1(wdSimdVec4f(5, 0, 0), 1);
    wdSimdBSphere s2(wdSimdVec4f(6, 0, 0), 1);
    wdSimdBSphere s3(wdSimdVec4f(5, 0, 0), 2);

    s1.ExpandToInclude(s2);
    WD_TEST_BOOL((s1.m_CenterAndRadius == wdSimdVec4f(5, 0, 0, 2)).AllSet());

    s1.ExpandToInclude(s3);
    WD_TEST_BOOL((s1.m_CenterAndRadius == wdSimdVec4f(5, 0, 0, 2)).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Transform")
  {
    wdSimdBSphere s(wdSimdVec4f(5, 0, 0), 2);

    wdSimdTransform t(wdSimdVec4f(4, 5, 6));
    t.m_Rotation.SetFromAxisAndAngle(wdSimdVec4f(0, 0, 1), wdAngle::Degree(90));
    t.m_Scale = wdSimdVec4f(1, -2, -4);

    s.Transform(t);
    WD_TEST_BOOL(s.m_CenterAndRadius.IsEqual(wdSimdVec4f(4, 10, 6, 8), wdSimdFloat(wdMath::SmallEpsilon<float>())).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (point)")
  {
    wdSimdBSphere s(wdSimdVec4f(5, 0, 0), 2);

    WD_TEST_BOOL(s.GetDistanceTo(wdSimdVec4f(5, 0, 0)) == -2.0f);
    WD_TEST_BOOL(s.GetDistanceTo(wdSimdVec4f(7, 0, 0)) == 0.0f);
    WD_TEST_BOOL(s.GetDistanceTo(wdSimdVec4f(9, 0, 0)) == 2.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (sphere)")
  {
    wdSimdBSphere s1(wdSimdVec4f(5, 0, 0), 2);
    wdSimdBSphere s2(wdSimdVec4f(10, 0, 0), 3);
    wdSimdBSphere s3(wdSimdVec4f(10, 0, 0), 1);

    WD_TEST_BOOL(s1.GetDistanceTo(s2) == 0.0f);
    WD_TEST_BOOL(s1.GetDistanceTo(s3) == 2.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (point)")
  {
    wdSimdBSphere s(wdSimdVec4f(5, 0, 0), 2.0f);

    WD_TEST_BOOL(s.Contains(wdSimdVec4f(3, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdSimdVec4f(5, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdSimdVec4f(6, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdSimdVec4f(7, 0, 0)));

    WD_TEST_BOOL(!s.Contains(wdSimdVec4f(2, 0, 0)));
    WD_TEST_BOOL(!s.Contains(wdSimdVec4f(8, 0, 0)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (sphere)")
  {
    wdSimdBSphere s1(wdSimdVec4f(5, 0, 0), 2);
    wdSimdBSphere s2(wdSimdVec4f(6, 0, 0), 1);
    wdSimdBSphere s3(wdSimdVec4f(6, 0, 0), 2);

    WD_TEST_BOOL(s1.Contains(s1));
    WD_TEST_BOOL(s2.Contains(s2));
    WD_TEST_BOOL(s3.Contains(s3));

    WD_TEST_BOOL(s1.Contains(s2));
    WD_TEST_BOOL(!s1.Contains(s3));

    WD_TEST_BOOL(!s2.Contains(s3));
    WD_TEST_BOOL(s3.Contains(s2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (sphere)")
  {
    wdSimdBSphere s1(wdSimdVec4f(5, 0, 0), 2);
    wdSimdBSphere s2(wdSimdVec4f(6, 0, 0), 2);
    wdSimdBSphere s3(wdSimdVec4f(8, 0, 0), 1);

    WD_TEST_BOOL(s1.Overlaps(s1));
    WD_TEST_BOOL(s2.Overlaps(s2));
    WD_TEST_BOOL(s3.Overlaps(s3));

    WD_TEST_BOOL(s1.Overlaps(s2));
    WD_TEST_BOOL(!s1.Overlaps(s3));

    WD_TEST_BOOL(s2.Overlaps(s3));
    WD_TEST_BOOL(s3.Overlaps(s2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetClampedPoint")
  {
    wdSimdBSphere s(wdSimdVec4f(1, 2, 3), 2.0f);

    WD_TEST_BOOL(s.GetClampedPoint(wdSimdVec4f(2, 2, 3)).IsEqual(wdSimdVec4f(2, 2, 3), 0.001f).AllSet<3>());
    WD_TEST_BOOL(s.GetClampedPoint(wdSimdVec4f(5, 2, 3)).IsEqual(wdSimdVec4f(3, 2, 3), 0.001f).AllSet<3>());
    WD_TEST_BOOL(s.GetClampedPoint(wdSimdVec4f(1, 7, 3)).IsEqual(wdSimdVec4f(1, 4, 3), 0.001f).AllSet<3>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdBSphere s1(wdSimdVec4f(5, 0, 0), 2);
    wdSimdBSphere s2(wdSimdVec4f(6, 0, 0), 1);

    WD_TEST_BOOL(s1 == wdSimdBSphere(wdSimdVec4f(5, 0, 0), 2));
    WD_TEST_BOOL(s1 != s2);
  }
}
