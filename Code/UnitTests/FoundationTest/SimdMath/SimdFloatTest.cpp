#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdFloat.h>

WD_CREATE_SIMPLE_TEST_GROUP(SimdMath);

WD_CREATE_SIMPLE_TEST(SimdMath, SimdFloat)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    // In debug the default constructor initializes everything with NaN.
    wdSimdFloat vDefCtor;
    WD_TEST_BOOL(wdMath::IsNaN((float)vDefCtor));
#else
// GCC assumes that the contents of the memory before calling the default constructor are irrelevant.
// So it optimizes away the 1,2,3,4 initializer completely.
#  if WD_DISABLED(WD_COMPILER_GCC)
    // Placement new of the default constructor should not have any effect on the previous data.
    alignas(16) float testBlock[4] = {1, 2, 3, 4};
    wdSimdFloat* pDefCtor = ::new ((void*)&testBlock[0]) wdSimdFloat;
    WD_TEST_BOOL_MSG((float)(*pDefCtor) == 1.0f, "Default constructed value is %f", (float)(*pDefCtor));
#  endif
#endif

    // Make sure the class didn't accidentally change in size.
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE
    WD_CHECK_AT_COMPILETIME(sizeof(wdSimdFloat) == 16);
    WD_CHECK_AT_COMPILETIME(WD_ALIGNMENT_OF(wdSimdFloat) == 16);
#endif

    wdSimdFloat vInit1F(2.0f);
    WD_TEST_BOOL(vInit1F == 2.0f);

    // Make sure all components are set to the same value
#if (WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE) && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(
      vInit1F.m_v.m128_f32[0] == 2.0f && vInit1F.m_v.m128_f32[1] == 2.0f && vInit1F.m_v.m128_f32[2] == 2.0f && vInit1F.m_v.m128_f32[3] == 2.0f);
#endif

    wdSimdFloat vInit1I(1);
    WD_TEST_BOOL(vInit1I == 1.0f);

    // Make sure all components are set to the same value
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(
      vInit1I.m_v.m128_f32[0] == 1.0f && vInit1I.m_v.m128_f32[1] == 1.0f && vInit1I.m_v.m128_f32[2] == 1.0f && vInit1I.m_v.m128_f32[3] == 1.0f);
#endif

    wdSimdFloat vInit1U(4553u);
    WD_TEST_BOOL(vInit1U == 4553.0f);

    // Make sure all components are set to the same value
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(vInit1U.m_v.m128_f32[0] == 4553.0f && vInit1U.m_v.m128_f32[1] == 4553.0f && vInit1U.m_v.m128_f32[2] == 4553.0f &&
                 vInit1U.m_v.m128_f32[3] == 4553.0f);
#endif

    wdSimdFloat z = wdSimdFloat::Zero();
    WD_TEST_BOOL(z == 0.0f);

    // Make sure all components are set to the same value
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(z.m_v.m128_f32[0] == 0.0f && z.m_v.m128_f32[1] == 0.0f && z.m_v.m128_f32[2] == 0.0f && z.m_v.m128_f32[3] == 0.0f);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    wdSimdFloat a = 5.0f;
    wdSimdFloat b = 2.0f;

    WD_TEST_FLOAT(a + b, 7.0f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a - b, 3.0f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a * b, 10.0f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a / b, 2.5f, wdMath::SmallEpsilon<float>());

    wdSimdFloat c = 1.0f;
    c += a;
    WD_TEST_FLOAT(c, 6.0f, wdMath::SmallEpsilon<float>());

    c = 1.0f;
    c -= b;
    WD_TEST_FLOAT(c, -1.0f, wdMath::SmallEpsilon<float>());

    c = 1.0f;
    c *= a;
    WD_TEST_FLOAT(c, 5.0f, wdMath::SmallEpsilon<float>());

    c = 1.0f;
    c /= a;
    WD_TEST_FLOAT(c, 0.2f, wdMath::SmallEpsilon<float>());

    WD_TEST_BOOL(c.IsEqual(0.201f, wdMath::HugeEpsilon<float>()));
    WD_TEST_BOOL(c.IsEqual(0.199f, wdMath::HugeEpsilon<float>()));
    WD_TEST_BOOL(!c.IsEqual(0.202f, wdMath::HugeEpsilon<float>()));
    WD_TEST_BOOL(!c.IsEqual(0.198f, wdMath::HugeEpsilon<float>()));

    c = b;
    WD_TEST_BOOL(c == b);
    WD_TEST_BOOL(c != a);
    WD_TEST_BOOL(a > b);
    WD_TEST_BOOL(c >= b);
    WD_TEST_BOOL(b < a);
    WD_TEST_BOOL(b <= c);

    WD_TEST_BOOL(c == 2.0f);
    WD_TEST_BOOL(c != 5.0f);
    WD_TEST_BOOL(a > 2.0f);
    WD_TEST_BOOL(c >= 2.0f);
    WD_TEST_BOOL(b < 5.0f);
    WD_TEST_BOOL(b <= 2.0f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Misc")
  {
    wdSimdFloat a = 2.0f;

    WD_TEST_FLOAT(a.GetReciprocal(), 0.5f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetReciprocal<wdMathAcc::FULL>(), 0.5f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetReciprocal<wdMathAcc::BITS_23>(), 0.5f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(a.GetReciprocal<wdMathAcc::BITS_12>(), 0.5f, wdMath::HugeEpsilon<float>());

    WD_TEST_FLOAT(a.GetSqrt(), 1.41421356f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetSqrt<wdMathAcc::FULL>(), 1.41421356f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetSqrt<wdMathAcc::BITS_23>(), 1.41421356f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(a.GetSqrt<wdMathAcc::BITS_12>(), 1.41421356f, wdMath::HugeEpsilon<float>());

    WD_TEST_FLOAT(a.GetInvSqrt(), 0.70710678f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetInvSqrt<wdMathAcc::FULL>(), 0.70710678f, wdMath::SmallEpsilon<float>());
    WD_TEST_FLOAT(a.GetInvSqrt<wdMathAcc::BITS_23>(), 0.70710678f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(a.GetInvSqrt<wdMathAcc::BITS_12>(), 0.70710678f, wdMath::HugeEpsilon<float>());

    wdSimdFloat b = 5.0f;
    WD_TEST_BOOL(a.Max(b) == b);
    WD_TEST_BOOL(a.Min(b) == a);

    wdSimdFloat c = -4.0f;
    WD_TEST_FLOAT(c.Abs(), 4.0f, wdMath::SmallEpsilon<float>());
  }
}
