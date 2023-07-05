#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Utilities/GraphicsUtils.h>

WD_CREATE_SIMPLE_TEST(Utility, GraphicsUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Perspective (-1/1): ConvertWorldPosToScreenPos / ConvertScreenPosToWorldPos")
  {
    wdMat4 mProj, mProjInv;

    mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(
      wdAngle::Degree(85.0f), 2.0f, 1.0f, 1000.0f, wdClipSpaceDepthRange::MinusOneToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);
    mProjInv = mProj.GetInverse();

    for (wdUInt32 y = 0; y < 25; ++y)
    {
      for (wdUInt32 x = 0; x < 50; ++x)
      {
        wdVec3 vPoint, vDir;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertScreenPosToWorldPos(
          mProjInv, 0, 0, 50, 25, wdVec3((float)x, (float)y, 0.5f), vPoint, &vDir, wdClipSpaceDepthRange::MinusOneToOne)
                       .Succeeded());

        WD_TEST_VEC3(vDir, vPoint.GetNormalized(), 0.01f);

        wdVec3 vScreen;
        WD_TEST_BOOL(
          wdGraphicsUtils::ConvertWorldPosToScreenPos(mProj, 0, 0, 50, 25, vPoint, vScreen, wdClipSpaceDepthRange::MinusOneToOne).Succeeded());

        WD_TEST_VEC3(vScreen, wdVec3((float)x, (float)y, 0.5f), 0.01f);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Perspective (0/1): ConvertWorldPosToScreenPos / ConvertScreenPosToWorldPos")
  {
    wdMat4 mProj, mProjInv;
    mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(
      wdAngle::Degree(85.0f), 2.0f, 1.0f, 1000.0f, wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);
    mProjInv = mProj.GetInverse();

    for (wdUInt32 y = 0; y < 25; ++y)
    {
      for (wdUInt32 x = 0; x < 50; ++x)
      {
        wdVec3 vPoint, vDir;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertScreenPosToWorldPos(
          mProjInv, 0, 0, 50, 25, wdVec3((float)x, (float)y, 0.5f), vPoint, &vDir, wdClipSpaceDepthRange::ZeroToOne)
                       .Succeeded());

        WD_TEST_VEC3(vDir, vPoint.GetNormalized(), 0.01f);

        wdVec3 vScreen;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertWorldPosToScreenPos(mProj, 0, 0, 50, 25, vPoint, vScreen, wdClipSpaceDepthRange::ZeroToOne).Succeeded());

        WD_TEST_VEC3(vScreen, wdVec3((float)x, (float)y, 0.5f), 0.01f);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ortho (-1/1): ConvertWorldPosToScreenPos / ConvertScreenPosToWorldPos")
  {
    wdMat4 mProj, mProjInv;
    mProj = wdGraphicsUtils::CreateOrthographicProjectionMatrix(
      50, 25, 1.0f, 1000.0f, wdClipSpaceDepthRange::MinusOneToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);

    mProjInv = mProj.GetInverse();

    for (wdUInt32 y = 0; y < 25; ++y)
    {
      for (wdUInt32 x = 0; x < 50; ++x)
      {
        wdVec3 vPoint, vDir;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertScreenPosToWorldPos(
          mProjInv, 0, 0, 50, 25, wdVec3((float)x, (float)y, 0.5f), vPoint, &vDir, wdClipSpaceDepthRange::MinusOneToOne)
                       .Succeeded());

        WD_TEST_VEC3(vDir, wdVec3(0, 0, 1.0f), 0.01f);

        wdVec3 vScreen;
        WD_TEST_BOOL(
          wdGraphicsUtils::ConvertWorldPosToScreenPos(mProj, 0, 0, 50, 25, vPoint, vScreen, wdClipSpaceDepthRange::MinusOneToOne).Succeeded());

        WD_TEST_VEC3(vScreen, wdVec3((float)x, (float)y, 0.5f), 0.01f);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ortho (0/1): ConvertWorldPosToScreenPos / ConvertScreenPosToWorldPos")
  {
    wdMat4 mProj, mProjInv;
    mProj = wdGraphicsUtils::CreateOrthographicProjectionMatrix(
      50, 25, 1.0f, 1000.0f, wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);
    mProjInv = mProj.GetInverse();

    for (wdUInt32 y = 0; y < 25; ++y)
    {
      for (wdUInt32 x = 0; x < 50; ++x)
      {
        wdVec3 vPoint, vDir;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertScreenPosToWorldPos(
          mProjInv, 0, 0, 50, 25, wdVec3((float)x, (float)y, 0.5f), vPoint, &vDir, wdClipSpaceDepthRange::ZeroToOne)
                       .Succeeded());

        WD_TEST_VEC3(vDir, wdVec3(0, 0, 1.0f), 0.01f);

        wdVec3 vScreen;
        WD_TEST_BOOL(wdGraphicsUtils::ConvertWorldPosToScreenPos(mProj, 0, 0, 50, 25, vPoint, vScreen, wdClipSpaceDepthRange::ZeroToOne).Succeeded());

        WD_TEST_VEC3(vScreen, wdVec3((float)x, (float)y, 0.5f), 0.01f);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertProjectionMatrixDepthRange")
  {
    wdMat4 mProj1, mProj2;
    mProj1 = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(
      wdAngle::Degree(85.0f), 2.0f, 1.0f, 1000.0f, wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);
    mProj2 = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(
      wdAngle::Degree(85.0f), 2.0f, 1.0f, 1000.0f, wdClipSpaceDepthRange::MinusOneToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);

    wdMat4 mProj1b = mProj1;
    wdMat4 mProj2b = mProj2;
    wdGraphicsUtils::ConvertProjectionMatrixDepthRange(mProj1b, wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceDepthRange::MinusOneToOne);
    wdGraphicsUtils::ConvertProjectionMatrixDepthRange(mProj2b, wdClipSpaceDepthRange::MinusOneToOne, wdClipSpaceDepthRange::ZeroToOne);

    WD_TEST_BOOL(mProj1.IsEqual(mProj2b, 0.001f));
    WD_TEST_BOOL(mProj2.IsEqual(mProj1b, 0.001f));
  }

  struct DepthRange
  {
    float fNear = 0.0f;
    float fFar = 0.0f;
  };

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExtractPerspectiveMatrixFieldOfView")
  {
    DepthRange depthRanges[] = {{1.0f, 1000.0f}, {1000.0f, 1.0f}, {0.5f, 20.0f}, {20.0f, 0.5f}};
    wdClipSpaceDepthRange::Enum clipRanges[] = {wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceDepthRange::MinusOneToOne};
    wdHandedness::Enum handednesses[] = {wdHandedness::LeftHanded, wdHandedness::RightHanded};
    wdClipSpaceYMode::Enum clipSpaceYModes[] = {wdClipSpaceYMode::Regular, wdClipSpaceYMode::Flipped};

    for (auto clipSpaceYMode : clipSpaceYModes)
    {
      for (auto handedness : handednesses)
      {
        for (auto depthRange : depthRanges)
        {
          for (auto clipRange : clipRanges)
          {
            for (wdUInt32 angle = 10; angle < 180; angle += 10)
            {
              {
                wdMat4 mProj;
                mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(
                  wdAngle::Degree((float)angle), 2.0f, depthRange.fNear, depthRange.fFar, clipRange, clipSpaceYMode, handedness);

                wdAngle fovx, fovy;
                wdGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(mProj, fovx, fovy);

                WD_TEST_FLOAT(fovx.GetDegree(), (float)angle, 0.5f);
              }

              {
                wdMat4 mProj;
                mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
                  wdAngle::Degree((float)angle), 1.0f / 3.0f, depthRange.fNear, depthRange.fFar, clipRange, clipSpaceYMode, handedness);

                wdAngle fovx, fovy;
                wdGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(mProj, fovx, fovy);

                WD_TEST_FLOAT(fovy.GetDegree(), (float)angle, 0.5f);
              }

              {
                const float fMinDepth = wdMath::Min(depthRange.fNear, depthRange.fFar);
                const wdAngle right = wdAngle::Degree((float)angle) / 2;
                const wdAngle top = wdAngle::Degree((float)angle) / 2;
                const float fLeft = wdMath::Tan(-right) * fMinDepth;
                const float fRight = wdMath::Tan(right) * fMinDepth * 0.8f;
                const float fBottom = wdMath::Tan(-top) * fMinDepth;
                const float fTop = wdMath::Tan(top) * fMinDepth * 0.7f;

                wdMat4 mProj;
                mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrix(fLeft, fRight, fBottom, fTop, depthRange.fNear, depthRange.fFar, clipRange, clipSpaceYMode, handedness);

                float fNearOut, fFarOut;
                WD_TEST_BOOL(wdGraphicsUtils::ExtractNearAndFarClipPlaneDistances(fNearOut, fFarOut, mProj, clipRange).Succeeded());
                WD_TEST_FLOAT(depthRange.fNear, fNearOut, 0.1f);
                WD_TEST_FLOAT(depthRange.fFar, fFarOut, 0.1f);

                float fLeftOut, fRightOut, fBottomOut, fTopOut;
                WD_TEST_BOOL(wdGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(mProj, fLeftOut, fRightOut, fBottomOut, fTopOut, clipRange, clipSpaceYMode).Succeeded());
                WD_TEST_FLOAT(fLeft, fLeftOut, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fRight, fRightOut, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fBottom, fBottomOut, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fTop, fTopOut, wdMath::LargeEpsilon<float>());

                wdAngle fFovLeft;
                wdAngle fFovRight;
                wdAngle fFovBottom;
                wdAngle fFovTop;
                wdGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(mProj, fFovLeft, fFovRight, fFovBottom, fFovTop, clipSpaceYMode);

                WD_TEST_FLOAT(fLeft, wdMath::Tan(fFovLeft) * fMinDepth, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fRight, wdMath::Tan(fFovRight) * fMinDepth, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fBottom, wdMath::Tan(fFovBottom) * fMinDepth, wdMath::LargeEpsilon<float>());
                WD_TEST_FLOAT(fTop, wdMath::Tan(fFovTop) * fMinDepth, wdMath::LargeEpsilon<float>());
              }
            }
          }
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ExtractNearAndFarClipPlaneDistances")
  {
    DepthRange depthRanges[] = {{0.001f, 100.0f}, {0.01f, 10.0f}, {10.0f, 0.01f}, {1.01f, 110.0f}, {110.0f, 1.01f}};
    wdClipSpaceDepthRange::Enum clipRanges[] = {wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceDepthRange::MinusOneToOne};
    wdHandedness::Enum handednesses[] = {wdHandedness::LeftHanded, wdHandedness::RightHanded};
    wdClipSpaceYMode::Enum clipSpaceYModes[] = {wdClipSpaceYMode::Regular, wdClipSpaceYMode::Flipped};
    wdAngle fovs[] = {wdAngle::Degree(10.0f), wdAngle::Degree(70.0f)};

    for (auto clipSpaceYMode : clipSpaceYModes)
    {
      for (auto handedness : handednesses)
      {
        for (auto depthRange : depthRanges)
        {
          for (auto clipRange : clipRanges)
          {
            for (auto fov : fovs)
            {
              wdMat4 mProj;
              mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
                fov, 0.7f, depthRange.fNear, depthRange.fFar, clipRange, clipSpaceYMode, handedness);

              float fNearOut, fFarOut;
              WD_TEST_BOOL(wdGraphicsUtils::ExtractNearAndFarClipPlaneDistances(fNearOut, fFarOut, mProj, clipRange).Succeeded());

              WD_TEST_FLOAT(depthRange.fNear, fNearOut, 0.1f);
              WD_TEST_FLOAT(depthRange.fFar, fFarOut, 0.2f);
            }
          }
        }
      }
    }

    { // Test failure on broken projection matrix
      // This matrix has a 0 in the w-component of the third column (invalid perspective divide)
      float vals[] = {0.770734549f, 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f, 1.73205078f, 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f, -1.00000000f, 0.00000000f, 0.000000000, 0.000000000f, -0.100000001f, 0.000000000f};
      wdMat4 mProj;
      memcpy(mProj.m_fElementsCM, vals, 16 * sizeof(float));
      float fNearOut = 0.f, fFarOut = 0.f;
      WD_TEST_BOOL(wdGraphicsUtils::ExtractNearAndFarClipPlaneDistances(fNearOut, fFarOut, mProj, wdClipSpaceDepthRange::MinusOneToOne).Failed());
      WD_TEST_BOOL(fNearOut == 0.0f);
      WD_TEST_BOOL(fFarOut == 0.0f);
    }

    { // Test failure on broken projection matrix
      // This matrix has a 0 in the z-component of the fourth column (one or both projection planes are zero)
      float vals[] = {0.770734549f, 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f, 1.73205078f, 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f, -1.00000000f, -1.00000000f, 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f};
      wdMat4 mProj;
      memcpy(mProj.m_fElementsCM, vals, 16 * sizeof(float));
      float fNearOut = 0.f, fFarOut = 0.f;
      WD_TEST_BOOL(wdGraphicsUtils::ExtractNearAndFarClipPlaneDistances(fNearOut, fFarOut, mProj, wdClipSpaceDepthRange::MinusOneToOne).Failed());
      WD_TEST_BOOL(fNearOut == 0.0f);
      WD_TEST_BOOL(fFarOut == 0.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeInterpolatedFrustumPlane")
  {
    for (wdUInt32 i = 0; i <= 10; ++i)
    {
      float nearPlane = 1.0f;
      float farPlane = 1000.0f;

      wdMat4 mProj;
      mProj = wdGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(
        wdAngle::Degree(90.0f), 1.0f, nearPlane, farPlane, wdClipSpaceDepthRange::ZeroToOne, wdClipSpaceYMode::Regular, wdHandedness::LeftHanded);

      const wdPlane horz = wdGraphicsUtils::ComputeInterpolatedFrustumPlane(
        wdGraphicsUtils::FrustumPlaneInterpolation::LeftToRight, i * 0.1f, mProj, wdClipSpaceDepthRange::ZeroToOne);
      const wdPlane vert = wdGraphicsUtils::ComputeInterpolatedFrustumPlane(
        wdGraphicsUtils::FrustumPlaneInterpolation::BottomToTop, i * 0.1f, mProj, wdClipSpaceDepthRange::ZeroToOne);
      const wdPlane forw = wdGraphicsUtils::ComputeInterpolatedFrustumPlane(
        wdGraphicsUtils::FrustumPlaneInterpolation::NearToFar, i * 0.1f, mProj, wdClipSpaceDepthRange::ZeroToOne);

      // Generate clip space point at intersection of the 3 planes and project to worldspace
      wdVec4 clipSpacePoint = wdVec4(0.1f * i * 2 - 1, 0.1f * i * 2 - 1, 0.1f * i, 1);

      wdVec4 worldSpacePoint = mProj.GetInverse() * clipSpacePoint;
      worldSpacePoint /= worldSpacePoint.w;

      WD_TEST_FLOAT(horz.GetDistanceTo(wdVec3::ZeroVector()), 0.0f, 0.01f);
      WD_TEST_FLOAT(vert.GetDistanceTo(wdVec3::ZeroVector()), 0.0f, 0.01f);

      if (i == 0)
      {
        WD_TEST_FLOAT(forw.GetDistanceTo(wdVec3::ZeroVector()), -nearPlane, 0.01f);
      }
      else if (i == 10)
      {
        WD_TEST_FLOAT(forw.GetDistanceTo(wdVec3::ZeroVector()), -farPlane, 0.01f);
      }

      WD_TEST_FLOAT(horz.GetDistanceTo(worldSpacePoint.GetAsVec3()), 0.0f, 0.02f);
      WD_TEST_FLOAT(vert.GetDistanceTo(worldSpacePoint.GetAsVec3()), 0.0f, 0.02f);
      WD_TEST_FLOAT(forw.GetDistanceTo(worldSpacePoint.GetAsVec3()), 0.0f, 0.02f);

      // this isn't interpolated linearly across the angle (rotated), so the epsilon has to be very large (just an approx test)
      WD_TEST_FLOAT(horz.m_vNormal.GetAngleBetween(wdVec3(1, 0, 0)).GetDegree(), wdMath::Abs(-45.0f + 90.0f * i * 0.1f), 4.0f);
      WD_TEST_FLOAT(vert.m_vNormal.GetAngleBetween(wdVec3(0, 1, 0)).GetDegree(), wdMath::Abs(-45.0f + 90.0f * i * 0.1f), 4.0f);
      WD_TEST_VEC3(forw.m_vNormal, wdVec3(0, 0, 1), 0.01f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateLookAtViewMatrix / CreateInverseLookAtViewMatrix")
  {
    for (int h = 0; h < 2; ++h)
    {
      const wdHandedness::Enum handedness = (h == 0) ? wdHandedness::LeftHanded : wdHandedness::RightHanded;

      {
        wdMat3 mLook3 = wdGraphicsUtils::CreateLookAtViewMatrix(wdVec3(1, 0, 0), wdVec3(0, 0, 1), handedness);
        wdMat3 mLookInv3 = wdGraphicsUtils::CreateInverseLookAtViewMatrix(wdVec3(1, 0, 0), wdVec3(0, 0, 1), handedness);

        WD_TEST_BOOL((mLook3 * mLookInv3).IsIdentity(0.01f));

        wdMat4 mLook4 = wdGraphicsUtils::CreateLookAtViewMatrix(wdVec3(0), wdVec3(1, 0, 0), wdVec3(0, 0, 1), handedness);
        wdMat4 mLookInv4 = wdGraphicsUtils::CreateInverseLookAtViewMatrix(wdVec3(0), wdVec3(1, 0, 0), wdVec3(0, 0, 1), handedness);

        WD_TEST_BOOL((mLook4 * mLookInv4).IsIdentity(0.01f));

        WD_TEST_BOOL(mLook3.IsEqual(mLook4.GetRotationalPart(), 0.01f));
        WD_TEST_BOOL(mLookInv3.IsEqual(mLookInv4.GetRotationalPart(), 0.01f));
      }

      {
        wdMat4 mLook4 = wdGraphicsUtils::CreateLookAtViewMatrix(wdVec3(1, 2, 0), wdVec3(4, 5, 0), wdVec3(0, 0, 1), handedness);
        wdMat4 mLookInv4 = wdGraphicsUtils::CreateInverseLookAtViewMatrix(wdVec3(1, 2, 0), wdVec3(4, 5, 0), wdVec3(0, 0, 1), handedness);

        WD_TEST_BOOL((mLook4 * mLookInv4).IsIdentity(0.01f));
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateViewMatrix / DecomposeViewMatrix / CreateInverseViewMatrix")
  {
    for (int h = 0; h < 2; ++h)
    {
      const wdHandedness::Enum handedness = (h == 0) ? wdHandedness::LeftHanded : wdHandedness::RightHanded;

      const wdVec3 vEye(0);
      const wdVec3 vTarget(0, 0, 1);
      const wdVec3 vUp0(0, 1, 0);
      const wdVec3 vFwd = (vTarget - vEye).GetNormalized();
      wdVec3 vRight = vUp0.CrossRH(vFwd).GetNormalized();
      const wdVec3 vUp = vFwd.CrossRH(vRight).GetNormalized();

      if (handedness == wdHandedness::RightHanded)
        vRight = -vRight;

      const wdMat4 mLookAt = wdGraphicsUtils::CreateLookAtViewMatrix(vEye, vTarget, vUp0, handedness);

      wdVec3 decFwd, decRight, decUp, decPos;
      wdGraphicsUtils::DecomposeViewMatrix(decPos, decFwd, decRight, decUp, mLookAt, handedness);

      WD_TEST_VEC3(decPos, vEye, 0.01f);
      WD_TEST_VEC3(decFwd, vFwd, 0.01f);
      WD_TEST_VEC3(decUp, vUp, 0.01f);
      WD_TEST_VEC3(decRight, vRight, 0.01f);

      const wdMat4 mView = wdGraphicsUtils::CreateViewMatrix(decPos, decFwd, decRight, decUp, handedness);
      const wdMat4 mViewInv = wdGraphicsUtils::CreateInverseViewMatrix(decPos, decFwd, decRight, decUp, handedness);

      WD_TEST_BOOL(mLookAt.IsEqual(mView, 0.01f));

      WD_TEST_BOOL((mLookAt * mViewInv).IsIdentity());
    }
  }
}
