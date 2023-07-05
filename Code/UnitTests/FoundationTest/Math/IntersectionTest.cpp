#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Intersection.h>
#include <Foundation/Math/Mat4.h>

WD_CREATE_SIMPLE_TEST(Math, Intersection)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "RayPolygonIntersection")
  {
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      wdMat4 m;
      m.SetRotationMatrix(wdVec3(i + 1.0f, i * 3.0f, i * 7.0f).GetNormalized(), wdAngle::Degree((float)i));
      m.SetTranslationVector(wdVec3((float)i, i * 2.0f, i * 3.0f));

      wdVec3 Vertices[8] = {m.TransformPosition(wdVec3(-10, -10, 0)), wdVec3(-10, -10, 0), m.TransformPosition(wdVec3(10, -10, 0)),
        wdVec3(10, -10, 0), m.TransformPosition(wdVec3(10, 10, 0)), wdVec3(10, 10, 0), m.TransformPosition(wdVec3(-10, 10, 0)), wdVec3(-10, 10, 0)};

      for (float y = -14.5; y <= 14.5f; y += 2.0f)
      {
        for (float x = -14.5; x <= 14.5f; x += 2.0f)
        {
          const wdVec3 vRayDir = m.TransformDirection(wdVec3(x, y, -10.0f));
          const wdVec3 vRayStart = m.TransformPosition(wdVec3(x, y, 0.0f)) - vRayDir * 3.0f;

          const bool bIntersects = (x >= -10.0f && x <= 10.0f && y >= -10.0f && y <= 10.0f);

          float fIntersection;
          wdVec3 vIntersection;
          WD_TEST_BOOL(wdIntersectionUtils::RayPolygonIntersection(
                         vRayStart, vRayDir, Vertices, 4, &fIntersection, &vIntersection, sizeof(wdVec3) * 2) == bIntersects);

          if (bIntersects)
          {
            WD_TEST_FLOAT(fIntersection, 3.0f, 0.0001f);
            WD_TEST_VEC3(vIntersection, m.TransformPosition(wdVec3(x, y, 0.0f)), 0.0001f);
          }
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ClosestPoint_PointLineSegment")
  {
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      wdMat4 m;
      m.SetRotationMatrix(wdVec3(i + 1.0f, i * 3.0f, i * 7.0f).GetNormalized(), wdAngle::Degree((float)i));
      m.SetTranslationVector(wdVec3((float)i, i * 2.0f, i * 3.0f));

      wdVec3 vSegment0 = m.TransformPosition(wdVec3(-10, 1, 2));
      wdVec3 vSegment1 = m.TransformPosition(wdVec3(10, 1, 2));

      for (float f = -20; f <= -10; f += 0.5f)
      {
        const wdVec3 vPos = m.TransformPosition(wdVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const wdVec3 vClosest = wdIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        WD_TEST_FLOAT(fFraction, 0.0f, 0.0001f);
        WD_TEST_VEC3(vClosest, vSegment0, 0.0001f);
      }

      for (float f = -10; f <= 10; f += 0.5f)
      {
        const wdVec3 vPos = m.TransformPosition(wdVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const wdVec3 vClosest = wdIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        WD_TEST_FLOAT(fFraction, (f + 10.0f) / 20.0f, 0.0001f);
        WD_TEST_VEC3(vClosest, m.TransformPosition(wdVec3(f, 1, 2)), 0.0001f);
      }

      for (float f = 10; f <= 20; f += 0.5f)
      {
        const wdVec3 vPos = m.TransformPosition(wdVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const wdVec3 vClosest = wdIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        WD_TEST_FLOAT(fFraction, 1.0f, 0.0001f);
        WD_TEST_VEC3(vClosest, vSegment1, 0.0001f);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ray2DLine2D")
  {
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      wdMat4 m;
      m.SetRotationMatrixZ(wdAngle::Degree((float)i));
      m.SetTranslationVector(wdVec3((float)i, i * 2.0f, i * 3.0f));

      const wdVec2 vSegment0 = m.TransformPosition(wdVec3(23, 42, 0)).GetAsVec2();
      const wdVec2 vSegmentDir = m.TransformDirection(wdVec3(13, 15, 0)).GetAsVec2();

      const wdVec2 vSegment1 = vSegment0 + vSegmentDir;

      for (float f = -1.1f; f < 2.0f; f += 0.2f)
      {
        const bool bIntersection = (f >= 0.0f && f <= 1.0f);
        const wdVec2 vSegmentPos = vSegment0 + f * vSegmentDir;

        const wdVec2 vRayDir = wdVec2(2.0f, f);
        const wdVec2 vRayStart = vSegmentPos - vRayDir * 5.0f;

        float fIntersection;
        wdVec2 vIntersection;
        WD_TEST_BOOL(wdIntersectionUtils::Ray2DLine2D(vRayStart, vRayDir, vSegment0, vSegment1, &fIntersection, &vIntersection) == bIntersection);

        if (bIntersection)
        {
          WD_TEST_FLOAT(fIntersection, 5.0f, 0.0001f);
          WD_TEST_VEC2(vIntersection, vSegmentPos, 0.0001f);
        }
      };
    }
  }
}
