#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Vec4.h>
#include <Foundation/SimdMath/SimdVec4f.h>

namespace
{
  static bool AllCompSame(const wdSimdFloat& a)
  {
    // Make sure all components are the same
    wdSimdVec4f test;
    test.m_v = a.m_v;
    return test.x() == test.y() && test.x() == test.z() && test.x() == test.w();
  }

  template <wdMathAcc::Enum acc>
  static void TestLength(const wdSimdVec4f& a, float r[4], const wdSimdFloat& fEps)
  {
    wdSimdFloat l1 = a.GetLength<1, acc>();
    wdSimdFloat l2 = a.GetLength<2, acc>();
    wdSimdFloat l3 = a.GetLength<3, acc>();
    wdSimdFloat l4 = a.GetLength<4, acc>();
    WD_TEST_FLOAT(l1, r[0], fEps);
    WD_TEST_FLOAT(l2, r[1], fEps);
    WD_TEST_FLOAT(l3, r[2], fEps);
    WD_TEST_FLOAT(l4, r[3], fEps);
    WD_TEST_BOOL(AllCompSame(l1));
    WD_TEST_BOOL(AllCompSame(l2));
    WD_TEST_BOOL(AllCompSame(l3));
    WD_TEST_BOOL(AllCompSame(l4));
  }

  template <wdMathAcc::Enum acc>
  static void TestInvLength(const wdSimdVec4f& a, float r[4], const wdSimdFloat& fEps)
  {
    wdSimdFloat l1 = a.GetInvLength<1, acc>();
    wdSimdFloat l2 = a.GetInvLength<2, acc>();
    wdSimdFloat l3 = a.GetInvLength<3, acc>();
    wdSimdFloat l4 = a.GetInvLength<4, acc>();
    WD_TEST_FLOAT(l1, r[0], fEps);
    WD_TEST_FLOAT(l2, r[1], fEps);
    WD_TEST_FLOAT(l3, r[2], fEps);
    WD_TEST_FLOAT(l4, r[3], fEps);
    WD_TEST_BOOL(AllCompSame(l1));
    WD_TEST_BOOL(AllCompSame(l2));
    WD_TEST_BOOL(AllCompSame(l3));
    WD_TEST_BOOL(AllCompSame(l4));
  }

  template <wdMathAcc::Enum acc>
  static void TestNormalize(const wdSimdVec4f& a, wdSimdVec4f n[4], wdSimdFloat r[4], const wdSimdFloat& fEps)
  {
    wdSimdVec4f n1 = a.GetNormalized<1, acc>();
    wdSimdVec4f n2 = a.GetNormalized<2, acc>();
    wdSimdVec4f n3 = a.GetNormalized<3, acc>();
    wdSimdVec4f n4 = a.GetNormalized<4, acc>();
    WD_TEST_BOOL(n1.IsEqual(n[0], fEps).AllSet());
    WD_TEST_BOOL(n2.IsEqual(n[1], fEps).AllSet());
    WD_TEST_BOOL(n3.IsEqual(n[2], fEps).AllSet());
    WD_TEST_BOOL(n4.IsEqual(n[3], fEps).AllSet());

    wdSimdVec4f a1 = a;
    wdSimdVec4f a2 = a;
    wdSimdVec4f a3 = a;
    wdSimdVec4f a4 = a;

    wdSimdFloat l1 = a1.GetLengthAndNormalize<1, acc>();
    wdSimdFloat l2 = a2.GetLengthAndNormalize<2, acc>();
    wdSimdFloat l3 = a3.GetLengthAndNormalize<3, acc>();
    wdSimdFloat l4 = a4.GetLengthAndNormalize<4, acc>();
    WD_TEST_FLOAT(l1, r[0], fEps);
    WD_TEST_FLOAT(l2, r[1], fEps);
    WD_TEST_FLOAT(l3, r[2], fEps);
    WD_TEST_FLOAT(l4, r[3], fEps);
    WD_TEST_BOOL(AllCompSame(l1));
    WD_TEST_BOOL(AllCompSame(l2));
    WD_TEST_BOOL(AllCompSame(l3));
    WD_TEST_BOOL(AllCompSame(l4));

    WD_TEST_BOOL(a1.IsEqual(n[0], fEps).AllSet());
    WD_TEST_BOOL(a2.IsEqual(n[1], fEps).AllSet());
    WD_TEST_BOOL(a3.IsEqual(n[2], fEps).AllSet());
    WD_TEST_BOOL(a4.IsEqual(n[3], fEps).AllSet());

    WD_TEST_BOOL(a1.IsNormalized<1>(fEps));
    WD_TEST_BOOL(a2.IsNormalized<2>(fEps));
    WD_TEST_BOOL(a3.IsNormalized<3>(fEps));
    WD_TEST_BOOL(a4.IsNormalized<4>(fEps));
    WD_TEST_BOOL(!a1.IsNormalized<2>(fEps));
    WD_TEST_BOOL(!a2.IsNormalized<3>(fEps));
    WD_TEST_BOOL(!a3.IsNormalized<4>(fEps));

    a1 = a;
    a1.Normalize<1, acc>();
    a2 = a;
    a2.Normalize<2, acc>();
    a3 = a;
    a3.Normalize<3, acc>();
    a4 = a;
    a4.Normalize<4, acc>();
    WD_TEST_BOOL(a1.IsEqual(n[0], fEps).AllSet());
    WD_TEST_BOOL(a2.IsEqual(n[1], fEps).AllSet());
    WD_TEST_BOOL(a3.IsEqual(n[2], fEps).AllSet());
    WD_TEST_BOOL(a4.IsEqual(n[3], fEps).AllSet());
  }

