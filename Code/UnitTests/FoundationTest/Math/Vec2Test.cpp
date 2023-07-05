#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>

#include <Foundation/Math/FixedPoint.h>

WD_CREATE_SIMPLE_TEST(Math, Vec2)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdVec2T vDefCtor;
      WD_TEST_BOOL(wdMath::IsNaN(vDefCtor.x) && wdMath::IsNaN(vDefCtor.y));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    wdVec2T::ComponentType testBlock[2] = {(wdVec2T::ComponentType)1, (wdVec2T::ComponentType)2};
    wdVec2T* pDefCtor = ::new ((void*)&testBlock[0]) wdVec2T;
    WD_TEST_BOOL(pDefCtor->x == (wdVec2T::ComponentType)1 && pDefCtor->y == (wdVec2T::ComponentType)2);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(x,y)")
  {
    wdVec2T v(1, 2);
    WD_TEST_FLOAT(v.x, 1, 0);
    WD_TEST_FLOAT(v.y, 2, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(xy)")
  {
    wdVec2T v(3);
    WD_TEST_VEC2(v, wdVec2T(3, 3), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ZeroVector") { WD_TEST_VEC2(wdVec2T::ZeroVector(), wdVec2T(0, 0), 0); }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsVec3") { WD_TEST_VEC3(wdVec2T(2, 3).GetAsVec3(4), wdVec3T(2, 3, 4), 0); }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAsVec4") { WD_TEST_VEC4(wdVec2T(2, 3).GetAsVec4(4, 5), wdVec4T(2, 3, 4, 5), 0); }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Set(x, y)")
  {
    wdVec2T v;
    v.Set(2, 3);

    WD_TEST_FLOAT(v.x, 2, 0);
    WD_TEST_FLOAT(v.y, 3, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Set(xy)")
  {
    wdVec2T v;
    v.Set(4);

    WD_TEST_FLOAT(v.x, 4, 0);
    WD_TEST_FLOAT(v.y, 4, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetZero")
  {
    wdVec2T v;
    v.Set(4);
    v.SetZero();

    WD_TEST_FLOAT(v.x, 0, 0);
    WD_TEST_FLOAT(v.y, 0, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLength")
  {
    wdVec2T v(0);
    WD_TEST_FLOAT(v.GetLength(), 0, 0.0001f);

    v.Set(1, 0);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(0, 1);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(2, 3);
    WD_TEST_FLOAT(v.GetLength(), wdMath::Sqrt((wdMathTestType)(4 + 9)), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLengthSquared")
  {
    wdVec2T v(0);
    WD_TEST_FLOAT(v.GetLengthSquared(), 0, 0.0001f);

    v.Set(1, 0);
    WD_TEST_FLOAT(v.GetLengthSquared(), 1, 0.0001f);

    v.Set(0, 1);
    WD_TEST_FLOAT(v.GetLengthSquared(), 1, 0.0001f);

    v.Set(2, 3);
    WD_TEST_FLOAT(v.GetLengthSquared(), 4 + 9, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLengthAndNormalize")
  {
    wdVec2T v(0.5f, 0);
    wdVec2T::ComponentType l = v.GetLengthAndNormalize();
    WD_TEST_FLOAT(l, 0.5f, 0.0001f);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(1, 0);
    l = v.GetLengthAndNormalize();
    WD_TEST_FLOAT(l, 1, 0.0001f);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(0, 1);
    l = v.GetLengthAndNormalize();
    WD_TEST_FLOAT(l, 1, 0.0001f);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);

    v.Set(2, 3);
    l = v.GetLengthAndNormalize();
    WD_TEST_FLOAT(l, wdMath::Sqrt((wdMathTestType)(4 + 9)), 0.0001f);
    WD_TEST_FLOAT(v.GetLength(), 1, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetNormalized")
  {
    wdVec2T v;

    v.Set(10, 0);
    WD_TEST_VEC2(v.GetNormalized(), wdVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    WD_TEST_VEC2(v.GetNormalized(), wdVec2T(0, 1), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Normalize")
  {
    wdVec2T v;

    v.Set(10, 0);
    v.Normalize();
    WD_TEST_VEC2(v, wdVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    v.Normalize();
    WD_TEST_VEC2(v, wdVec2T(0, 1), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "NormalizeIfNotZero")
  {
    wdVec2T v;

    v.Set(10, 0);
    WD_TEST_BOOL(v.NormalizeIfNotZero() == WD_SUCCESS);
    WD_TEST_VEC2(v, wdVec2T(1, 0), 0.001f);

    v.Set(0, 10);
    WD_TEST_BOOL(v.NormalizeIfNotZero() == WD_SUCCESS);
    WD_TEST_VEC2(v, wdVec2T(0, 1), 0.001f);

    v.SetZero();
    WD_TEST_BOOL(v.NormalizeIfNotZero() == WD_FAILURE);
    WD_TEST_VEC2(v, wdVec2T(1, 0), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsZero")
  {
    wdVec2T v;

    v.Set(1);
    WD_TEST_BOOL(v.IsZero() == false);

    v.Set(0.001f);
    WD_TEST_BOOL(v.IsZero() == false);
    WD_TEST_BOOL(v.IsZero(0.01f) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNormalized")
  {
    wdVec2T v;

    v.SetZero();
    WD_TEST_BOOL(v.IsNormalized(wdMath::HugeEpsilon<wdMathTestType>()) == false);

    v.Set(1, 0);
    WD_TEST_BOOL(v.IsNormalized(wdMath::HugeEpsilon<wdMathTestType>()) == true);

    v.Set(0, 1);
    WD_TEST_BOOL(v.IsNormalized(wdMath::HugeEpsilon<wdMathTestType>()) == true);

    v.Set(0.1f, 1);
    WD_TEST_BOOL(v.IsNormalized(wdMath::DefaultEpsilon<wdMathTestType>()) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdVec2T v(0);

      WD_TEST_BOOL(!v.IsNaN());

      v.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(v.IsNaN());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      wdVec2T v(0);

      WD_TEST_BOOL(v.IsValid());

      v.x = wdMath::NaN<wdMathTestType>();
      WD_TEST_BOOL(!v.IsValid());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-")
  {
    wdVec2T v(1);

    WD_TEST_VEC2(-v, wdVec2T(-1), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+=")
  {
    wdVec2T v(1, 2);

    v += wdVec2T(3, 4);
    WD_TEST_VEC2(v, wdVec2T(4, 6), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-=")
  {
    wdVec2T v(1, 2);

    v -= wdVec2T(3, 5);
    WD_TEST_VEC2(v, wdVec2T(-2, -3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator*=(float)")
  {
    wdVec2T v(1, 2);

    v *= 3;
    WD_TEST_VEC2(v, wdVec2T(3, 6), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/=(float)")
  {
    wdVec2T v(1, 2);

    v /= 2;
    WD_TEST_VEC2(v, wdVec2T(0.5f, 1), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdentical")
  {
    wdVec2T v1(1, 2);
    wdVec2T v2 = v1;

    WD_TEST_BOOL(v1.IsIdentical(v2));

    v2.x += wdVec2T::ComponentType(0.001f);
    WD_TEST_BOOL(!v1.IsIdentical(v2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    wdVec2T v1(1, 2);
    wdVec2T v2 = v1;

    WD_TEST_BOOL(v1.IsEqual(v2, 0.00001f));

    v2.x += wdVec2T::ComponentType(0.001f);
    WD_TEST_BOOL(!v1.IsEqual(v2, 0.0001f));
    WD_TEST_BOOL(v1.IsEqual(v2, 0.01f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetAngleBetween")
  {
    wdVec2T v1(1, 0);
    wdVec2T v2(0, 1);

    WD_TEST_FLOAT(v1.GetAngleBetween(v1).GetDegree(), 0, 0.001f);
    WD_TEST_FLOAT(v2.GetAngleBetween(v2).GetDegree(), 0, 0.001f);
    WD_TEST_FLOAT(v1.GetAngleBetween(v2).GetDegree(), 90, 0.001f);
    WD_TEST_FLOAT(v1.GetAngleBetween(-v1).GetDegree(), 180, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Dot")
  {
    wdVec2T v1(1, 0);
    wdVec2T v2(0, 1);
    wdVec2T v0(0, 0);

    WD_TEST_FLOAT(v0.Dot(v0), 0, 0.001f);
    WD_TEST_FLOAT(v1.Dot(v1), 1, 0.001f);
    WD_TEST_FLOAT(v2.Dot(v2), 1, 0.001f);
    WD_TEST_FLOAT(v1.Dot(v2), 0, 0.001f);
    WD_TEST_FLOAT(v1.Dot(-v1), -1, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompMin")
  {
    wdVec2T v1(2, 3);
    wdVec2T v2 = v1.CompMin(wdVec2T(1, 4));
    WD_TEST_VEC2(v2, wdVec2T(1, 3), 0);

    v2 = v1.CompMin(wdVec2T(3, 1));
    WD_TEST_VEC2(v2, wdVec2T(2, 1), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompMax")
  {
    wdVec2T v1(2, 3.5f);
    wdVec2T v2 = v1.CompMax(wdVec2T(1, 4));
    WD_TEST_VEC2(v2, wdVec2T(2, 4), 0);

    v2 = v1.CompMax(wdVec2T(3, 1));
    WD_TEST_VEC2(v2, wdVec2T(3, 3.5f), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompClamp")
  {
    const wdVec2T vOp1(-4.0, 0.2f);
    const wdVec2T vOp2(2.0, -0.3f);

    WD_TEST_BOOL(vOp1.CompClamp(vOp1, vOp2).IsEqual(wdVec2T(-4.0f, -0.3f), wdMath::SmallEpsilon<wdMathTestType>()));
    WD_TEST_BOOL(vOp2.CompClamp(vOp1, vOp2).IsEqual(wdVec2T(2.0f, 0.2f), wdMath::SmallEpsilon<wdMathTestType>()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompMul")
  {
    wdVec2T v1(2, 3);
    wdVec2T v2 = v1.CompMul(wdVec2T(2, 4));
    WD_TEST_VEC2(v2, wdVec2T(4, 12), 0);

    v2 = v1.CompMul(wdVec2T(3, 7));
    WD_TEST_VEC2(v2, wdVec2T(6, 21), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompDiv")
  {
    wdVec2T v1(12, 32);
    wdVec2T v2 = v1.CompDiv(wdVec2T(3, 4));
    WD_TEST_VEC2(v2, wdVec2T(4, 8), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Abs")
  {
    wdVec2T v1(-5, 7);
    wdVec2T v2 = v1.Abs();
    WD_TEST_VEC2(v2, wdVec2T(5, 7), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeOrthogonalTo")
  {
    wdVec2T v;

    v.Set(1, 1);
    v.MakeOrthogonalTo(wdVec2T(1, 0));
    WD_TEST_VEC2(v, wdVec2T(0, 1), 0.001f);

    v.Set(1, 1);
    v.MakeOrthogonalTo(wdVec2T(0, 1));
    WD_TEST_VEC2(v, wdVec2T(1, 0), 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetOrthogonalVector")
  {
    wdVec2T v;

    for (float i = 1; i < 360; i += 3)
    {
      v.Set(i, i * 3);
      WD_TEST_FLOAT(v.GetOrthogonalVector().Dot(v), 0, 0.001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetReflectedVector")
  {
    wdVec2T v, v2;

    v.Set(1, 1);
    v2 = v.GetReflectedVector(wdVec2T(0, -1));
    WD_TEST_VEC2(v2, wdVec2T(1, -1), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+")
  {
    wdVec2T v = wdVec2T(1, 2) + wdVec2T(3, 4);
    WD_TEST_VEC2(v, wdVec2T(4, 6), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-")
  {
    wdVec2T v = wdVec2T(1, 2) - wdVec2T(3, 5);
    WD_TEST_VEC2(v, wdVec2T(-2, -3), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator* (vec, float) | operator* (float, vec)")
  {
    wdVec2T v = wdVec2T(1, 2) * wdVec2T::ComponentType(3);
    WD_TEST_VEC2(v, wdVec2T(3, 6), 0.0001f);

    v = wdVec2T::ComponentType(7) * wdVec2T(1, 2);
    WD_TEST_VEC2(v, wdVec2T(7, 14), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/ (vec, float)")
  {
    wdVec2T v = wdVec2T(2, 4) / wdVec2T::ComponentType(2);
    WD_TEST_VEC2(v, wdVec2T(1, 2), 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== | operator!=")
  {
    wdVec2T v1(1, 2);
    wdVec2T v2 = v1;

    WD_TEST_BOOL(v1 == v2);

    v2.x += wdVec2T::ComponentType(0.001f);
    WD_TEST_BOOL(v1 != v2);
  }
}
