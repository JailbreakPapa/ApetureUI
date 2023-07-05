#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Random.h>
#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>


WD_CREATE_SIMPLE_TEST(Math, Vec3)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdVec3T::ComponentType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdVec3T vDefCtor;
      WD_TEST_BOOL(wdMath::IsNaN(vDefCtor.x) && wdMath::IsNaN(vDefCtor.y) && wdMath::IsNaN(vDefCtor.z));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdVec3T::ComponentType testBlock[3] = {(wdVec3T::ComponentType)1, (wdVec3T::ComponentType)2, (wdVec3T::ComponentType)3};
    wdVec3T* pDefCtor = ::new ((void*)&testBlock[0]) wdVec3T;
    WD_TEST_BOOL(pDefCtor->x == (wdVec3T::ComponentType)1 && pDefCtor->y == (wdVec3T::ComponentType)2 && pDefCtor->z == (wdVec3T::ComponentType)3.);
#endif

    // Make sure the class didn't accidentally change in size.
    WD_TEST_BOOL(sizeof(wdVec3) == 12);
    WD_TEST_BOOL(sizeof(wdVec3d) == 24);

    wdVec3T vInit1F(2.0f);
    WD_TEST_BOOL(vInit1F.x == 2.0f && vInit1F.y == 2.0f && vInit1F.z == 2.0f);

    wdVec3T vInit4F(1.0f, 2.0f, 3.0f);
    WD_TEST_BOOL(vInit4F.x == 1.0f && vInit4F.y == 2.0f && vInit4F.z == 3.0f);

    wdVec3T vCopy(vInit4F);
    WD_TEST_BOOL(vCopy.x == 1.0f && vCopy.y == 2.0f && vCopy.z == 3.0f);

    wdVec3T vZero = wdVec3T::ZeroVector();
    WD_TEST_BOOL(vZero.x == 0.0f && vZero.y == 0.0f && vZero.z == 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion")
  {
    wdVec3T vData(1.0f, 2.0f, 3.0f);
    wdVec2T vToVec2 = vData.GetAsVec2();
    WD_TEST_BOOL(vToVec2.x == vData.x && vToVec2.y == vData.y);

    wdVec4T vToVec4 = vData.GetAsVec4(42.0f);
    WD_TEST_BOOL(vToVec4.x == vData.x && vToVec4.y == vData.y && vToVec4.z == vData.z && vToVec4.w == 42.0f);

    wdVec4T vToVec4Pos = vData.GetAsPositionVec4();
    WD_TEST_BOOL(vToVec4Pos.x == vData.x && vToVec4Pos.y == vData.y && vToVec4Pos.z == vData.z && vToVec4Pos.w == 1.0f);

    wdVec4T vToVec4Dir = vData.GetAsDirectionVec4();
    WD_TEST_BOOL(vToVec4Dir.x == vData.x && vToVec4Dir.y == vData.y && vToVec4Dir.z == vData.z && vToVec4Dir.w == 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Setter")
  {
    wdVec3T vSet1F;
    vSet1F.Set(2.0f);
    WD_TEST_BOOL(vSet1F.x == 2.0f && vSet1F.y == 2.0f && vSet1F.z == 2.0f);

    wdVec3T vSet4F;
    vSet4F.Set(1.0f, 2.0f, 3.0f);
    WD_TEST_BOOL(vSet4F.x == 1.0f && vSet4F.y == 2.0f && vSet4F.z == 3.0f);

    wdVec3T vSetZero;
    vSetZero.SetZero();
    WD_TEST_BOOL(vSetZero.x == 0.0f && vSetZero.y == 0.0f && vSetZero.z == 0.0f);
  }


  {
    const wdVec3T vOp1(-4.0, 4.0f, -2.0f);
    const wdVec3T compArray[3] = {wdVec3T(1.0f, 0.0f, 0.0f), wdVec3T(0.0f, 1.0f, 0.0f), wdVec3T(0.0f, 0.0f, 1.0f)};

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLength") { WD_TEST_FLOAT(vOp1.GetLength(), 6.0f, wdMath::SmallEpsilon<wdMathTestType>()); }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "SetLength")
    {
      wdVec3T vSetLength = vOp1.GetNormalized() * wdMath::DefaultEpsilon<wdMathTestType>();
      WD_TEST_BOOL(vSetLength.SetLength(4.0f, wdMath::LargeEpsilon<wdMathTestType>()) == WD_FAILURE);
      WD_TEST_BOOL(vSetLength == wdVec3T::ZeroVector());
      vSetLength = vOp1.GetNormalized() * (wdMathTestType)0.001;
      WD_TEST_BOOL(vSetLength.SetLength(4.0f, (wdMathTestType)wdMath::DefaultEpsilon<wdMathTestType>()) == WD_SUCCESS);
      WD_TEST_FLOAT(vSetLength.GetLength(), 4.0f, wdMath::SmallEpsilon<wdMathTestType>());
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLengthSquared") { WD_TEST_FLOAT(vOp1.GetLengthSquared(), 36.0f, wdMath::SmallEpsilon<wdMathTestType>()); }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLengthAndNormalize")
    {
      wdVec3T vLengthAndNorm = vOp1;
      wdMathTestType fLength = vLengthAndNorm.GetLengthAndNormalize();
      WD_TEST_FLOAT(vLengthAndNorm.GetLength(), 1.0f, wdMath::SmallEpsilon<wdMathTestType>());
      WD_TEST_FLOAT(fLength, 6.0f, wdMath::SmallEpsilon<wdMathTestType>());
      WD_TEST_FLOAT(vLengthAndNorm.x * vLengthAndNorm.x + vLengthAndNorm.y * vLengthAndNorm.y + vLengthAndNorm.z * vLengthAndNorm.z, 1.0f,
        wdMath::SmallEpsilon<wdMathTestType>());
      WD_TEST_BOOL(vLengthAndNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetNormalized")
    {
      wdVec3T vGetNorm = vOp1.GetNormalized();
      WD_TEST_FLOAT(vGetNorm.x * vGetNorm.x + vGetNorm.y * vGetNorm.y + vGetNorm.z * vGetNorm.z, 1.0f, wdMath::SmallEpsilon<wdMathTestType>());
      WD_TEST_BOOL(vGetNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "Normalize")
    {
      wdVec3T vNorm = vOp1;
      vNorm.Normalize();
      WD_TEST_FLOAT(vNorm.x * vNorm.x + vNorm.y * vNorm.y + vNorm.z * vNorm.z, 1.0f, wdMath::SmallEpsilon<wdMathTestType>());
      WD_TEST_BOOL(vNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "NormalizeIfNotZero")
    {
      wdVec3T vNorm = vOp1;
      vNorm.Normalize();

      wdVec3T vNormCond = vNorm * wdMath::DefaultEpsilon<wdMathTestType>();
      WD_TEST_BOOL(vNormCond.NormalizeIfNotZero(vOp1, wdMath::LargeEpsilon<wdMathTestType>()) == WD_FAILURE);
      WD_TEST_BOOL(vNormCond == vOp1);
      vNormCond = vNorm * wdMath::DefaultEpsilon<wdMathTestType>();
      WD_TEST_BOOL(vNormCond.NormalizeIfNotZero(vOp1, wdMath::SmallEpsilon<wdMathTestType>()) == WD_SUCCESS);
      WD_TEST_VEC3(vNormCond, vNorm, wdMath::DefaultEpsilon<wdVec3T::ComponentType>());
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsZero")
    {
      WD_TEST_BOOL(wdVec3T::ZeroVector().IsZero());
      for (int i = 0; i < 3; ++i)
      {
        WD_TEST_BOOL(!compArray[i].IsZero());
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsZero(float)")
    {
      WD_TEST_BOOL(wdVec3T::ZeroVector().IsZero(0.0f));
      for (int i = 0; i < 3; ++i)
      {
        WD_TEST_BOOL(!compArray[i].IsZero(0.0f));
        WD_TEST_BOOL(compArray[i].IsZero(1.0f));
        WD_TEST_BOOL((-compArray[i]).IsZero(1.0f));
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNormalized (2)")
    {
      for (int i = 0; i < 3; ++i)
      {
        WD_TEST_BOOL(compArray[i].IsNormalized());
        WD_TEST_BOOL((-compArray[i]).IsNormalized());
        WD_TEST_BOOL((compArray[i] * (wdMathTestType)2).IsNormalized((wdMathTestType)4));
        WD_TEST_BOOL((compArray[i] * (wdMathTestType)2).IsNormalized((wdMathTestType)4));
      }
    }

    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdMathTestType fNaN = wdMath::NaN<wdMathTestType>();
      const wdVec3T nanArray[3] = {wdVec3T(fNaN, 0.0f, 0.0f), wdVec3T(0.0f, fNaN, 0.0f), wdVec3T(0.0f, 0.0f, fNaN)};

      WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
      {
        for (int i = 0; i < 3; ++i)
        {
          WD_TEST_BOOL(nanArray[i].IsNaN());
          WD_TEST_BOOL(!compArray[i].IsNaN());
        }
      }

      WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
      {
        for (int i = 0; i < 3; ++i)
        {
          WD_TEST_BOOL(!nanArray[i].IsValid());
          WD_TEST_BOOL(compArray[i].IsValid());

          WD_TEST_BOOL(!(compArray[i] * wdMath::Infinity<wdMathTestType>()).IsValid());
          WD_TEST_BOOL(!(compArray[i] * -wdMath::Infinity<wdMathTestType>()).IsValid());
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    const wdVec3T vOp1(-4.0, 0.2f, -7.0f);
    const wdVec3T vOp2(2.0, 0.3f, 0.0f);
    const wdVec3T compArray[3] = {wdVec3T(1.0f, 0.0f, 0.0f), wdVec3T(0.0f, 1.0f, 0.0f), wdVec3T(0.0f, 0.0f, 1.0f)};
    // IsIdentical
    WD_TEST_BOOL(vOp1.IsIdentical(vOp1));
    for (int i = 0; i < 3; ++i)
    {
      WD_TEST_BOOL(!vOp1.IsIdentical(vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
      WD_TEST_BOOL(!vOp1.IsIdentical(vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
    }

    // IsEqual
    WD_TEST_BOOL(vOp1.IsEqual(vOp1, 0.0f));
    for (int i = 0; i < 3; ++i)
    {
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 + wdMath::SmallEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::SmallEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 - wdMath::SmallEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::SmallEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 + wdMath::DefaultEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::DefaultEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 - wdMath::DefaultEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::DefaultEpsilon<wdMathTestType>()));
    }

    // operator-
    wdVec3T vNegated = -vOp1;
    WD_TEST_BOOL(vOp1.x == -vNegated.x && vOp1.y == -vNegated.y && vOp1.z == -vNegated.z);

    // operator+= (wdVec3T)
    wdVec3T vPlusAssign = vOp1;
    vPlusAssign += vOp2;
    WD_TEST_BOOL(vPlusAssign.IsEqual(wdVec3T(-2.0f, 0.5f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator-= (wdVec3T)
    wdVec3T vMinusAssign = vOp1;
    vMinusAssign -= vOp2;
    WD_TEST_BOOL(vMinusAssign.IsEqual(wdVec3T(-6.0f, -0.1f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator*= (float)
    wdVec3T vMulFloat = vOp1;
    vMulFloat *= 2.0f;
    WD_TEST_BOOL(vMulFloat.IsEqual(wdVec3T(-8.0f, 0.4f, -14.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulFloat *= 0.0f;
    WD_TEST_BOOL(vMulFloat.IsEqual(wdVec3T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator/= (float)
    wdVec3T vDivFloat = vOp1;
    vDivFloat /= 2.0f;
    WD_TEST_BOOL(vDivFloat.IsEqual(wdVec3T(-2.0f, 0.1f, -3.5f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator+ (wdVec3T, wdVec3T)
    wdVec3T vPlus = (vOp1 + vOp2);
    WD_TEST_BOOL(vPlus.IsEqual(wdVec3T(-2.0f, 0.5f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator- (wdVec3T, wdVec3T)
    wdVec3T vMinus = (vOp1 - vOp2);
    WD_TEST_BOOL(vMinus.IsEqual(wdVec3T(-6.0f, -0.1f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator* (float, wdVec3T)
    wdVec3T vMulFloatVec3 = ((wdMathTestType)2 * vOp1);
    WD_TEST_BOOL(
      vMulFloatVec3.IsEqual(wdVec3T((wdMathTestType)-8.0, (wdMathTestType)0.4, (wdMathTestType)-14.0), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulFloatVec3 = ((wdMathTestType)0 * vOp1);
    WD_TEST_BOOL(vMulFloatVec3.IsEqual(wdVec3T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator* (wdVec3T, float)
    wdVec3T vMulVec3Float = (vOp1 * (wdMathTestType)2);
    WD_TEST_BOOL(vMulVec3Float.IsEqual(wdVec3T(-8.0f, 0.4f, -14.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulVec3Float = (vOp1 * (wdMathTestType)0);
    WD_TEST_BOOL(vMulVec3Float.IsEqual(wdVec3T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator/ (wdVec3T, float)
    wdVec3T vDivVec3Float = (vOp1 / (wdMathTestType)2);
    WD_TEST_BOOL(vDivVec3Float.IsEqual(wdVec3T(-2.0f, 0.1f, -3.5f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator== (wdVec3T, wdVec3T)
    WD_TEST_BOOL(vOp1 == vOp1);
    for (int i = 0; i < 3; ++i)
    {
      WD_TEST_BOOL(!(vOp1 == (vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i])));
      WD_TEST_BOOL(!(vOp1 == (vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i])));
    }

    // operator!= (wdVec3T, wdVec3T)
    WD_TEST_BOOL(!(vOp1 != vOp1));
    for (int i = 0; i < 3; ++i)
    {
      WD_TEST_BOOL(vOp1 != (vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
      WD_TEST_BOOL(vOp1 != (vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
    }

    // operator< (wdVec3T, wdVec3T)
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        if (i == j)
        {
          WD_TEST_BOOL(!(compArray[i] < compArray[j]));
          WD_TEST_BOOL(!(compArray[j] < compArray[i]));
        }
        else if (i < j)
        {
          WD_TEST_BOOL(!(compArray[i] < compArray[j]));
          WD_TEST_BOOL(compArray[j] < compArray[i]);
        }
        else
        {
          WD_TEST_BOOL(!(compArray[j] < compArray[i]));
          WD_TEST_BOOL(compArray[i] < compArray[j]);
        }
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Common")
  {
    const wdVec3T vOp1(-4.0, 0.2f, -7.0f);
    const wdVec3T vOp2(2.0, -0.3f, 0.5f);

    const wdVec3T compArray[3] = {wdVec3T(1.0f, 0.0f, 0.0f), wdVec3T(0.0f, 1.0f, 0.0f), wdVec3T(0.0f, 0.0f, 1.0f)};

    // GetAngleBetween
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        WD_TEST_FLOAT(compArray[i].GetAngleBetween(compArray[j]).GetDegree(), i == j ? 0.0f : 90.0f, 0.00001f);
      }
    }

    // Dot
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        WD_TEST_FLOAT(compArray[i].Dot(compArray[j]), i == j ? 1.0f : 0.0f, wdMath::SmallEpsilon<wdMathTestType>());
      }
    }
    WD_TEST_FLOAT(vOp1.Dot(vOp2), -11.56f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_FLOAT(vOp2.Dot(vOp1), -11.56f, wdMath::SmallEpsilon<wdMathTestType>());

    // Cross
    // Right-handed coordinate system check
    WD_TEST_BOOL(compArray[0].CrossRH(compArray[1]).IsEqual(compArray[2], wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(compArray[1].CrossRH(compArray[2]).IsEqual(compArray[0], wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(compArray[2].CrossRH(compArray[0]).IsEqual(compArray[1], wdMath::SmallEpsilon<wdMathTestType>()));

    // CompMin
    WD_TEST_BOOL(vOp1.CompMin(vOp2).IsEqual(wdVec3T(-4.0f, -0.3f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMin(vOp1).IsEqual(wdVec3T(-4.0f, -0.3f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompMax
    WD_TEST_BOOL(vOp1.CompMax(vOp2).IsEqual(wdVec3T(2.0f, 0.2f, 0.5f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMax(vOp1).IsEqual(wdVec3T(2.0f, 0.2f, 0.5f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompClamp
    WD_TEST_BOOL(vOp1.CompClamp(vOp1, vOp2).IsEqual(wdVec3T(-4.0f, -0.3f, -7.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompClamp(vOp1, vOp2).IsEqual(wdVec3T(2.0f, 0.2f, 0.5f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompMul
    WD_TEST_BOOL(vOp1.CompMul(vOp2).IsEqual(wdVec3T(-8.0f, -0.06f, -3.5f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMul(vOp1).IsEqual(wdVec3T(-8.0f, -0.06f, -3.5f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompDiv
    WD_TEST_BOOL(vOp1.CompDiv(vOp2).IsEqual(wdVec3T(-2.0f, -0.66666666f, -14.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // Abs
    WD_TEST_VEC3(vOp1.Abs(), wdVec3T(4.0, 0.2f, 7.0f), wdMath::SmallEpsilon<wdMathTestType>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CalculateNormal")
  {
    wdVec3T n;
    WD_TEST_BOOL(n.CalculateNormal(wdVec3T(-1, 0, 1), wdVec3T(1, 0, 1), wdVec3T(0, 0, -1)) == WD_SUCCESS);
    WD_TEST_VEC3(n, wdVec3T(0, 1, 0), 0.001f);

    WD_TEST_BOOL(n.CalculateNormal(wdVec3T(-1, 0, -1), wdVec3T(1, 0, -1), wdVec3T(0, 0, 1)) == WD_SUCCESS);
    WD_TEST_VEC3(n, wdVec3T(0, -1, 0), 0.001f);

    WD_TEST_BOOL(n.CalculateNormal(wdVec3T(-1, 0, 1), wdVec3T(1, 0, 1), wdVec3T(1, 0, 1)) == WD_FAILURE);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeOrthogonalTo")
  {
    wdVec3T v;

    v.Set(1, 1, 0);
    v.MakeOrthogonalTo(wdVec3T(1, 0, 0));
    WD_TEST_VEC3(v, wdVec3T(0, 1, 0), 0.001f);

    v.Set(1, 1, 0);
    v.MakeOrthogonalTo(wdVec3T(0, 1, 0));
    WD_TEST_VEC3(v, wdVec3T(1, 0, 0), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetOrthogonalVector")
  {
    wdVec3T v;

    for (float i = 1; i < 360; i += 3.0f)
    {
      v.Set(i, i * 3, i * 7);
      WD_TEST_FLOAT(v.GetOrthogonalVector().Dot(v), 0.0f, 0.001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetReflectedVector")
  {
    wdVec3T v, v2;

    v.Set(1, 1, 0);
    v2 = v.GetReflectedVector(wdVec3T(0, -1, 0));
    WD_TEST_VEC3(v2, wdVec3T(1, -1, 0), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomPointInSphere")
  {
    wdVec3T v;

    wdRandom rng;
    rng.Initialize(0xEEFF0011AABBCCDDULL);

    wdVec3T avg;
    avg.SetZero();

    const wdUInt32 uiNumSamples = 100'000;
    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      v = wdVec3T::CreateRandomPointInSphere(rng);

      WD_TEST_BOOL(v.GetLength() <= 1.0f + wdMath::SmallEpsilon<float>());
      WD_TEST_BOOL(!v.IsZero());

      avg += v;
    }

    avg /= (float)uiNumSamples;

    // the average point cloud center should be within at least 10% of the sphere's center
    // otherwise the points aren't equally distributed
    WD_TEST_BOOL(avg.IsZero(0.1f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomDirection")
  {
    wdVec3T v;

    wdRandom rng;
    rng.InitializeFromCurrentTime();

    wdVec3T avg;
    avg.SetZero();

    const wdUInt32 uiNumSamples = 100'000;
    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      v = wdVec3T::CreateRandomDirection(rng);

      WD_TEST_BOOL(v.IsNormalized());

      avg += v;
    }

    avg /= (float)uiNumSamples;

    // the average point cloud center should be within at least 10% of the sphere's center
    // otherwise the points aren't equally distributed
    WD_TEST_BOOL(avg.IsZero(0.1f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomDeviationX")
  {
    wdVec3T v;
    wdVec3T avg;
    avg.SetZero();

    wdRandom rng;
    rng.InitializeFromCurrentTime();

    const wdAngle dev = wdAngle::Degree(65);
    const wdUInt32 uiNumSamples = 100'000;
    const wdVec3 vAxis(1, 0, 0);

    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      v = wdVec3T::CreateRandomDeviationX(rng, dev);

      WD_TEST_BOOL(v.IsNormalized());

      WD_TEST_BOOL(vAxis.GetAngleBetween(v).GetRadian() <= dev.GetRadian() + wdMath::DefaultEpsilon<float>());

      avg += v;
    }

    // average direction should be close to the main axis
    avg.Normalize();
    WD_TEST_BOOL(avg.IsEqual(vAxis, 0.1f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomDeviationY")
  {
    wdVec3T v;
    wdVec3T avg;
    avg.SetZero();

    wdRandom rng;
    rng.InitializeFromCurrentTime();

    const wdAngle dev = wdAngle::Degree(65);
    const wdUInt32 uiNumSamples = 100'000;
    const wdVec3 vAxis(0, 1, 0);

    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      v = wdVec3T::CreateRandomDeviationY(rng, dev);

      WD_TEST_BOOL(v.IsNormalized());

      WD_TEST_BOOL(vAxis.GetAngleBetween(v).GetRadian() <= dev.GetRadian() + wdMath::DefaultEpsilon<float>());

      avg += v;
    }

    // average direction should be close to the main axis
    avg.Normalize();
    WD_TEST_BOOL(avg.IsEqual(vAxis, 0.1f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomDeviationZ")
  {
    wdVec3T v;
    wdVec3T avg;
    avg.SetZero();

    wdRandom rng;
    rng.InitializeFromCurrentTime();

    const wdAngle dev = wdAngle::Degree(65);
    const wdUInt32 uiNumSamples = 100'000;
    const wdVec3 vAxis(0, 0, 1);

    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      v = wdVec3T::CreateRandomDeviationZ(rng, dev);

      WD_TEST_BOOL(v.IsNormalized());

      WD_TEST_BOOL(vAxis.GetAngleBetween(v).GetRadian() <= dev.GetRadian() + wdMath::DefaultEpsilon<float>());

      avg += v;
    }

    // average direction should be close to the main axis
    avg.Normalize();
    WD_TEST_BOOL(avg.IsEqual(vAxis, 0.1f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CreateRandomDeviation")
  {
    wdVec3T v;

    wdRandom rng;
    rng.InitializeFromCurrentTime();

    const wdAngle dev = wdAngle::Degree(65);
    const wdUInt32 uiNumSamples = 100'000;
    wdVec3 vAxis;

    for (wdUInt32 i = 0; i < uiNumSamples; ++i)
    {
      vAxis = wdVec3T::CreateRandomDirection(rng);

      v = wdVec3T::CreateRandomDeviation(rng, dev, vAxis);

      WD_TEST_BOOL(v.IsNormalized());

      WD_TEST_BOOL(vAxis.GetAngleBetween(v).GetDegree() <= dev.GetDegree() + 1.0f);
    }
  }
}
