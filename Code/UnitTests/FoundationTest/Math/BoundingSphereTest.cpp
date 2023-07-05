#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/BoundingBox.h>
#include <Foundation/Math/BoundingSphere.h>

WD_CREATE_SIMPLE_TEST(Math, BoundingSphere)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(s.m_fRadius == 4.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetInvalid / IsValid")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    WD_TEST_BOOL(s.IsValid());

    s.SetInvalid();

    WD_TEST_BOOL(!s.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetZero / IsZero")
  {
    wdBoundingSphereT s;
    s.SetZero();

    WD_TEST_BOOL(s.IsValid());
    WD_TEST_BOOL(s.m_vCenter.IsZero());
    WD_TEST_BOOL(s.m_fRadius == 0.0f);
    WD_TEST_BOOL(s.IsZero());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetElements")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(s.m_fRadius == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFromPoints")
  {
    wdBoundingSphereT s;

    wdVec3T p[4] = {wdVec3T(2, 6, 0), wdVec3T(4, 2, 0), wdVec3T(2, 0, 0), wdVec3T(0, 4, 0)};

    s.SetFromPoints(p, 4);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(2, 3, 0));
    WD_TEST_BOOL(s.m_fRadius == 3);

    for (int i = 0; i < WD_ARRAY_SIZE(p); ++i)
    {
      WD_TEST_BOOL(s.Contains(p[i]));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude(Point)")
  {
    wdBoundingSphereT s;
    s.SetZero();

    s.ExpandToInclude(wdVec3T(3, 0, 0));

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(0, 0, 0));
    WD_TEST_BOOL(s.m_fRadius == 3);

    s.SetInvalid();

    s.ExpandToInclude(wdVec3T(0.25, 0.0, 0.0));

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(0, 0, 0));
    WD_TEST_BOOL(s.m_fRadius == 0.25);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude(array)")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(2, 2, 0), 0.0f);

    wdVec3T p[4] = {wdVec3T(0, 2, 0), wdVec3T(4, 2, 0), wdVec3T(2, 0, 0), wdVec3T(2, 4, 0)};

    s.ExpandToInclude(p, 4);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(2, 2, 0));
    WD_TEST_BOOL(s.m_fRadius == 2);

    for (int i = 0; i < WD_ARRAY_SIZE(p); ++i)
    {
      WD_TEST_BOOL(s.Contains(p[i]));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (sphere)")
  {
    wdBoundingSphereT s1, s2, s3;
    s1.SetElements(wdVec3T(5, 0, 0), 1);
    s2.SetElements(wdVec3T(6, 0, 0), 1);
    s3.SetElements(wdVec3T(5, 0, 0), 2);

    s1.ExpandToInclude(s2);
    WD_TEST_BOOL(s1.m_vCenter == wdVec3T(5, 0, 0));
    WD_TEST_BOOL(s1.m_fRadius == 2);

    s1.ExpandToInclude(s3);
    WD_TEST_BOOL(s1.m_vCenter == wdVec3T(5, 0, 0));
    WD_TEST_BOOL(s1.m_fRadius == 2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExpandToInclude (box)")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 1);

    wdBoundingBoxT b;
    b.SetCenterAndHalfExtents(wdVec3T(1, 2, 3), wdVec3T(2.0f));

    s.ExpandToInclude(b);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(1, 2, 3));
    WD_TEST_FLOAT(s.m_fRadius, wdMath::Sqrt((wdMathTestType)12), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Grow")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    s.Grow(5);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(s.m_fRadius == 9);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical, ==, !=")
  {
    wdBoundingSphereT s1, s2, s3;

    s1.SetElements(wdVec3T(1, 2, 3), 4);
    s2.SetElements(wdVec3T(1, 2, 3), 4);
    s3.SetElements(wdVec3T(1.001f, 2.001f, 3.001f), 4.001f);

    WD_TEST_BOOL(s1 == s1);
    WD_TEST_BOOL(s2 == s2);
    WD_TEST_BOOL(s3 == s3);

    WD_TEST_BOOL(s1 == s2);
    WD_TEST_BOOL(s2 == s1);

    WD_TEST_BOOL(s1 != s3);
    WD_TEST_BOOL(s2 != s3);
    WD_TEST_BOOL(s3 != s1);
    WD_TEST_BOOL(s3 != s2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdBoundingSphereT s1, s2, s3;

    s1.SetElements(wdVec3T(1, 2, 3), 4);
    s2.SetElements(wdVec3T(1, 2, 3), 4);
    s3.SetElements(wdVec3T(1.001f, 2.001f, 3.001f), 4.001f);

    WD_TEST_BOOL(s1.IsEqual(s1));
    WD_TEST_BOOL(s2.IsEqual(s2));
    WD_TEST_BOOL(s3.IsEqual(s3));

    WD_TEST_BOOL(s1.IsEqual(s2));
    WD_TEST_BOOL(s2.IsEqual(s1));

    WD_TEST_BOOL(!s1.IsEqual(s3, 0.0001f));
    WD_TEST_BOOL(!s2.IsEqual(s3, 0.0001f));
    WD_TEST_BOOL(!s3.IsEqual(s1, 0.0001f));
    WD_TEST_BOOL(!s3.IsEqual(s2, 0.0001f));

    WD_TEST_BOOL(s1.IsEqual(s3, 0.002f));
    WD_TEST_BOOL(s2.IsEqual(s3, 0.002f));
    WD_TEST_BOOL(s3.IsEqual(s1, 0.002f));
    WD_TEST_BOOL(s3.IsEqual(s2, 0.002f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Translate")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    s.Translate(wdVec3T(4, 5, 6));

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(5, 7, 9));
    WD_TEST_BOOL(s.m_fRadius == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ScaleFromCenter")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    s.ScaleFromCenter(5.0f);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(1, 2, 3));
    WD_TEST_BOOL(s.m_fRadius == 20);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ScaleFromOrigin")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 4);

    s.ScaleFromOrigin(wdVec3T(2, 3, 4));

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(2, 6, 12));
    WD_TEST_BOOL(s.m_fRadius == 16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (point)")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(5, 0, 0), 2);

    WD_TEST_BOOL(s.GetDistanceTo(wdVec3T(5, 0, 0)) == -2.0f);
    WD_TEST_BOOL(s.GetDistanceTo(wdVec3T(7, 0, 0)) == 0.0f);
    WD_TEST_BOOL(s.GetDistanceTo(wdVec3T(9, 0, 0)) == 2.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (sphere)")
  {
    wdBoundingSphereT s1, s2, s3;
    s1.SetElements(wdVec3T(5, 0, 0), 2);
    s2.SetElements(wdVec3T(10, 0, 0), 3);
    s3.SetElements(wdVec3T(10, 0, 0), 1);

    WD_TEST_BOOL(s1.GetDistanceTo(s2) == 0.0f);
    WD_TEST_BOOL(s1.GetDistanceTo(s3) == 2.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetDistanceTo (array)")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(0.0f), 0.0f);

    wdVec3T p[4] = {
      wdVec3T(5),
      wdVec3T(10),
      wdVec3T(15),
      wdVec3T(7),
    };

    WD_TEST_FLOAT(s.GetDistanceTo(p, 4), wdVec3T(5).GetLength(), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (point)")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(5, 0, 0), 2.0f);

    WD_TEST_BOOL(s.Contains(wdVec3T(3, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdVec3T(5, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdVec3T(6, 0, 0)));
    WD_TEST_BOOL(s.Contains(wdVec3T(7, 0, 0)));

    WD_TEST_BOOL(!s.Contains(wdVec3T(2, 0, 0)));
    WD_TEST_BOOL(!s.Contains(wdVec3T(8, 0, 0)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (array)")
  {
    wdBoundingSphereT s(wdVec3T(0.0f), 6.0f);

    wdVec3T p[4] = {
      wdVec3T(3),
      wdVec3T(10),
      wdVec3T(2),
      wdVec3T(7),
    };

    WD_TEST_BOOL(s.Contains(p, 2, sizeof(wdVec3T) * 2));
    WD_TEST_BOOL(!s.Contains(p + 1, 2, sizeof(wdVec3T) * 2));
    WD_TEST_BOOL(!s.Contains(p, 4, sizeof(wdVec3T)));
  }

  // Disabled because MSVC 2017 has code generation issues in Release builds
  WD_TEST_BLOCK(wdTestBlock::Disabled, "Contains (sphere)")
  {
    wdBoundingSphereT s1, s2, s3;
    s1.SetElements(wdVec3T(5, 0, 0), 2);
    s2.SetElements(wdVec3T(6, 0, 0), 1);
    s3.SetElements(wdVec3T(6, 0, 0), 2);

    WD_TEST_BOOL(s1.Contains(s1));
    WD_TEST_BOOL(s2.Contains(s2));
    WD_TEST_BOOL(s3.Contains(s3));

    WD_TEST_BOOL(s1.Contains(s2));
    WD_TEST_BOOL(!s1.Contains(s3));

    WD_TEST_BOOL(!s2.Contains(s3));
    WD_TEST_BOOL(s3.Contains(s2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Contains (box)")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);
    wdBoundingBoxT b1(wdVec3T(1, 2, 3) - wdVec3T(1), wdVec3T(1, 2, 3) + wdVec3T(1));
    wdBoundingBoxT b2(wdVec3T(1, 2, 3) - wdVec3T(1), wdVec3T(1, 2, 3) + wdVec3T(3));

    WD_TEST_BOOL(s.Contains(b1));
    WD_TEST_BOOL(!s.Contains(b2));

    wdVec3T vDir(1, 1, 1);
    vDir.SetLength(3.99f).IgnoreResult();
    wdBoundingBoxT b3(wdVec3T(1, 2, 3) - wdVec3T(1), wdVec3T(1, 2, 3) + vDir);

    WD_TEST_BOOL(s.Contains(b3));

    vDir.SetLength(4.01f).IgnoreResult();
    wdBoundingBoxT b4(wdVec3T(1, 2, 3) - wdVec3T(1), wdVec3T(1, 2, 3) + vDir);

    WD_TEST_BOOL(!s.Contains(b4));
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (array)")
  {
    wdBoundingSphereT s(wdVec3T(0.0f), 6.0f);

    wdVec3T p[4] = {
      wdVec3T(3),
      wdVec3T(10),
      wdVec3T(2),
      wdVec3T(7),
    };

    WD_TEST_BOOL(s.Overlaps(p, 2, sizeof(wdVec3T) * 2));
    WD_TEST_BOOL(!s.Overlaps(p + 1, 2, sizeof(wdVec3T) * 2));
    WD_TEST_BOOL(s.Overlaps(p, 4, sizeof(wdVec3T)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (sphere)")
  {
    wdBoundingSphereT s1, s2, s3;
    s1.SetElements(wdVec3T(5, 0, 0), 2);
    s2.SetElements(wdVec3T(6, 0, 0), 2);
    s3.SetElements(wdVec3T(8, 0, 0), 1);

    WD_TEST_BOOL(s1.Overlaps(s1));
    WD_TEST_BOOL(s2.Overlaps(s2));
    WD_TEST_BOOL(s3.Overlaps(s3));

    WD_TEST_BOOL(s1.Overlaps(s2));
    WD_TEST_BOOL(!s1.Overlaps(s3));

    WD_TEST_BOOL(s2.Overlaps(s3));
    WD_TEST_BOOL(s3.Overlaps(s2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Overlaps (box)")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 2);
    wdBoundingBoxT b1(wdVec3T(1, 2, 3), wdVec3T(1, 2, 3) + wdVec3T(2));
    wdBoundingBoxT b2(wdVec3T(1, 2, 3) + wdVec3T(2), wdVec3T(1, 2, 3) + wdVec3T(3));

    WD_TEST_BOOL(s.Overlaps(b1));
    WD_TEST_BOOL(!s.Overlaps(b2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetBoundingBox")
  {
    wdBoundingSphereT s;
    s.SetElements(wdVec3T(1, 2, 3), 2.0f);

    wdBoundingBoxT b = s.GetBoundingBox();

    WD_TEST_BOOL(b.m_vMin == wdVec3T(-1, 0, 1));
    WD_TEST_BOOL(b.m_vMax == wdVec3T(3, 4, 5));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetClampedPoint")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 2.0f);

    WD_TEST_VEC3(s.GetClampedPoint(wdVec3T(2, 2, 3)), wdVec3T(2, 2, 3), 0.001);
    WD_TEST_VEC3(s.GetClampedPoint(wdVec3T(5, 2, 3)), wdVec3T(3, 2, 3), 0.001);
    WD_TEST_VEC3(s.GetClampedPoint(wdVec3T(1, 7, 3)), wdVec3T(1, 4, 3), 0.001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRayIntersection")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);

    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      const wdVec3T vDir =
        wdVec3T(wdMath::Sin(wdAngle::Degree(i * 1.0f)), wdMath::Cos(wdAngle::Degree(i * 3.0f)), wdMath::Cos(wdAngle::Degree(i * 1.0f)))
          .GetNormalized();
      const wdVec3T vTarget = vDir * s.m_fRadius + s.m_vCenter;
      const wdVec3T vSource = vTarget + vDir * (wdMathTestType)5;

      WD_TEST_FLOAT((vSource - vTarget).GetLength(), 5.0f, 0.001f);

      wdMathTestType fIntersection;
      wdVec3T vIntersection;
      WD_TEST_BOOL(s.GetRayIntersection(vSource, -vDir, &fIntersection, &vIntersection) == true);
      WD_TEST_FLOAT(fIntersection, (vSource - vTarget).GetLength(), 0.0001f);
      WD_TEST_BOOL(vIntersection.IsEqual(vTarget, 0.0001f));

      WD_TEST_BOOL(s.GetRayIntersection(vSource, vDir, &fIntersection, &vIntersection) == false);

      WD_TEST_BOOL(s.GetRayIntersection(vTarget - vDir, vDir, &fIntersection, &vIntersection) == true);
      WD_TEST_FLOAT(fIntersection, 1, 0.0001f);
      WD_TEST_BOOL(vIntersection.IsEqual(vTarget, 0.0001f));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLineSegmentIntersection")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);

    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      const wdVec3T vDir = wdVec3T(wdMath::Sin(wdAngle::Degree(i * (wdMathTestType)1)), wdMath::Cos(wdAngle::Degree(i * (wdMathTestType)3)),
        wdMath::Cos(wdAngle::Degree(i * (wdMathTestType)1)))
                             .GetNormalized();
      const wdVec3T vTarget = vDir * s.m_fRadius + s.m_vCenter - vDir;
      const wdVec3T vSource = vTarget + vDir * (wdMathTestType)5;

      wdMathTestType fIntersection;
      wdVec3T vIntersection;
      WD_TEST_BOOL(s.GetLineSegmentIntersection(vSource, vTarget, &fIntersection, &vIntersection) == true);
      WD_TEST_FLOAT(fIntersection, 4.0f / 5.0f, 0.0001f);
      WD_TEST_BOOL(vIntersection.IsEqual(vTarget + vDir, 0.0001f));

      WD_TEST_BOOL(s.GetLineSegmentIntersection(vTarget, vSource, &fIntersection, &vIntersection) == true);
      WD_TEST_FLOAT(fIntersection, 1.0f / 5.0f, 0.0001f);
      WD_TEST_BOOL(vIntersection.IsEqual(vTarget + vDir, 0.0001f));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformFromOrigin")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);
    wdMat4T mTransform;

    mTransform.SetTranslationMatrix(wdVec3T(5, 6, 7));
    mTransform.SetScalingFactors(wdVec3T(4, 3, 2)).IgnoreResult();

    s.TransformFromOrigin(mTransform);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(9, 12, 13));
    WD_TEST_BOOL(s.m_fRadius == 16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformFromCenter")
  {
    wdBoundingSphereT s(wdVec3T(1, 2, 3), 4);
    wdMat4T mTransform;

    mTransform.SetTranslationMatrix(wdVec3T(5, 6, 7));
    mTransform.SetScalingFactors(wdVec3T(4, 3, 2)).IgnoreResult();

    s.TransformFromCenter(mTransform);

    WD_TEST_BOOL(s.m_vCenter == wdVec3T(6, 8, 10));
    WD_TEST_BOOL(s.m_fRadius == 16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdBoundingSphereT s;

      s.SetInvalid();
      WD_TEST_BOOL(!s.IsNaN());

      s.SetInvalid();
      s.m_fRadius = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(s.IsNaN());

      s.SetInvalid();
      s.m_vCenter.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(s.IsNaN());

      s.SetInvalid();
      s.m_vCenter.y = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(s.IsNaN());

      s.SetInvalid();
      s.m_vCenter.z = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(s.IsNaN());
    }
  }
}