  template <wdMathAcc::Enum acc>
  static void TestNormalizeIfNotZero(const wdSimdVec4f& a, wdSimdVec4f n[4], const wdSimdFloat& fEps)
  {
    wdSimdVec4f a1 = a;
    a1.NormalizeIfNotZero<1>(fEps);
    wdSimdVec4f a2 = a;
    a2.NormalizeIfNotZero<2>(fEps);
    wdSimdVec4f a3 = a;
    a3.NormalizeIfNotZero<3>(fEps);
    wdSimdVec4f a4 = a;
    a4.NormalizeIfNotZero<4>(fEps);
    WD_TEST_BOOL(a1.IsEqual(n[0], fEps).AllSet());
    WD_TEST_BOOL(a2.IsEqual(n[1], fEps).AllSet());
    WD_TEST_BOOL(a3.IsEqual(n[2], fEps).AllSet());
    WD_TEST_BOOL(a4.IsEqual(n[3], fEps).AllSet());

    WD_TEST_BOOL(a1.IsNormalized<1>(fEps));
    WD_TEST_BOOL(a2.IsNormalized<2>(fEps));
    WD_TEST_BOOL(a3.IsNormalized<3>(fEps));
    WD_TEST_BOOL(a4.IsNormalized<4>(fEps));
    WD_TEST_BOOL(!a1.IsNormalized<2>(fEps));
    WD_TEST_BOOL(!a2.IsNormalized<3>(fEps));
    WD_TEST_BOOL(!a3.IsNormalized<4>(fEps));

    wdSimdVec4f b(fEps);
    b.NormalizeIfNotZero<4>(fEps);
    WD_TEST_BOOL(b.IsZero<4>());
  }
} // namespace

WD_CREATE_SIMPLE_TEST(SimdMath, SimdVec4f)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    // In debug the default constructor initializes everything with NaN.
    wdSimdVec4f vDefCtor;
    WD_TEST_BOOL(vDefCtor.IsNaN<4>());
#else
// GCC assumes that the contents of the memory prior to the placement constructor doesn't matter
// So it optimizes away the initialization.
#  if WD_DISABLED(WD_COMPILER_GCC)
    // Placement new of the default constructor should not have any effect on the previous data.
    alignas(16) float testBlock[4] = {1, 2, 3, 4};
    wdSimdVec4f* pDefCtor = ::new ((void*)&testBlock[0]) wdSimdVec4f;
    WD_TEST_BOOL(pDefCtor->x() == 1.0f && pDefCtor->y() == 2.0f && pDefCtor->z() == 3.0f && pDefCtor->w() == 4.0f);
#  endif
#endif

    // Make sure the class didn't accidentally change in size.
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE
    WD_CHECK_AT_COMPILETIME(sizeof(wdSimdVec4f) == 16);
    WD_CHECK_AT_COMPILETIME(WD_ALIGNMENT_OF(wdSimdVec4f) == 16);
