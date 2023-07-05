#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBox.h>

WD_CREATE_SIMPLE_TEST(Math, BoundingBox)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(SetElements)")
  {
    wdBoundingBoxT b(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(1, 2, 3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetElements")
  {
    wdBoundingBoxT b;
    b.SetElements(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(1, 2, 3));
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

    wdBoundingBoxT b;
    b.SetFromPoints(p, 6);

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-4, -6, -8));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(5, 7, 9));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid")
  {
    wdBoundingBoxT b;
    b.SetInvalid();

    WD_TEST_BOOL(!b.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetCenterAndHalfExtents")
  {
    wdBoundingBoxT b;
    b.SetCenterAndHalfExtents(wdVec3T(1, 2, 3), wdVec3T(4, 5, 6));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-3, -3, -3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(5, 7, 9));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCorners")
  {
    wdBoundingBoxT b;
    b.SetElements(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    wdVec3T c[8];
    b.GetCorners(c);

    WD_TEST_BOOL(c[0] == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(c[1] == wdVec3T(-1, -2, 3));
    WD_TEST_BOOL(c[2] == wdVec3T(-1, 2, -3));
    WD_TEST_BOOL(c[3] == wdVec3T(-1, 2, 3));
    WD_TEST_BOOL(c[4] == wdVec3T(1, -2, -3));
    WD_TEST_BOOL(c[5] == wdVec3T(1, -2, 3));
    WD_TEST_BOOL(c[6] == wdVec3T(1, 2, -3));
    WD_TEST_BOOL(c[7] == wdVec3T(1, 2, 3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclue (Point)")
  {
    wdBoundingBoxT b;
    b.SetInvalid();
    b.ExpandToInclude(wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(1, 2, 3));


    b.ExpandToInclude(wdVec3T(2, 3, 4));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(2, 3, 4));

    b.ExpandToInclude(wdVec3T(0, 1, 2));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(0, 1, 2));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(2, 3, 4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (Box)")
  {
    wdBoundingBoxT b1, b2;

    b1.SetElements(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));
    b2.SetElements(wdVec3T(0), wdVec3T(4, 5, 6));

    b1.ExpandToInclude(b2);

    WD_TEST_BOOL(b1.m_vMin == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(b1.m_vMax == wdVec3T(4, 5, 6));

    b2.SetElements(wdVec3T(-4, -5, -6), wdVec3T(0));

    b1.ExpandToInclude(b2);

    WD_TEST_BOOL(b1.m_vMin == wdVec3T(-4, -5, -6));
    WD_TEST_BOOL(b1.m_vMax == wdVec3T(4, 5, 6));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (array)")
  {
    wdVec3T v[4] = {wdVec3T(1, 1, 1), wdVec3T(-1, -1, -1), wdVec3T(2, 2, 2), wdVec3T(4, 4, 4)};

    wdBoundingBoxT b;
    b.SetInvalid();
    b.ExpandToInclude(v, 2, sizeof(wdVec3T) * 2);

    WD_TEST_BOOL(b.m_vMin == wdVec3T(1, 1, 1));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(2, 2, 2));

    b.ExpandToInclude(v, 4, sizeof(wdVec3T));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-1, -1, -1));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(4, 4, 4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToCube")
  {
    wdBoundingBoxT b;
    b.SetCenterAndHalfExtents(wdVec3T(1, 2, 3), wdVec3T(4, 5, 6));

    b.ExpandToCube();

    WD_TEST_VEC3(b.GetCenter(), wdVec3T(1, 2, 3), wdMath::DefaultEpsilon<wdMathTestType>());
    WD_TEST_VEC3(b.GetHalfExtents(), wdVec3T(6, 6, 6), wdMath::DefaultEpsilon<wdMathTestType>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Grow")
  {
    wdBoundingBoxT b(wdVec3T(1, 2, 3), wdVec3T(4, 5, 6));
    b.Grow(wdVec3T(2, 4, 6));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-1, -2, -3));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(6, 9, 12));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Point)")
  {
    wdBoundingBoxT b(wdVec3T(0), wdVec3T(0));

    WD_TEST_BOOL(b.Contains(wdVec3T(0)));
    WD_TEST_BOOL(!b.Contains(wdVec3T(1, 0, 0)));
    WD_TEST_BOOL(!b.Contains(wdVec3T(-1, 0, 0)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Box)")
  {
    wdBoundingBoxT b1(wdVec3T(-3), wdVec3T(3));
    wdBoundingBoxT b2(wdVec3T(-1), wdVec3T(1));
    wdBoundingBoxT b3(wdVec3T(-1), wdVec3T(4));

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Array)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    wdVec3T v[4] = {wdVec3T(0), wdVec3T(1), wdVec3T(5), wdVec3T(6)};

    WD_TEST_BOOL(!b.Contains(&v[0], 4, sizeof(wdVec3T)));
    WD_TEST_BOOL(b.Contains(&v[1], 2, sizeof(wdVec3T)));
    WD_TEST_BOOL(b.Contains(&v[2], 1, sizeof(wdVec3T)));

    WD_TEST_BOOL(!b.Contains(&v[1], 2, sizeof(wdVec3T) * 2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (Sphere)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    WD_TEST_BOOL(b.Contains(wdBoundingSphereT(wdVec3T(3), 2)));
    WD_TEST_BOOL(!b.Contains(wdBoundingSphereT(wdVec3T(3), 2.1f)));
    WD_TEST_BOOL(!b.Contains(wdBoundingSphereT(wdVec3T(8), 2)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (box)")
  {
    wdBoundingBoxT b1(wdVec3T(-3), wdVec3T(3));
    wdBoundingBoxT b2(wdVec3T(-1), wdVec3T(1));
    wdBoundingBoxT b3(wdVec3T(1), wdVec3T(4));
    wdBoundingBoxT b4(wdVec3T(-4, 1, 1), wdVec3T(4, 2, 2));

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (Array)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    wdVec3T v[4] = {wdVec3T(0), wdVec3T(1), wdVec3T(5), wdVec3T(6)};

    WD_TEST_BOOL(!b.Overlaps(&v[0], 1, sizeof(wdVec3T)));
    WD_TEST_BOOL(!b.Overlaps(&v[3], 1, sizeof(wdVec3T)));

    WD_TEST_BOOL(b.Overlaps(&v[0], 4, sizeof(wdVec3T)));
    WD_TEST_BOOL(b.Overlaps(&v[1], 2, sizeof(wdVec3T)));
    WD_TEST_BOOL(b.Overlaps(&v[2], 1, sizeof(wdVec3T)));

    WD_TEST_BOOL(b.Overlaps(&v[1], 2, sizeof(wdVec3T) * 2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (Sphere)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    WD_TEST_BOOL(b.Overlaps(wdBoundingSphereT(wdVec3T(3), 2)));
    WD_TEST_BOOL(b.Overlaps(wdBoundingSphereT(wdVec3T(3), 2.1f)));
    WD_TEST_BOOL(!b.Overlaps(wdBoundingSphereT(wdVec3T(8), 2)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical, ==, !=")
  {
    wdBoundingBoxT b1, b2, b3;

    b1.SetElements(wdVec3T(1), wdVec3T(2));
    b2.SetElements(wdVec3T(1), wdVec3T(2));
    b3.SetElements(wdVec3T(1), wdVec3T(2.01f));

    WD_TEST_BOOL(b1.IsIdentical(b1));
    WD_TEST_BOOL(b2.IsIdentical(b2));
    WD_TEST_BOOL(b3.IsIdentical(b3));

    WD_TEST_BOOL(b1 == b1);
    WD_TEST_BOOL(b2 == b2);
    WD_TEST_BOOL(b3 == b3);

    WD_TEST_BOOL(b1.IsIdentical(b2));
    WD_TEST_BOOL(b2.IsIdentical(b1));

    WD_TEST_BOOL(!b1.IsIdentical(b3));
    WD_TEST_BOOL(!b2.IsIdentical(b3));
    WD_TEST_BOOL(!b3.IsIdentical(b1));
    WD_TEST_BOOL(!b3.IsIdentical(b1));

    WD_TEST_BOOL(b1 == b2);
    WD_TEST_BOOL(b2 == b1);

    WD_TEST_BOOL(b1 != b3);
    WD_TEST_BOOL(b2 != b3);
    WD_TEST_BOOL(b3 != b1);
    WD_TEST_BOOL(b3 != b1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdBoundingBoxT b1, b2;
    b1.SetElements(wdVec3T(-1), wdVec3T(1));
    b2.SetElements(wdVec3T(-1), wdVec3T(2));

    WD_TEST_BOOL(!b1.IsEqual(b2));
    WD_TEST_BOOL(!b1.IsEqual(b2, 0.5f));
    WD_TEST_BOOL(b1.IsEqual(b2, 1));
    WD_TEST_BOOL(b1.IsEqual(b2, 2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCenter")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(7));

    WD_TEST_BOOL(b.GetCenter() == wdVec3T(5));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetExtents")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(7));

    WD_TEST_BOOL(b.GetExtents() == wdVec3T(4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetHalfExtents")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(7));

    WD_TEST_BOOL(b.GetHalfExtents() == wdVec3T(2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Translate")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

    b.Translate(wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.m_vMin == wdVec3T(4, 5, 6));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(6, 7, 8));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ScaleFromCenter")
  {
    {
      wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

      b.ScaleFromCenter(wdVec3T(1, 2, 3));

      WD_TEST_BOOL(b.m_vMin == wdVec3T(3, 2, 1));
      WD_TEST_BOOL(b.m_vMax == wdVec3T(5, 6, 7));
    }
    {
      wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

      b.ScaleFromCenter(wdVec3T(-1, -2, -3));

      WD_TEST_BOOL(b.m_vMin == wdVec3T(3, 2, 1));
      WD_TEST_BOOL(b.m_vMax == wdVec3T(5, 6, 7));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ScaleFromOrigin")
  {
    {
      wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

      b.ScaleFromOrigin(wdVec3T(1, 2, 3));

      WD_TEST_BOOL(b.m_vMin == wdVec3T(3, 6, 9));
      WD_TEST_BOOL(b.m_vMax == wdVec3T(5, 10, 15));
    }
    {
      wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

      b.ScaleFromOrigin(wdVec3T(-1, -2, -3));

      WD_TEST_BOOL(b.m_vMin == wdVec3T(-5, -10, -15));
      WD_TEST_BOOL(b.m_vMax == wdVec3T(-3, -6, -9));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformFromOrigin")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

    wdMat4T m;
    m.SetScalingMatrix(wdVec3T(2));

    b.TransformFromOrigin(m);

    WD_TEST_BOOL(b.m_vMin == wdVec3T(6, 6, 6));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(10, 10, 10));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformFromCenter")
  {
    wdBoundingBoxT b(wdVec3T(3), wdVec3T(5));

    wdMat4T m;
    m.SetScalingMatrix(wdVec3T(2));

    b.TransformFromCenter(m);

    WD_TEST_BOOL(b.m_vMin == wdVec3T(2, 2, 2));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(6, 6, 6));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetClampedPoint")
  {
    wdBoundingBoxT b(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(-2, 0, 0)) == wdVec3T(-1, 0, 0));
    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(2, 0, 0)) == wdVec3T(1, 0, 0));

    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(0, -3, 0)) == wdVec3T(0, -2, 0));
    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(0, 3, 0)) == wdVec3T(0, 2, 0));

    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(0, 0, -4)) == wdVec3T(0, 0, -3));
    WD_TEST_BOOL(b.GetClampedPoint(wdVec3T(0, 0, 4)) == wdVec3T(0, 0, 3));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (point)")
  {
    wdBoundingBoxT b(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(-2, 0, 0)) == 1);
    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(2, 0, 0)) == 1);

    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(0, -4, 0)) == 2);
    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(0, 4, 0)) == 2);

    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(0, 0, -6)) == 3);
    WD_TEST_BOOL(b.GetDistanceTo(wdVec3T(0, 0, 6)) == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (Sphere)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    WD_TEST_BOOL(b.GetDistanceTo(wdBoundingSphereT(wdVec3T(3), 2)) < 0);
    WD_TEST_BOOL(b.GetDistanceTo(wdBoundingSphereT(wdVec3T(5), 1)) < 0);
    WD_TEST_FLOAT(b.GetDistanceTo(wdBoundingSphereT(wdVec3T(8, 2, 2), 2)), 1, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (box)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    wdBoundingBoxT b1, b2, b3;
    b1.SetCenterAndHalfExtents(wdVec3T(3), wdVec3T(2));
    b2.SetCenterAndHalfExtents(wdVec3T(5), wdVec3T(1));
    b3.SetCenterAndHalfExtents(wdVec3T(9, 2, 2), wdVec3T(2));

    WD_TEST_BOOL(b.GetDistanceTo(b1) <= 0);
    WD_TEST_BOOL(b.GetDistanceTo(b2) <= 0);
    WD_TEST_FLOAT(b.GetDistanceTo(b3), 2, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceSquaredTo (point)")
  {
    wdBoundingBoxT b(wdVec3T(-1, -2, -3), wdVec3T(1, 2, 3));

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(-2, 0, 0)) == 1);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(2, 0, 0)) == 1);

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(0, -4, 0)) == 4);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(0, 4, 0)) == 4);

    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(0, 0, -6)) == 9);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(wdVec3T(0, 0, 6)) == 9);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceSquaredTo (box)")
  {
    wdBoundingBoxT b(wdVec3T(1), wdVec3T(5));

    wdBoundingBoxT b1, b2, b3;
    b1.SetCenterAndHalfExtents(wdVec3T(3), wdVec3T(2));
    b2.SetCenterAndHalfExtents(wdVec3T(5), wdVec3T(1));
    b3.SetCenterAndHalfExtents(wdVec3T(9, 2, 2), wdVec3T(2));

    WD_TEST_BOOL(b.GetDistanceSquaredTo(b1) <= 0);
    WD_TEST_BOOL(b.GetDistanceSquaredTo(b2) <= 0);
    WD_TEST_FLOAT(b.GetDistanceSquaredTo(b3), 4, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetBoundingSphere")
  {
    wdBoundingBoxT b;
    b.SetCenterAndHalfExtents(wdVec3T(5, 4, 2), wdVec3T(3));

    wdBoundingSphereT s = b.GetBoundingSphere();

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(5, 4, 2));
    WD_TEST_FLOAT(s.m_fRadius, wdVec3T(3).GetLength(), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRayIntersection")
  {
    if (wdMath::SupportsInfinity<wdMathTestType>())
    {
      const wdVec3T c = wdVec3T(10);

      wdBoundingBoxT b;
      b.SetCenterAndHalfExtents(c, wdVec3T(2, 4, 8));

      for (wdMathTestType x = b.m_vMin.x - (wdMathTestType)1; x < b.m_vMax.x + (wdMathTestType)1; x += (wdMathTestType)0.2f)
      {
        for (wdMathTestType y = b.m_vMin.y - (wdMathTestType)1; y < b.m_vMax.y + (wdMathTestType)1; y += (wdMathTestType)0.2f)
        {
          for (wdMathTestType z = b.m_vMin.z - (wdMathTestType)1; z < b.m_vMax.z + (wdMathTestType)1; z += (wdMathTestType)0.2f)
          {
            const wdVec3T v(x, y, z);

            if (b.Contains(v))
              continue;

            const wdVec3T vTarget = b.GetClampedPoint(v);

            const wdVec3T vDir = (vTarget - c).GetNormalized();

            const wdVec3T vSource = vTarget + vDir * (wdMathTestType)3;

            wdMathTestType f;
            wdVec3T vi;
            WD_TEST_BOOL(b.GetRayIntersection(vSource, -vDir, &f, &vi) == true);
            WD_TEST_FLOAT(f, 3, 0.001f);
            WD_TEST_BOOL(vi.IsEqual(vTarget, 0.0001f));

            WD_TEST_BOOL(b.GetRayIntersection(vSource, vDir, &f, &vi) == false);
            WD_TEST_BOOL(b.GetRayIntersection(vTarget, vDir, &f, &vi) == false);
          }
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLineSegmentIntersection")
  {
    if (wdMath::SupportsInfinity<wdMathTestType>())
    {
      const wdVec3T c = wdVec3T(10);

      wdBoundingBoxT b;
      b.SetCenterAndHalfExtents(c, wdVec3T(2, 4, 8));

      for (wdMathTestType x = b.m_vMin.x - (wdMathTestType)1; x < b.m_vMax.x + (wdMathTestType)1; x += (wdMathTestType)0.2f)
      {
        for (wdMathTestType y = b.m_vMin.y - (wdMathTestType)1; y < b.m_vMax.y + (wdMathTestType)1; y += (wdMathTestType)0.2f)
        {
          for (wdMathTestType z = b.m_vMin.z - (wdMathTestType)1; z < b.m_vMax.z + (wdMathTestType)1; z += (wdMathTestType)0.2f)
          {
            const wdVec3T v(x, y, z);

            if (b.Contains(v))
              continue;

            const wdVec3T vTarget0 = b.GetClampedPoint(v);

            const wdVec3T vDir = (vTarget0 - c).GetNormalized();

            const wdVec3T vTarget = vTarget0 - vDir * (wdMathTestType)1;
            const wdVec3T vSource = vTarget0 + vDir * (wdMathTestType)3;

            wdMathTestType f;
            wdVec3T vi;
            WD_TEST_BOOL(b.GetLineSegmentIntersection(vSource, vTarget, &f, &vi) == true);
            WD_TEST_FLOAT(f, 0.75f, 0.001f);
            WD_TEST_BOOL(vi.IsEqual(vTarget0, 0.0001f));
          }
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdBoundingBoxT b;

      b.SetInvalid();
      WD_TEST_BOOL(!b.IsNaN());

      b.SetInvalid();
      b.m_vMin.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vMin.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vMin.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vMax.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vMax.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());

      b.SetInvalid();
      b.m_vMax.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(b.IsNaN());
    }
  }
}
