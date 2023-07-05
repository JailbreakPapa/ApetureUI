#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Frustum.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/Utilities/GraphicsUtils.h>

WD_CREATE_SIMPLE_TEST(Math, Frustum)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFrustum (planes)")
  {
    wdFrustum f;

    wdPlane p[6];
    p[0].SetFromNormalAndPoint(wdVec3(1, 0, 0), wdVec3(1, 2, 3));
    p[1].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[2].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[3].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[4].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[5].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));

    f.SetFrustum(p);

    WD_TEST_BOOL(f.GetPlane(0) == p[0]);
    WD_TEST_BOOL(f.GetPlane(1) == p[1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TransformFrustum")
  {
    wdFrustum f;

    wdPlane p[6];
    p[0].SetFromNormalAndPoint(wdVec3(1, 0, 0), wdVec3(1, 2, 3));
    p[1].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[2].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[3].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[4].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[5].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));

    f.SetFrustum(p);

    wdMat4 mTransform;
    mTransform.SetRotationMatrixY(wdAngle::Degree(90.0f));
    mTransform.SetTranslationVector(wdVec3(2, 3, 4));

    f.TransformFrustum(mTransform);

    p[0].Transform(mTransform);
    p[1].Transform(mTransform);

    WD_TEST_BOOL(f.GetPlane(0).IsEqual(p[0], 0.001f));
    WD_TEST_BOOL(f.GetPlane(1).IsEqual(p[1], 0.001f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "InvertFrustum")
  {
    wdFrustum f;

    wdPlane p[6];
    p[0].SetFromNormalAndPoint(wdVec3(1, 0, 0), wdVec3(1, 2, 3));
    p[1].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[2].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[3].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[4].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));
    p[5].SetFromNormalAndPoint(wdVec3(0, 1, 0), wdVec3(2, 3, 4));

    f.SetFrustum(p);

    f.InvertFrustum();

    p[0].Flip();
    p[1].Flip();

    WD_TEST_BOOL(f.GetPlane(0) == p[0]);
    WD_TEST_BOOL(f.GetPlane(1) == p[1]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFrustum")
  {
    // check that the extracted frustum planes are always the same, no matter the handedness or depth-range

    // test the different depth ranges
    for (int r = 0; r < 2; ++r)
    {
      const wdClipSpaceDepthRange::Enum range = (r == 0) ? wdClipSpaceDepthRange::MinusOneToOne : wdClipSpaceDepthRange::ZeroToOne;

      // test rotated model-view matrices
      for (int rot = 0; rot < 360; rot += 45)
      {
        wdVec3 vLookDir;
        vLookDir.Set(wdMath::Sin(wdAngle::Degree((float)rot)), 0, -wdMath::Cos(wdAngle::Degree((float)rot)));

        wdVec3 vRightDir;
        vRightDir.Set(wdMath::Sin(wdAngle::Degree(rot + 90.0f)), 0, -wdMath::Cos(wdAngle::Degree(rot + 90.0f)));

        const wdVec3 vCamPos(rot * 1.0f, rot * 0.5f, rot * -0.3f);

        // const wdMat4 mViewLH = wdGraphicsUtils::CreateViewMatrix(vCamPos, vLookDir, -vRightDir, wdVec3(0, 1, 0), wdHandedness::LeftHanded);
        // const wdMat4 mViewRH = wdGraphicsUtils::CreateViewMatrix(vCamPos, vLookDir, vRightDir, wdVec3(0, 1, 0), wdHandedness::RightHanded);
        const wdMat4 mViewLH = wdGraphicsUtils::CreateLookAtViewMatrix(vCamPos, vCamPos + vLookDir, wdVec3(0, 1, 0), wdHandedness::LeftHanded);
        const wdMat4 mViewRH = wdGraphicsUtils::CreateLookAtViewMatrix(vCamPos, vCamPos + vLookDir, wdVec3(0, 1, 0), wdHandedness::RightHanded);

        const wdMat4 mProjLH = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
          wdAngle::Degree(90), 1.0f, 1.0f, 100.0f, range, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);
        const wdMat4 mProjRH = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
          wdAngle::Degree(90), 1.0f, 1.0f, 100.0f, range, wdClipSpaceYMode::Regular, wdHandedness::RightHanded);

        const wdMat4 mViewProjLH = mProjLH * mViewLH;
        const wdMat4 mViewProjRH = mProjRH * mViewRH;

        wdFrustum fLH, fRH, fB;
        fLH.SetFrustum(mViewProjLH, range, wdHandedness::LeftHanded);
        fRH.SetFrustum(mViewProjRH, range, wdHandedness::RightHanded);

        fB.SetFrustum(vCamPos, vLookDir, wdVec3(0, 1, 0), wdAngle::Degree(90), wdAngle::Degree(90), 1.0f, 100.0f);

        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::NearPlane).IsEqual(fB.GetPlane(wdFrustum::NearPlane), 0.1f));
        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::LeftPlane).IsEqual(fB.GetPlane(wdFrustum::LeftPlane), 0.1f));
        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::RightPlane).IsEqual(fB.GetPlane(wdFrustum::RightPlane), 0.1f));
        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::FarPlane).IsEqual(fB.GetPlane(wdFrustum::FarPlane), 0.1f));
        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::BottomPlane).IsEqual(fB.GetPlane(wdFrustum::BottomPlane), 0.1f));
        WD_TEST_BOOL(fRH.GetPlane(wdFrustum::TopPlane).IsEqual(fB.GetPlane(wdFrustum::TopPlane), 0.1f));

        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::NearPlane).IsEqual(fB.GetPlane(wdFrustum::NearPlane), 0.1f));
        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::LeftPlane).IsEqual(fB.GetPlane(wdFrustum::LeftPlane), 0.1f));
        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::RightPlane).IsEqual(fB.GetPlane(wdFrustum::RightPlane), 0.1f));
        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::FarPlane).IsEqual(fB.GetPlane(wdFrustum::FarPlane), 0.1f));
        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::BottomPlane).IsEqual(fB.GetPlane(wdFrustum::BottomPlane), 0.1f));
        WD_TEST_BOOL(fLH.GetPlane(wdFrustum::TopPlane).IsEqual(fB.GetPlane(wdFrustum::TopPlane), 0.1f));
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Culling")
  {
    const wdVec3 offsetPos(23, 17, -9);
    const wdVec3 camDir[6] = {wdVec3(-1, 0, 0), wdVec3(1, 0, 0), wdVec3(0, -1, 0), wdVec3(0, 1, 0), wdVec3(0, 0, -1), wdVec3(0, 0, 1)};
    const wdVec3 objPos[6] = {wdVec3(-9, 0, 0), wdVec3(9, 0, 0), wdVec3(0, -9, 0), wdVec3(0, 9, 0), wdVec3(0, 0, -9), wdVec3(0, 0, 9)};

    for (wdUInt32 dir = 0; dir < 6; ++dir)
    {
      wdFrustum fDir;
      fDir.SetFrustum(
        offsetPos, camDir[dir], camDir[dir].GetOrthogonalVector() /*arbitrary*/, wdAngle::Degree(90), wdAngle::Degree(90), 1.0f, 100.0f);

      for (wdUInt32 obj = 0; obj < 6; ++obj)
      {
        // box
        {
          wdBoundingBox boundingObj;
          boundingObj.SetCenterAndHalfExtents(offsetPos + objPos[obj], wdVec3(1.0f));

          const wdVolumePosition::Enum res = fDir.GetObjectPosition(boundingObj);

          if (obj == dir)
            WD_TEST_BOOL(res == wdVolumePosition::Inside);
          else
            WD_TEST_BOOL(res == wdVolumePosition::Outside);
        }

        // sphere
        {
          wdBoundingSphere boundingObj;
          boundingObj.SetElements(offsetPos + objPos[obj], 0.93f);

          const wdVolumePosition::Enum res = fDir.GetObjectPosition(boundingObj);

          if (obj == dir)
            WD_TEST_BOOL(res == wdVolumePosition::Inside);
          else
            WD_TEST_BOOL(res == wdVolumePosition::Outside);
        }

        // vertices
        {
          wdBoundingBox boundingObj;
          boundingObj.SetCenterAndHalfExtents(offsetPos + objPos[obj], wdVec3(1.0f));

          wdVec3 vertices[8];
          boundingObj.GetCorners(vertices);

          const wdVolumePosition::Enum res = fDir.GetObjectPosition(vertices, 8);

          if (obj == dir)
            WD_TEST_BOOL(res == wdVolumePosition::Inside);
          else
            WD_TEST_BOOL(res == wdVolumePosition::Outside);
        }

        // vertices + transform
        {
          wdBoundingBox boundingObj;
          boundingObj.SetCenterAndHalfExtents(objPos[obj], wdVec3(1.0f));

          wdVec3 vertices[8];
          boundingObj.GetCorners(vertices);

          wdMat4 transform;
          transform.SetTranslationMatrix(offsetPos);

          const wdVolumePosition::Enum res = fDir.GetObjectPosition(vertices, 8, transform);

          if (obj == dir)
            WD_TEST_BOOL(res == wdVolumePosition::Inside);
          else
            WD_TEST_BOOL(res == wdVolumePosition::Outside);
        }

        // SIMD box
        {
          wdBoundingBox boundingObj;
          boundingObj.SetCenterAndHalfExtents(offsetPos + objPos[obj], wdVec3(1.0f));

          const bool res = fDir.Overlaps(wdSimdConversion::ToBBox(boundingObj));

          if (obj == dir)
            WD_TEST_BOOL(res == true);
          else
            WD_TEST_BOOL(res == false);
        }

        // SIMD sphere
        {
          wdBoundingSphere boundingObj;
          boundingObj.SetElements(offsetPos + objPos[obj], 0.93f);

          const bool res = fDir.Overlaps(wdSimdConversion::ToBSphere(boundingObj));

          if (obj == dir)
            WD_TEST_BOOL(res == true);
          else
            WD_TEST_BOOL(res == false);
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeCornerPoints")
  {
    const wdMat4 mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
      wdAngle::Degree(90), 1.0f, 1.0f, 10.0f, wdClipSpaceDepthRange::MinusOneToOne, wdClipSpaceYMode::Regular, wdHandedness::RightHanded);

    wdFrustum frustum[2];
    frustum[0].SetFrustum(mProj, wdClipSpaceDepthRange::MinusOneToOne, wdHandedness::RightHanded);
    frustum[1].SetFrustum(wdVec3::ZeroVector(), wdVec3(0, 0, -1), wdVec3(0, 1, 0), wdAngle::Degree(90), wdAngle::Degree(90), 1.0f, 10.0f);

    for (int f = 0; f < 2; ++f)
    {
      wdVec3 corner[8];
      frustum[f].ComputeCornerPoints(corner);

      wdPositionOnPlane::Enum results[8][6];

      for (int c = 0; c < 8; ++c)
      {
        for (int p = 0; p < 6; ++p)
        {
          results[c][p] = wdPositionOnPlane::Back;
        }
      }

      results[wdFrustum::FrustumCorner::NearTopLeft][wdFrustum::PlaneType::NearPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearTopLeft][wdFrustum::PlaneType::TopPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearTopLeft][wdFrustum::PlaneType::LeftPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::NearTopRight][wdFrustum::PlaneType::NearPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearTopRight][wdFrustum::PlaneType::TopPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearTopRight][wdFrustum::PlaneType::RightPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::NearBottomLeft][wdFrustum::PlaneType::NearPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearBottomLeft][wdFrustum::PlaneType::BottomPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearBottomLeft][wdFrustum::PlaneType::LeftPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::NearBottomRight][wdFrustum::PlaneType::NearPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearBottomRight][wdFrustum::PlaneType::BottomPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::NearBottomRight][wdFrustum::PlaneType::RightPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::FarTopLeft][wdFrustum::PlaneType::FarPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarTopLeft][wdFrustum::PlaneType::TopPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarTopLeft][wdFrustum::PlaneType::LeftPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::FarTopRight][wdFrustum::PlaneType::FarPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarTopRight][wdFrustum::PlaneType::TopPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarTopRight][wdFrustum::PlaneType::RightPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::FarBottomLeft][wdFrustum::PlaneType::FarPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarBottomLeft][wdFrustum::PlaneType::BottomPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarBottomLeft][wdFrustum::PlaneType::LeftPlane] = wdPositionOnPlane::OnPlane;

      results[wdFrustum::FrustumCorner::FarBottomRight][wdFrustum::PlaneType::FarPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarBottomRight][wdFrustum::PlaneType::BottomPlane] = wdPositionOnPlane::OnPlane;
      results[wdFrustum::FrustumCorner::FarBottomRight][wdFrustum::PlaneType::RightPlane] = wdPositionOnPlane::OnPlane;

      for (int c = 0; c < 8; ++c)
      {
        wdFrustum::FrustumCorner cornerName = (wdFrustum::FrustumCorner)c;

        for (int p = 0; p < 6; ++p)
        {
          wdFrustum::PlaneType planeName = (wdFrustum::PlaneType)p;

          wdPlane plane = frustum[f].GetPlane(planeName);
          wdPositionOnPlane::Enum expected = results[cornerName][planeName];
          wdPositionOnPlane::Enum result = plane.GetPointPosition(corner[cornerName], 0.1f);
          // float fDistToPlane = plane.GetDistanceTo(corner[cornerName]);
          WD_TEST_BOOL(result == expected);
        }
      }
    }
  }
}