#endif

    wdSimdVec4f vInit1F(2.0f);
    WD_TEST_BOOL(vInit1F.x() == 2.0f && vInit1F.y() == 2.0f && vInit1F.z() == 2.0f && vInit1F.w() == 2.0f);

    wdSimdFloat a(3.0f);
    wdSimdVec4f vInit1SF(a);
    WD_TEST_BOOL(vInit1SF.x() == 3.0f && vInit1SF.y() == 3.0f && vInit1SF.z() == 3.0f && vInit1SF.w() == 3.0f);

    wdSimdVec4f vInit4F(1.0f, 2.0f, 3.0f, 4.0f);
    WD_TEST_BOOL(vInit4F.x() == 1.0f && vInit4F.y() == 2.0f && vInit4F.z() == 3.0f && vInit4F.w() == 4.0f);

    // Make sure all components have the correct values
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(
      vInit4F.m_v.m128_f32[0] == 1.0f && vInit4F.m_v.m128_f32[1] == 2.0f && vInit4F.m_v.m128_f32[2] == 3.0f && vInit4F.m_v.m128_f32[3] == 4.0f);
#endif

    wdSimdVec4f vCopy(vInit4F);
    WD_TEST_BOOL(vCopy.x() == 1.0f && vCopy.y() == 2.0f && vCopy.z() == 3.0f && vCopy.w() == 4.0f);

    wdSimdVec4f vZero = wdSimdVec4f::ZeroVector();
    WD_TEST_BOOL(vZero.x() == 0.0f && vZero.y() == 0.0f && vZero.z() == 0.0f && vZero.w() == 0.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Setter")
  {
    wdSimdVec4f a;
    a.Set(2.0f);
    WD_TEST_BOOL(a.x() == 2.0f && a.y() == 2.0f && a.z() == 2.0f && a.w() == 2.0f);

    wdSimdVec4f b;
    b.Set(1.0f, 2.0f, 3.0f, 4.0f);
    WD_TEST_BOOL(b.x() == 1.0f && b.y() == 2.0f && b.z() == 3.0f && b.w() == 4.0f);

    b.SetX(5.0f);
    WD_TEST_BOOL(b.x() == 5.0f && b.y() == 2.0f && b.z() == 3.0f && b.w() == 4.0f);

    b.SetY(6.0f);
    WD_TEST_BOOL(b.x() == 5.0f && b.y() == 6.0f && b.z() == 3.0f && b.w() == 4.0f);

    b.SetZ(7.0f);
    WD_TEST_BOOL(b.x() == 5.0f && b.y() == 6.0f && b.z() == 7.0f && b.w() == 4.0f);

    b.SetW(8.0f);
    WD_TEST_BOOL(b.x() == 5.0f && b.y() == 6.0f && b.z() == 7.0f && b.w() == 8.0f);

    wdSimdVec4f c;
    c.SetZero();
    WD_TEST_BOOL(c.x() == 0.0f && c.y() == 0.0f && c.z() == 0.0f && c.w() == 0.0f);

    {
      float testBlock[4] = {1, 2, 3, 4};
      wdSimdVec4f x;
      x.Load<1>(testBlock);
      WD_TEST_BOOL(x.x() == 1.0f && x.y() == 0.0f && x.z() == 0.0f && x.w() == 0.0f);

      wdSimdVec4f xy;
      xy.Load<2>(testBlock);
      WD_TEST_BOOL(xy.x() == 1.0f && xy.y() == 2.0f && xy.z() == 0.0f && xy.w() == 0.0f);

      wdSimdVec4f xyz;
      xyz.Load<3>(testBlock);
      WD_TEST_BOOL(xyz.x() == 1.0f && xyz.y() == 2.0f && xyz.z() == 3.0f && xyz.w() == 0.0f);

      wdSimdVec4f xyzw;
      xyzw.Load<4>(testBlock);
      WD_TEST_BOOL(xyzw.x() == 1.0f && xyzw.y() == 2.0f && xyzw.z() == 3.0f && xyzw.w() == 4.0f);

      WD_TEST_BOOL(xyzw.GetComponent(0) == 1.0f);
      WD_TEST_BOOL(xyzw.GetComponent(1) == 2.0f);
      WD_TEST_BOOL(xyzw.GetComponent(2) == 3.0f);
      WD_TEST_BOOL(xyzw.GetComponent(3) == 4.0f);
      WD_TEST_BOOL(xyzw.GetComponent(4) == 4.0f);

      // Make sure all components have the correct values
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
      WD_TEST_BOOL(xyzw.m_v.m128_f32[0] == 1.0f && xyzw.m_v.m128_f32[1] == 2.0f && xyzw.m_v.m128_f32[2] == 3.0f && xyzw.m_v.m128_f32[3] == 4.0f);
#endif
    }

    {
      float testBlock[4] = {7, 7, 7, 7};
      float mem[4] = {};

      wdSimdVec4f b2(1, 2, 3, 4);

      memcpy(mem, testBlock, 16);
      b2.Store<1>(mem);
      WD_TEST_BOOL(mem[0] == 1.0f && mem[1] == 7.0f && mem[2] == 7.0f && mem[3] == 7.0f);

      memcpy(mem, testBlock, 16);
      b2.Store<2>(mem);
      WD_TEST_BOOL(mem[0] == 1.0f && mem[1] == 2.0f && mem[2] == 7.0f && mem[3] == 7.0f);

      memcpy(mem, testBlock, 16);
      b2.Store<3>(mem);
      WD_TEST_BOOL(mem[0] == 1.0f && mem[1] == 2.0f && mem[2] == 3.0f && mem[3] == 7.0f);

      memcpy(mem, testBlock, 16);
      b2.Store<4>(mem);
      WD_TEST_BOOL(mem[0] == 1.0f && mem[1] == 2.0f && mem[2] == 3.0f && mem[3] == 4.0f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Functions")
  {
    {
      wdSimdVec4f a(1.0f, 2.0f, 4.0f, 8.0f);
      wdSimdVec4f b(1.0f, 0.5f, 0.25f, 0.125f);

      WD_TEST_BOOL(a.GetReciprocal().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetReciprocal<wdMathAcc::FULL>().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetReciprocal<wdMathAcc::BITS_23>().IsEqual(b, wdMath::DefaultEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetReciprocal<wdMathAcc::BITS_12>().IsEqual(b, wdMath::HugeEpsilon<float>()).AllSet());
    }

    {
      wdSimdVec4f a(1.0f, 2.0f, 4.0f, 8.0f);
      wdSimdVec4f b(1.0f, wdMath::Sqrt(2.0f), wdMath::Sqrt(4.0f), wdMath::Sqrt(8.0f));

      WD_TEST_BOOL(a.GetSqrt().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetSqrt<wdMathAcc::FULL>().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetSqrt<wdMathAcc::BITS_23>().IsEqual(b, wdMath::DefaultEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetSqrt<wdMathAcc::BITS_12>().IsEqual(b, wdMath::HugeEpsilon<float>()).AllSet());
    }

    {
      wdSimdVec4f a(1.0f, 2.0f, 4.0f, 8.0f);
      wdSimdVec4f b(1.0f, 1.0f / wdMath::Sqrt(2.0f), 1.0f / wdMath::Sqrt(4.0f), 1.0f / wdMath::Sqrt(8.0f));

      WD_TEST_BOOL(a.GetInvSqrt().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetInvSqrt<wdMathAcc::FULL>().IsEqual(b, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetInvSqrt<wdMathAcc::BITS_23>().IsEqual(b, wdMath::DefaultEpsilon<float>()).AllSet());
      WD_TEST_BOOL(a.GetInvSqrt<wdMathAcc::BITS_12>().IsEqual(b, wdMath::HugeEpsilon<float>()).AllSet());
    }

    {
      wdSimdVec4f a(2.0f, -2.0f, 4.0f, -8.0f);
      float r[4];
      r[0] = 2.0f;
      r[1] = wdVec2(a.x(), a.y()).GetLength();
      r[2] = wdVec3(a.x(), a.y(), a.z()).GetLength();
      r[3] = wdVec4(a.x(), a.y(), a.z(), a.w()).GetLength();

      WD_TEST_FLOAT(a.GetLength<1>(), r[0], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLength<2>(), r[1], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLength<3>(), r[2], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLength<4>(), r[3], wdMath::SmallEpsilon<float>());

      TestLength<wdMathAcc::FULL>(a, r, wdMath::SmallEpsilon<float>());
      TestLength<wdMathAcc::BITS_23>(a, r, wdMath::DefaultEpsilon<float>());
      TestLength<wdMathAcc::BITS_12>(a, r, 0.01f);
    }

    {
      wdSimdVec4f a(2.0f, -2.0f, 4.0f, -8.0f);
      float r[4];
      r[0] = 0.5f;
      r[1] = 1.0f / wdVec2(a.x(), a.y()).GetLength();
      r[2] = 1.0f / wdVec3(a.x(), a.y(), a.z()).GetLength();
      r[3] = 1.0f / wdVec4(a.x(), a.y(), a.z(), a.w()).GetLength();

      WD_TEST_FLOAT(a.GetInvLength<1>(), r[0], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetInvLength<2>(), r[1], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetInvLength<3>(), r[2], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetInvLength<4>(), r[3], wdMath::SmallEpsilon<float>());

      TestInvLength<wdMathAcc::FULL>(a, r, wdMath::SmallEpsilon<float>());
      TestInvLength<wdMathAcc::BITS_23>(a, r, wdMath::DefaultEpsilon<float>());
      TestInvLength<wdMathAcc::BITS_12>(a, r, wdMath::HugeEpsilon<float>());
    }

    {
      wdSimdVec4f a(2.0f, -2.0f, 4.0f, -8.0f);
      float r[4];
      r[0] = 2.0f * 2.0f;
      r[1] = wdVec2(a.x(), a.y()).GetLengthSquared();
      r[2] = wdVec3(a.x(), a.y(), a.z()).GetLengthSquared();
      r[3] = wdVec4(a.x(), a.y(), a.z(), a.w()).GetLengthSquared();

      WD_TEST_FLOAT(a.GetLengthSquared<1>(), r[0], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLengthSquared<2>(), r[1], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLengthSquared<3>(), r[2], wdMath::SmallEpsilon<float>());
      WD_TEST_FLOAT(a.GetLengthSquared<4>(), r[3], wdMath::SmallEpsilon<float>());
    }

    {
      wdSimdVec4f a(2.0f, -2.0f, 4.0f, -8.0f);
      wdSimdFloat r[4];
      r[0] = 2.0f;
      r[1] = wdVec2(a.x(), a.y()).GetLength();
      r[2] = wdVec3(a.x(), a.y(), a.z()).GetLength();
      r[3] = wdVec4(a.x(), a.y(), a.z(), a.w()).GetLength();

      wdSimdVec4f n[4];
      n[0] = a / r[0];
      n[1] = a / r[1];
      n[2] = a / r[2];
      n[3] = a / r[3];

      TestNormalize<wdMathAcc::FULL>(a, n, r, wdMath::SmallEpsilon<float>());
      TestNormalize<wdMathAcc::BITS_23>(a, n, r, wdMath::DefaultEpsilon<float>());
      TestNormalize<wdMathAcc::BITS_12>(a, n, r, 0.01f);
    }

    {
      wdSimdVec4f a(2.0f, -2.0f, 4.0f, -8.0f);
      wdSimdVec4f n[4];
      n[0] = a / 2.0f;
      n[1] = a / wdVec2(a.x(), a.y()).GetLength();
      n[2] = a / wdVec3(a.x(), a.y(), a.z()).GetLength();
      n[3] = a / wdVec4(a.x(), a.y(), a.z(), a.w()).GetLength();

      TestNormalizeIfNotZero<wdMathAcc::FULL>(a, n, wdMath::SmallEpsilon<float>());
      TestNormalizeIfNotZero<wdMathAcc::BITS_23>(a, n, wdMath::DefaultEpsilon<float>());
      TestNormalizeIfNotZero<wdMathAcc::BITS_12>(a, n, wdMath::HugeEpsilon<float>());
    }

    {
      wdSimdVec4f a;

      a.Set(0.0f, 2.0f, 0.0f, 0.0f);
      WD_TEST_BOOL(a.IsZero<1>());
      WD_TEST_BOOL(!a.IsZero<2>());

      a.Set(0.0f, 0.0f, 3.0f, 0.0f);
      WD_TEST_BOOL(a.IsZero<2>());
      WD_TEST_BOOL(!a.IsZero<3>());

      a.Set(0.0f, 0.0f, 0.0f, 4.0f);
      WD_TEST_BOOL(a.IsZero<3>());
      WD_TEST_BOOL(!a.IsZero<4>());

      float smallEps = wdMath::SmallEpsilon<float>();
      a.Set(smallEps, 2.0f, smallEps, smallEps);
      WD_TEST_BOOL(a.IsZero<1>(wdMath::DefaultEpsilon<float>()));
      WD_TEST_BOOL(!a.IsZero<2>(wdMath::DefaultEpsilon<float>()));

      a.Set(smallEps, smallEps, 3.0f, smallEps);
      WD_TEST_BOOL(a.IsZero<2>(wdMath::DefaultEpsilon<float>()));
      WD_TEST_BOOL(!a.IsZero<3>(wdMath::DefaultEpsilon<float>()));

      a.Set(smallEps, smallEps, smallEps, 4.0f);
      WD_TEST_BOOL(a.IsZero<3>(wdMath::DefaultEpsilon<float>()));
      WD_TEST_BOOL(!a.IsZero<4>(wdMath::DefaultEpsilon<float>()));
    }

    {
      wdSimdVec4f a;

      float NaN = wdMath::NaN<float>();
      float Inf = wdMath::Infinity<float>();

      a.Set(NaN, 1.0f, NaN, NaN);
      WD_TEST_BOOL(a.IsNaN<1>());
      WD_TEST_BOOL(a.IsNaN<2>());
      WD_TEST_BOOL(!a.IsValid<2>());

      a.Set(Inf, 1.0f, NaN, NaN);
      WD_TEST_BOOL(!a.IsNaN<1>());
      WD_TEST_BOOL(!a.IsNaN<2>());
      WD_TEST_BOOL(!a.IsValid<2>());

      a.Set(1.0f, 2.0f, Inf, NaN);
      WD_TEST_BOOL(a.IsNaN<4>());
      WD_TEST_BOOL(!a.IsNaN<3>());
      WD_TEST_BOOL(a.IsValid<2>());
      WD_TEST_BOOL(!a.IsValid<3>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swizzle")
  {
    wdSimdVec4f a(3.0f, 5.0f, 7.0f, 9.0f);

    wdSimdVec4f b = a.Get<wdSwizzle::XXXX>();
    WD_TEST_BOOL(b.x() == 3.0f && b.y() == 3.0f && b.z() == 3.0f && b.w() == 3.0f);

    b = a.Get<wdSwizzle::YYYX>();
    WD_TEST_BOOL(b.x() == 5.0f && b.y() == 5.0f && b.z() == 5.0f && b.w() == 3.0f);

    b = a.Get<wdSwizzle::ZZZX>();
    WD_TEST_BOOL(b.x() == 7.0f && b.y() == 7.0f && b.z() == 7.0f && b.w() == 3.0f);

    b = a.Get<wdSwizzle::WWWX>();
    WD_TEST_BOOL(b.x() == 9.0f && b.y() == 9.0f && b.z() == 9.0f && b.w() == 3.0f);

    b = a.Get<wdSwizzle::WZYX>();
    WD_TEST_BOOL(b.x() == 9.0f && b.y() == 7.0f && b.z() == 5.0f && b.w() == 3.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);

      wdSimdVec4f b = -a;
      WD_TEST_BOOL(b.x() == 3.0f && b.y() == -5.0f && b.z() == 7.0f && b.w() == -9.0f);

      b.Set(8.0f, 6.0f, 4.0f, 2.0f);
      wdSimdVec4f c;
      c = a + b;
      WD_TEST_BOOL(c.x() == 5.0f && c.y() == 11.0f && c.z() == -3.0f && c.w() == 11.0f);

      c = a - b;
      WD_TEST_BOOL(c.x() == -11.0f && c.y() == -1.0f && c.z() == -11.0f && c.w() == 7.0f);

      c = a * wdSimdFloat(3.0f);
      WD_TEST_BOOL(c.x() == -9.0f && c.y() == 15.0f && c.z() == -21.0f && c.w() == 27.0f);

      c = a / wdSimdFloat(2.0f);
      WD_TEST_BOOL(c.x() == -1.5f && c.y() == 2.5f && c.z() == -3.5f && c.w() == 4.5f);

      c = a.CompMul(b);
      WD_TEST_BOOL(c.x() == -24.0f && c.y() == 30.0f && c.z() == -28.0f && c.w() == 18.0f);

      wdSimdVec4f divRes(-0.375f, 5.0f / 6.0f, -1.75f, 4.5f);
      wdSimdVec4f d1 = a.CompDiv(b);
      wdSimdVec4f d2 = a.CompDiv<wdMathAcc::FULL>(b);
      wdSimdVec4f d3 = a.CompDiv<wdMathAcc::BITS_23>(b);
      wdSimdVec4f d4 = a.CompDiv<wdMathAcc::BITS_12>(b);

      WD_TEST_BOOL(d1.IsEqual(divRes, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(d2.IsEqual(divRes, wdMath::SmallEpsilon<float>()).AllSet());
      WD_TEST_BOOL(d3.IsEqual(divRes, wdMath::DefaultEpsilon<float>()).AllSet());
      WD_TEST_BOOL(d4.IsEqual(divRes, 0.01f).AllSet());
    }

    {
      wdSimdVec4f a(-3.4f, 5.4f, -7.6f, 9.6f);
      wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);
      wdSimdVec4f c;

      c = a.CompMin(b);
      WD_TEST_BOOL(c.x() == -3.4f && c.y() == 5.4f && c.z() == -7.6f && c.w() == 2.0f);

      c = a.CompMax(b);
      WD_TEST_BOOL(c.x() == 8.0f && c.y() == 6.0f && c.z() == 4.0f && c.w() == 9.6f);

      c = a.Abs();
      WD_TEST_BOOL(c.x() == 3.4f && c.y() == 5.4f && c.z() == 7.6f && c.w() == 9.6f);

      c = a.Round();
      WD_TEST_BOOL(c.x() == -3.0f && c.y() == 5.0f && c.z() == -8.0f && c.w() == 10.0f);

      c = a.Floor();
      WD_TEST_BOOL(c.x() == -4.0f && c.y() == 5.0f && c.z() == -8.0f && c.w() == 9.0f);

      c = a.Ceil();
      WD_TEST_BOOL(c.x() == -3.0f && c.y() == 6.0f && c.z() == -7.0f && c.w() == 10.0f);

      c = a.Trunc();
      WD_TEST_BOOL(c.x() == -3.0f && c.y() == 5.0f && c.z() == -7.0f && c.w() == 9.0f);

      c = a.Fraction();
      WD_TEST_BOOL(c.IsEqual(wdSimdVec4f(-0.4f, 0.4f, -0.6f, 0.6f), wdMath::SmallEpsilon<float>()).AllSet());
    }

    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);
      wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);

      wdSimdVec4b cmp(true, false, false, true);
      wdSimdVec4f c;

      c = a.FlipSign(cmp);
      WD_TEST_BOOL(c.x() == 3.0f && c.y() == 5.0f && c.z() == -7.0f && c.w() == -9.0f);

      c = wdSimdVec4f::Select(cmp, b, a);
      WD_TEST_BOOL(c.x() == 8.0f && c.y() == 5.0f && c.z() == -7.0f && c.w() == 2.0f);

      c = wdSimdVec4f::Select(cmp, a, b);
      WD_TEST_BOOL(c.x() == -3.0f && c.y() == 6.0f && c.z() == 4.0f && c.w() == 9.0f);
    }

    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);
      wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);

      wdSimdVec4f c = a;
      c += b;
      WD_TEST_BOOL(c.x() == 5.0f && c.y() == 11.0f && c.z() == -3.0f && c.w() == 11.0f);

      c = a;
      c -= b;
      WD_TEST_BOOL(c.x() == -11.0f && c.y() == -1.0f && c.z() == -11.0f && c.w() == 7.0f);

      c = a;
      c *= wdSimdFloat(3.0f);
      WD_TEST_BOOL(c.x() == -9.0f && c.y() == 15.0f && c.z() == -21.0f && c.w() == 27.0f);

      c = a;
      c /= wdSimdFloat(2.0f);
      WD_TEST_BOOL(c.x() == -1.5f && c.y() == 2.5f && c.z() == -3.5f && c.w() == 4.5f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdVec4f a(7.0f, 5.0f, 4.0f, 3.0f);
    wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);
    wdSimdVec4b cmp;

    cmp = a == b;
    WD_TEST_BOOL(!cmp.x() && !cmp.y() && cmp.z() && !cmp.w());

    cmp = a != b;
    WD_TEST_BOOL(cmp.x() && cmp.y() && !cmp.z() && cmp.w());

    cmp = a <= b;
    WD_TEST_BOOL(cmp.x() && cmp.y() && cmp.z() && !cmp.w());

    cmp = a < b;
    WD_TEST_BOOL(cmp.x() && cmp.y() && !cmp.z() && !cmp.w());

    cmp = a >= b;
    WD_TEST_BOOL(!cmp.x() && !cmp.y() && cmp.z() && cmp.w());

    cmp = a > b;
    WD_TEST_BOOL(!cmp.x() && !cmp.y() && !cmp.z() && cmp.w());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Advanced Operators")
  {
    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);

      WD_TEST_FLOAT(a.HorizontalSum<1>(), -3.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalSum<2>(), 2.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalSum<3>(), -5.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalSum<4>(), 4.0f, 0.0f);
      WD_TEST_BOOL(AllCompSame(a.HorizontalSum<1>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalSum<2>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalSum<3>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalSum<4>()));

      WD_TEST_FLOAT(a.HorizontalMin<1>(), -3.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMin<2>(), -3.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMin<3>(), -7.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMin<4>(), -7.0f, 0.0f);
      WD_TEST_BOOL(AllCompSame(a.HorizontalMin<1>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMin<2>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMin<3>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMin<4>()));

      WD_TEST_FLOAT(a.HorizontalMax<1>(), -3.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMax<2>(), 5.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMax<3>(), 5.0f, 0.0f);
      WD_TEST_FLOAT(a.HorizontalMax<4>(), 9.0f, 0.0f);
      WD_TEST_BOOL(AllCompSame(a.HorizontalMax<1>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMax<2>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMax<3>()));
      WD_TEST_BOOL(AllCompSame(a.HorizontalMax<4>()));
    }

    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);
      wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);

      WD_TEST_FLOAT(a.Dot<1>(b), -24.0f, 0.0f);
      WD_TEST_FLOAT(a.Dot<2>(b), 6.0f, 0.0f);
      WD_TEST_FLOAT(a.Dot<3>(b), -22.0f, 0.0f);
      WD_TEST_FLOAT(a.Dot<4>(b), -4.0f, 0.0f);
      WD_TEST_BOOL(AllCompSame(a.Dot<1>(b)));
      WD_TEST_BOOL(AllCompSame(a.Dot<2>(b)));
      WD_TEST_BOOL(AllCompSame(a.Dot<3>(b)));
      WD_TEST_BOOL(AllCompSame(a.Dot<4>(b)));
    }

    {
      wdSimdVec4f a(1.0f, 2.0f, 3.0f, 0.0f);
      wdSimdVec4f b(2.0f, -4.0f, 6.0f, 8.0f);

      wdVec3 res = wdVec3(a.x(), a.y(), a.z()).CrossRH(wdVec3(b.x(), b.y(), b.z()));

      wdSimdVec4f c = a.CrossRH(b);
      WD_TEST_BOOL(c.x() == res.x);
      WD_TEST_BOOL(c.y() == res.y);
      WD_TEST_BOOL(c.z() == res.z);
    }

    {
      wdSimdVec4f a(1.0f, 2.0f, 3.0f, 0.0f);
      wdSimdVec4f b(2.0f, -4.0f, 6.0f, 0.0f);

      wdVec3 res = wdVec3(a.x(), a.y(), a.z()).CrossRH(wdVec3(b.x(), b.y(), b.z()));

      wdSimdVec4f c = a.CrossRH(b);
      WD_TEST_BOOL(c.x() == res.x);
      WD_TEST_BOOL(c.y() == res.y);
      WD_TEST_BOOL(c.z() == res.z);
    }

    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 0.0f);
      wdSimdVec4f b = a.GetOrthogonalVector();

      WD_TEST_BOOL(!b.IsZero<3>());
      WD_TEST_FLOAT(a.Dot<3>(b), 0.0f, 0.0f);
    }

    {
      wdSimdVec4f a(-3.0f, 5.0f, -7.0f, 9.0f);
      wdSimdVec4f b(8.0f, 6.0f, 4.0f, 2.0f);
      wdSimdVec4f c(1.0f, 2.0f, 3.0f, 4.0f);
      wdSimdVec4f d;

      d = wdSimdVec4f::MulAdd(a, b, c);
      WD_TEST_BOOL(d.x() == -23.0f && d.y() == 32.0f && d.z() == -25.0f && d.w() == 22.0f);

      d = wdSimdVec4f::MulAdd(a, wdSimdFloat(3.0f), c);
      WD_TEST_BOOL(d.x() == -8.0f && d.y() == 17.0f && d.z() == -18.0f && d.w() == 31.0f);

      d = wdSimdVec4f::MulSub(a, b, c);
      WD_TEST_BOOL(d.x() == -25.0f && d.y() == 28.0f && d.z() == -31.0f && d.w() == 14.0f);

      d = wdSimdVec4f::MulSub(a, wdSimdFloat(3.0f), c);
      WD_TEST_BOOL(d.x() == -10.0f && d.y() == 13.0f && d.z() == -24.0f && d.w() == 23.0f);

      d = wdSimdVec4f::CopySign(b, a);
      WD_TEST_BOOL(d.x() == -8.0f && d.y() == 6.0f && d.z() == -4.0f && d.w() == 2.0f);
    }
  }
}
