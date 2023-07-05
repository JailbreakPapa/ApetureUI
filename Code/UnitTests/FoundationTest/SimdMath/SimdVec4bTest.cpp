#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdVec4b.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdVec4b)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_DISABLED(WD_COMPILER_GCC) && WD_DISABLED(WD_COMPILE_FOR_DEBUG)
    // Placement new of the default constructor should not have any effect on the previous data.
    alignas(16) float testBlock[4] = {1, 2, 3, 4};
    wdSimdVec4b* pDefCtor = ::new ((void*)&testBlock[0]) wdSimdVec4b;
    WD_TEST_BOOL(testBlock[0] == 1.0f && testBlock[1] == 2.0f && testBlock[2] == 3.0f && testBlock[3] == 4.0f);
#endif

    // Make sure the class didn't accidentally change in size.
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE
    WD_CHECK_AT_COMPILETIME(sizeof(wdSimdVec4b) == 16);
    WD_CHECK_AT_COMPILETIME(WD_ALIGNMENT_OF(wdSimdVec4b) == 16);
#endif

    wdSimdVec4b vInit1B(true);
    WD_TEST_BOOL(vInit1B.x() == true && vInit1B.y() == true && vInit1B.z() == true && vInit1B.w() == true);

    // Make sure all components have the correct value
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(vInit1B.m_v.m128_u32[0] == 0xFFFFFFFF && vInit1B.m_v.m128_u32[1] == 0xFFFFFFFF && vInit1B.m_v.m128_u32[2] == 0xFFFFFFFF &&
                 vInit1B.m_v.m128_u32[3] == 0xFFFFFFFF);
#endif

    wdSimdVec4b vInit4B(false, true, false, true);
    WD_TEST_BOOL(vInit4B.x() == false && vInit4B.y() == true && vInit4B.z() == false && vInit4B.w() == true);

    // Make sure all components have the correct value
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(
      vInit4B.m_v.m128_u32[0] == 0 && vInit4B.m_v.m128_u32[1] == 0xFFFFFFFF && vInit4B.m_v.m128_u32[2] == 0 && vInit4B.m_v.m128_u32[3] == 0xFFFFFFFF);
#endif

    wdSimdVec4b vCopy(vInit4B);
    WD_TEST_BOOL(vCopy.x() == false && vCopy.y() == true && vCopy.z() == false && vCopy.w() == true);

    WD_TEST_BOOL(
      vCopy.GetComponent<0>() == false && vCopy.GetComponent<1>() == true && vCopy.GetComponent<2>() == false && vCopy.GetComponent<3>() == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swizzle")
  {
    wdSimdVec4b a(true, false, true, false);

    wdSimdVec4b b = a.Get<wdSwizzle::XXXX>();
    WD_TEST_BOOL(b.x() && b.y() && b.z() && b.w());

    b = a.Get<wdSwizzle::YYYX>();
    WD_TEST_BOOL(!b.x() && !b.y() && !b.z() && b.w());

    b = a.Get<wdSwizzle::ZZZX>();
    WD_TEST_BOOL(b.x() && b.y() && b.z() && b.w());

    b = a.Get<wdSwizzle::WWWX>();
    WD_TEST_BOOL(!b.x() && !b.y() && !b.z() && b.w());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    wdSimdVec4b a(true, false, true, false);
    wdSimdVec4b b(false, true, true, false);

    wdSimdVec4b c = a && b;
    WD_TEST_BOOL(!c.x() && !c.y() && c.z() && !c.w());

    c = a || b;
    WD_TEST_BOOL(c.x() && c.y() && c.z() && !c.w());

    c = !a;
    WD_TEST_BOOL(!c.x() && c.y() && !c.z() && c.w());
    WD_TEST_BOOL(c.AnySet<2>());
    WD_TEST_BOOL(!c.AllSet<4>());
    WD_TEST_BOOL(!c.NoneSet<4>());

    c = c || a;
    WD_TEST_BOOL(c.AnySet<4>());
    WD_TEST_BOOL(c.AllSet<4>());
    WD_TEST_BOOL(!c.NoneSet<4>());

    c = !c;
    WD_TEST_BOOL(!c.AnySet<4>());
    WD_TEST_BOOL(!c.AllSet<4>());
    WD_TEST_BOOL(c.NoneSet<4>());

    c = a == b;
    WD_TEST_BOOL(!c.x() && !c.y() && c.z() && c.w());

    c = a != b;
    WD_TEST_BOOL(c.x() && c.y() && !c.z() && !c.w());

    WD_TEST_BOOL(a.AllSet<1>());
    WD_TEST_BOOL(b.NoneSet<1>());

    wdSimdVec4b cmp(false, true, false, true);
    c = wdSimdVec4b::Select(cmp, a, b);
    WD_TEST_BOOL(!c.x() && !c.y() && c.z() && !c.w());
  }
}
