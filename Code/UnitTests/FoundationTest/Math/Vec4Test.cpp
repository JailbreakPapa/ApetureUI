#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>


WD_CREATE_SIMPLE_TEST(Math, Vec4)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdVec4T vDefCtor;
      WD_TEST_BOOL(wdMath::IsNaN(vDefCtor.x) && wdMath::IsNaN(vDefCtor.y) /* && wdMath::IsNaN(vDefCtor.z) && wdMath::IsNaN(vDefCtor.w)*/);
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdVec4T::ComponentType testBlock[4] = {
      (wdVec4T::ComponentType)1, (wdVec4T::ComponentType)2, (wdVec4T::ComponentType)3, (wdVec4T::ComponentType)4};
    wdVec4T* pDefCtor = ::new ((void*)&testBlock[0]) wdVec4T;
    WD_TEST_BOOL(pDefCtor->x == (wdVec4T::ComponentType)1 && pDefCtor->y == (wdVec4T::ComponentType)2 && pDefCtor->z == (wdVec4T::ComponentType)3 &&
                 pDefCtor->w == (wdVec4T::ComponentType)4);
#endif

    // Make sure the class didn't accidentally change in size.
    WD_TEST_BOOL(sizeof(wdVec4) == 16);
    WD_TEST_BOOL(sizeof(wdVec4d) == 32);

    wdVec4T vInit1F(2.0f);
    WD_TEST_BOOL(vInit1F.x == 2.0f && vInit1F.y == 2.0f && vInit1F.z == 2.0f && vInit1F.w == 2.0f);

    wdVec4T vInit4F(1.0f, 2.0f, 3.0f, 4.0f);
    WD_TEST_BOOL(vInit4F.x == 1.0f && vInit4F.y == 2.0f && vInit4F.z == 3.0f && vInit4F.w == 4.0f);

    wdVec4T vCopy(vInit4F);
    WD_TEST_BOOL(vCopy.x == 1.0f && vCopy.y == 2.0f && vCopy.z == 3.0f && vCopy.w == 4.0f);

    wdVec4T vZero = wdVec4T::ZeroVector();
    WD_TEST_BOOL(vZero.x == 0.0f && vZero.y == 0.0f && vZero.z == 0.0f && vZero.w == 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion")
  {
    wdVec4T vData(1.0f, 2.0f, 3.0f, 4.0f);
    wdVec2T vToVec2 = vData.GetAsVec2();
    WD_TEST_BOOL(vToVec2.x == vData.x && vToVec2.y == vData.y);

    wdVec3T vToVec3 = vData.GetAsVec3();
    WD_TEST_BOOL(vToVec3.x == vData.x && vToVec3.y == vData.y && vToVec3.z == vData.z);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Setter")
  {
    wdVec4T vSet1F;
    vSet1F.Set(2.0f);
    WD_TEST_BOOL(vSet1F.x == 2.0f && vSet1F.y == 2.0f && vSet1F.z == 2.0f && vSet1F.w == 2.0f);

    wdVec4T vSet4F;
    vSet4F.Set(1.0f, 2.0f, 3.0f, 4.0f);
    WD_TEST_BOOL(vSet4F.x == 1.0f && vSet4F.y == 2.0f && vSet4F.z == 3.0f && vSet4F.w == 4.0f);

    wdVec4T vSetZero;
    vSetZero.SetZero();
    WD_TEST_BOOL(vSetZero.x == 0.0f && vSetZero.y == 0.0f && vSetZero.z == 0.0f && vSetZero.w == 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Length")
  {
    const wdVec4T vOp1(-4.0, 4.0f, -2.0f, -0.0f);
    const wdVec4T compArray[4] = {
      wdVec4T(1.0f, 0.0f, 0.0f, 0.0f), wdVec4T(0.0f, 1.0f, 0.0f, 0.0f), wdVec4T(0.0f, 0.0f, 1.0f, 0.0f), wdVec4T(0.0f, 0.0f, 0.0f, 1.0f)};

    // GetLength
    WD_TEST_FLOAT(vOp1.GetLength(), 6.0f, wdMath::SmallEpsilon<wdMathTestType>());

    // GetLengthSquared
    WD_TEST_FLOAT(vOp1.GetLengthSquared(), 36.0f, wdMath::SmallEpsilon<wdMathTestType>());

    // GetLengthAndNormalize
    wdVec4T vLengthAndNorm = vOp1;
    wdMathTestType fLength = vLengthAndNorm.GetLengthAndNormalize();
    WD_TEST_FLOAT(vLengthAndNorm.GetLength(), 1.0f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_FLOAT(fLength, 6.0f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_FLOAT(vLengthAndNorm.x * vLengthAndNorm.x + vLengthAndNorm.y * vLengthAndNorm.y + vLengthAndNorm.z * vLengthAndNorm.z +
                    vLengthAndNorm.w * vLengthAndNorm.w,
      1.0f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_BOOL(vLengthAndNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));

    // GetNormalized
    wdVec4T vGetNorm = vOp1.GetNormalized();
    WD_TEST_FLOAT(vGetNorm.x * vGetNorm.x + vGetNorm.y * vGetNorm.y + vGetNorm.z * vGetNorm.z + vGetNorm.w * vGetNorm.w, 1.0f,
      wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_BOOL(vGetNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));

    // Normalize
    wdVec4T vNorm = vOp1;
    vNorm.Normalize();
    WD_TEST_FLOAT(vNorm.x * vNorm.x + vNorm.y * vNorm.y + vNorm.z * vNorm.z + vNorm.w * vNorm.w, 1.0f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_BOOL(vNorm.IsNormalized(wdMath::SmallEpsilon<wdMathTestType>()));

    // NormalizeIfNotZero
    wdVec4T vNormCond = vNorm * wdMath::DefaultEpsilon<wdMathTestType>();
    WD_TEST_BOOL(vNormCond.NormalizeIfNotZero(vOp1, wdMath::LargeEpsilon<wdMathTestType>()) == WD_FAILURE);
    WD_TEST_BOOL(vNormCond == vOp1);
    vNormCond = vNorm * wdMath::DefaultEpsilon<wdMathTestType>();
    WD_TEST_BOOL(vNormCond.NormalizeIfNotZero(vOp1, wdMath::SmallEpsilon<wdMathTestType>()) == WD_SUCCESS);
    WD_TEST_VEC4(vNormCond, vNorm, wdMath::DefaultEpsilon<wdVec4T::ComponentType>());

    // IsZero
    WD_TEST_BOOL(wdVec4T::ZeroVector().IsZero());
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(!compArray[i].IsZero());
    }

    // IsZero(float)
    WD_TEST_BOOL(wdVec4T::ZeroVector().IsZero(0.0f));
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(!compArray[i].IsZero(0.0f));
      WD_TEST_BOOL(compArray[i].IsZero(1.0f));
      WD_TEST_BOOL((-compArray[i]).IsZero(1.0f));
    }

    // IsNormalized (already tested above)
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(compArray[i].IsNormalized());
      WD_TEST_BOOL((-compArray[i]).IsNormalized());
      WD_TEST_BOOL((compArray[i] * (wdMathTestType)2).IsNormalized((wdMathTestType)4));
      WD_TEST_BOOL((compArray[i] * (wdMathTestType)2).IsNormalized((wdMathTestType)4));
    }

    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdMathTestType TypeNaN = wdMath::NaN<wdMathTestType>();
      const wdVec4T nanArray[4] = {wdVec4T(TypeNaN, 0.0f, 0.0f, 0.0f), wdVec4T(0.0f, TypeNaN, 0.0f, 0.0f), wdVec4T(0.0f, 0.0f, TypeNaN, 0.0f),
        wdVec4T(0.0f, 0.0f, 0.0f, TypeNaN)};

      // IsNaN
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(nanArray[i].IsNaN());
        WD_TEST_BOOL(!compArray[i].IsNaN());
      }

      // IsValid
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(!nanArray[i].IsValid());
        WD_TEST_BOOL(compArray[i].IsValid());

        WD_TEST_BOOL(!(compArray[i] * wdMath::Infinity<wdMathTestType>()).IsValid());
        WD_TEST_BOOL(!(compArray[i] * -wdMath::Infinity<wdMathTestType>()).IsValid());
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    const wdVec4T vOp1(-4.0, 0.2f, -7.0f, -0.0f);
    const wdVec4T vOp2(2.0, 0.3f, 0.0f, 1.0f);
    const wdVec4T compArray[4] = {
      wdVec4T(1.0f, 0.0f, 0.0f, 0.0f), wdVec4T(0.0f, 1.0f, 0.0f, 0.0f), wdVec4T(0.0f, 0.0f, 1.0f, 0.0f), wdVec4T(0.0f, 0.0f, 0.0f, 1.0f)};
    // IsIdentical
    WD_TEST_BOOL(vOp1.IsIdentical(vOp1));
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(!vOp1.IsIdentical(vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
      WD_TEST_BOOL(!vOp1.IsIdentical(vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
    }

    // IsEqual
    WD_TEST_BOOL(vOp1.IsEqual(vOp1, 0.0f));
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 + wdMath::SmallEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::SmallEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 - wdMath::SmallEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::SmallEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 + wdMath::DefaultEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::DefaultEpsilon<wdMathTestType>()));
      WD_TEST_BOOL(vOp1.IsEqual(vOp1 - wdMath::DefaultEpsilon<wdMathTestType>() * compArray[i], 2 * wdMath::DefaultEpsilon<wdMathTestType>()));
    }

    // operator-
    wdVec4T vNegated = -vOp1;
    WD_TEST_BOOL(vOp1.x == -vNegated.x && vOp1.y == -vNegated.y && vOp1.z == -vNegated.z && vOp1.w == -vNegated.w);

    // operator+= (wdVec4T)
    wdVec4T vPlusAssign = vOp1;
    vPlusAssign += vOp2;
    WD_TEST_BOOL(vPlusAssign.IsEqual(wdVec4T(-2.0f, 0.5f, -7.0f, 1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator-= (wdVec4T)
    wdVec4T vMinusAssign = vOp1;
    vMinusAssign -= vOp2;
    WD_TEST_BOOL(vMinusAssign.IsEqual(wdVec4T(-6.0f, -0.1f, -7.0f, -1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator*= (float)
    wdVec4T vMulFloat = vOp1;
    vMulFloat *= 2.0f;
    WD_TEST_BOOL(vMulFloat.IsEqual(wdVec4T(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulFloat *= 0.0f;
    WD_TEST_BOOL(vMulFloat.IsEqual(wdVec4T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator/= (float)
    wdVec4T vDivFloat = vOp1;
    vDivFloat /= 2.0f;
    WD_TEST_BOOL(vDivFloat.IsEqual(wdVec4T(-2.0f, 0.1f, -3.5f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator+ (wdVec4T, wdVec4T)
    wdVec4T vPlus = (vOp1 + vOp2);
    WD_TEST_BOOL(vPlus.IsEqual(wdVec4T(-2.0f, 0.5f, -7.0f, 1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator- (wdVec4T, wdVec4T)
    wdVec4T vMinus = (vOp1 - vOp2);
    WD_TEST_BOOL(vMinus.IsEqual(wdVec4T(-6.0f, -0.1f, -7.0f, -1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator* (float, wdVec4T)
    wdVec4T vMulFloatVec4 = ((wdMathTestType)2 * vOp1);
    WD_TEST_BOOL(vMulFloatVec4.IsEqual(wdVec4T(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulFloatVec4 = ((wdMathTestType)0 * vOp1);
    WD_TEST_BOOL(vMulFloatVec4.IsEqual(wdVec4T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator* (wdVec4T, float)
    wdVec4T vMulVec4Float = (vOp1 * (wdMathTestType)2);
    WD_TEST_BOOL(vMulVec4Float.IsEqual(wdVec4T(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    vMulVec4Float = (vOp1 * (wdMathTestType)0);
    WD_TEST_BOOL(vMulVec4Float.IsEqual(wdVec4T::ZeroVector(), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator/ (wdVec4T, float)
    wdVec4T vDivVec4Float = (vOp1 / (wdMathTestType)2);
    WD_TEST_BOOL(vDivVec4Float.IsEqual(wdVec4T(-2.0f, 0.1f, -3.5f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // operator== (wdVec4T, wdVec4T)
    WD_TEST_BOOL(vOp1 == vOp1);
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(!(vOp1 == (vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i])));
      WD_TEST_BOOL(!(vOp1 == (vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i])));
    }

    // operator!= (wdVec4T, wdVec4T)
    WD_TEST_BOOL(!(vOp1 != vOp1));
    for (int i = 0; i < 4; ++i)
    {
      WD_TEST_BOOL(vOp1 != (vOp1 + (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
      WD_TEST_BOOL(vOp1 != (vOp1 - (wdMathTestType)wdMath::SmallEpsilon<wdMathTestType>() * compArray[i]));
    }

    // operator< (wdVec4T, wdVec4T)
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
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
    const wdVec4T vOp1(-4.0, 0.2f, -7.0f, -0.0f);
    const wdVec4T vOp2(2.0, -0.3f, 0.5f, 1.0f);

    // Dot
    WD_TEST_FLOAT(vOp1.Dot(vOp2), -11.56f, wdMath::SmallEpsilon<wdMathTestType>());
    WD_TEST_FLOAT(vOp2.Dot(vOp1), -11.56f, wdMath::SmallEpsilon<wdMathTestType>());

    // CompMin
    WD_TEST_BOOL(vOp1.CompMin(vOp2).IsEqual(wdVec4T(-4.0f, -0.3f, -7.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMin(vOp1).IsEqual(wdVec4T(-4.0f, -0.3f, -7.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompMax
    WD_TEST_BOOL(vOp1.CompMax(vOp2).IsEqual(wdVec4T(2.0f, 0.2f, 0.5f, 1.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMax(vOp1).IsEqual(wdVec4T(2.0f, 0.2f, 0.5f, 1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompClamp
    WD_TEST_BOOL(vOp1.CompClamp(vOp1, vOp2).IsEqual(wdVec4T(-4.0f, -0.3f, -7.0f, -0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompClamp(vOp1, vOp2).IsEqual(wdVec4T(2.0f, 0.2f, 0.5f, 1.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompMul
    WD_TEST_BOOL(vOp1.CompMul(vOp2).IsEqual(wdVec4T(-8.0f, -0.06f, -3.5f, 0.0f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompMul(vOp1).IsEqual(wdVec4T(-8.0f, -0.06f, -3.5f, 0.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // CompDiv
    WD_TEST_BOOL(vOp1.CompDiv(vOp2).IsEqual(wdVec4T(-2.0f, -0.66666666f, -14.0f, 0.0f), wdMath::SmallEpsilon<wdMathTestType>()));

    // Abs
    WD_TEST_VEC4(vOp1.Abs(), wdVec4T(4.0, 0.2f, 7.0f, 0.0f), wdMath::SmallEpsilon<wdMathTestType>());
  }
}
