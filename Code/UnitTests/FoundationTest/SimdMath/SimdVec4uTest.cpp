#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdVec4u.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdVec4u)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    // In debug the default constructor initializes everything with 0xCDCDCDCD.
    wdSimdVec4u vDefCtor;
    WD_TEST_BOOL(vDefCtor.x() == 0xCDCDCDCD && vDefCtor.y() == 0xCDCDCDCD && vDefCtor.z() == 0xCDCDCDCD && vDefCtor.w() == 0xCDCDCDCD);
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    alignas(16) float testBlock[4] = {1, 2, 3, 4};
    wdSimdVec4u* pDefCtor = ::new ((void*)&testBlock[0]) wdSimdVec4u;
    WD_TEST_BOOL(testBlock[0] == 1 && testBlock[1] == 2 && testBlock[2] == 3 && testBlock[3] == 4);
#endif

    // Make sure the class didn't accidentally change in size.
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE
    WD_CHECK_AT_COMPILETIME(sizeof(wdSimdVec4u) == 16);
    WD_CHECK_AT_COMPILETIME(WD_ALIGNMENT_OF(wdSimdVec4u) == 16);
#endif

    wdSimdVec4u a(2);
    WD_TEST_BOOL(a.x() == 2 && a.y() == 2 && a.z() == 2 && a.w() == 2);

    wdSimdVec4u b(1, 2, 3, 0xFFFFFFFFu);
    WD_TEST_BOOL(b.x() == 1 && b.y() == 2 && b.z() == 3 && b.w() == 0xFFFFFFFFu);

    // Make sure all components have the correct values
#if WD_SIMD_IMPLEMENTATION == WD_SIMD_IMPLEMENTATION_SSE && WD_ENABLED(WD_COMPILER_MSVC)
    WD_TEST_BOOL(b.m_v.m128i_u32[0] == 1 && b.m_v.m128i_u32[1] == 2 && b.m_v.m128i_u32[2] == 3 && b.m_v.m128i_u32[3] == 0xFFFFFFFFu);
#endif

    wdSimdVec4u copy(b);
    WD_TEST_BOOL(copy.x() == 1 && copy.y() == 2 && copy.z() == 3 && copy.w() == 0xFFFFFFFFu);

    WD_TEST_BOOL(copy.GetComponent<0>() == 1 && copy.GetComponent<1>() == 2 && copy.GetComponent<2>() == 3 && copy.GetComponent<3>() == 0xFFFFFFFFu);

    wdSimdVec4u vZero = wdSimdVec4u::ZeroVector();
    WD_TEST_BOOL(vZero.x() == 0 && vZero.y() == 0 && vZero.z() == 0 && vZero.w() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Setter")
  {
    wdSimdVec4u a;
    a.Set(2);
    WD_TEST_BOOL(a.x() == 2 && a.y() == 2 && a.z() == 2 && a.w() == 2);

    wdSimdVec4u b;
    b.Set(1, 2, 3, 4);
    WD_TEST_BOOL(b.x() == 1 && b.y() == 2 && b.z() == 3 && b.w() == 4);

    wdSimdVec4u vSetZero;
    vSetZero.SetZero();
    WD_TEST_BOOL(vSetZero.x() == 0 && vSetZero.y() == 0 && vSetZero.z() == 0 && vSetZero.w() == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion")
  {
    wdSimdVec4u ua(-10000, 5, -7, 11);

    wdSimdVec4i ia(ua);
    WD_TEST_BOOL(ia.x() == -10000 && ia.y() == 5 && ia.z() == -7 && ia.w() == 11);

    wdSimdVec4f fa = ua.ToFloat();
    WD_TEST_BOOL(fa.x() == 4294957296.0f && fa.y() == 5.0f && fa.z() == 4294967289.0f && fa.w() == 11.0f);

    fa = wdSimdVec4f(-2.3f, 5.7f, -4294967040.0f, 4294967040.0f);
    wdSimdVec4u b = wdSimdVec4u::Truncate(fa);
    WD_TEST_INT(b.x(), 0);
    WD_TEST_INT(b.y(), 5);
    WD_TEST_INT(b.z(), 0);
    WD_TEST_INT(b.w(), 4294967040);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swizzle")
  {
    wdSimdVec4u a(3, 5, 7, 9);

    wdSimdVec4u b = a.Get<wdSwizzle::XXXX>();
    WD_TEST_BOOL(b.x() == 3 && b.y() == 3 && b.z() == 3 && b.w() == 3);

    b = a.Get<wdSwizzle::YYYX>();
    WD_TEST_BOOL(b.x() == 5 && b.y() == 5 && b.z() == 5 && b.w() == 3);

    b = a.Get<wdSwizzle::ZZZX>();
    WD_TEST_BOOL(b.x() == 7 && b.y() == 7 && b.z() == 7 && b.w() == 3);

    b = a.Get<wdSwizzle::WWWX>();
    WD_TEST_BOOL(b.x() == 9 && b.y() == 9 && b.z() == 9 && b.w() == 3);

    b = a.Get<wdSwizzle::WZYX>();
    WD_TEST_BOOL(b.x() == 9 && b.y() == 7 && b.z() == 5 && b.w() == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    {
      wdSimdVec4u a(-3, 5, -7, 9);
      wdSimdVec4u b(8, 6, 4, 2);
      wdSimdVec4u c;
      c = a + b;
      WD_TEST_BOOL(c.x() == 5 && c.y() == 11 && c.z() == -3 && c.w() == 11);

      c = a - b;
      WD_TEST_BOOL(c.x() == -11 && c.y() == -1 && c.z() == -11 && c.w() == 7);

      a.Set(0xFFFFFFFF);
      c = a.CompMul(b);
      WD_TEST_BOOL(c.x() == 4294967288u && c.y() == 4294967290u && c.z() == 4294967292u && c.w() == 4294967294u);
    }

    {
      wdSimdVec4u a(WD_BIT(1), WD_BIT(2), WD_BIT(3), WD_BIT(4));
      wdSimdVec4u b(WD_BIT(4), WD_BIT(3), WD_BIT(3), WD_BIT(5) - 1);
      wdSimdVec4u c;

      c = a | b;
      WD_TEST_BOOL(c.x() == (WD_BIT(1) | WD_BIT(4)) && c.y() == (WD_BIT(2) | WD_BIT(3)) && c.z() == WD_BIT(3) && c.w() == WD_BIT(5) - 1);

      c = a & b;
      WD_TEST_BOOL(c.x() == 0 && c.y() == 0 && c.z() == WD_BIT(3) && c.w() == WD_BIT(4));

      c = a ^ b;
      WD_TEST_BOOL(c.x() == (WD_BIT(1) | WD_BIT(4)) && c.y() == (WD_BIT(2) | WD_BIT(3)) && c.z() == 0 && c.w() == WD_BIT(4) - 1);

      c = ~a;
      WD_TEST_BOOL(c.x() == 0xFFFFFFFD && c.y() == 0xFFFFFFFB && c.z() == 0xFFFFFFF7 && c.w() == 0xFFFFFFEF);

      c = a << 3;
      WD_TEST_BOOL(c.x() == WD_BIT(4) && c.y() == WD_BIT(5) && c.z() == WD_BIT(6) && c.w() == WD_BIT(7));

      c = a >> 1;
      WD_TEST_BOOL(c.x() == WD_BIT(0) && c.y() == WD_BIT(1) && c.z() == WD_BIT(2) && c.w() == WD_BIT(3));
    }

    {
      wdSimdVec4u a(-3, 5, -7, 9);
      wdSimdVec4u b(8, 6, 4, 2);

      wdSimdVec4u c = a;
      c += b;
      WD_TEST_BOOL(c.x() == 5 && c.y() == 11 && c.z() == -3 && c.w() == 11);

      c = a;
      c -= b;
      WD_TEST_BOOL(c.x() == -11 && c.y() == -1 && c.z() == -11 && c.w() == 7);
    }

    {
      wdSimdVec4u a(WD_BIT(1), WD_BIT(2), WD_BIT(3), WD_BIT(4));
      wdSimdVec4u b(WD_BIT(4), WD_BIT(3), WD_BIT(3), WD_BIT(5) - 1);

      wdSimdVec4u c = a;
      c |= b;
      WD_TEST_BOOL(c.x() == (WD_BIT(1) | WD_BIT(4)) && c.y() == (WD_BIT(2) | WD_BIT(3)) && c.z() == WD_BIT(3) && c.w() == WD_BIT(5) - 1);

      c = a;
      c &= b;
      WD_TEST_BOOL(c.x() == 0 && c.y() == 0 && c.z() == WD_BIT(3) && c.w() == WD_BIT(4));

      c = a;
      c ^= b;
      WD_TEST_BOOL(c.x() == (WD_BIT(1) | WD_BIT(4)) && c.y() == (WD_BIT(2) | WD_BIT(3)) && c.z() == 0 && c.w() == WD_BIT(4) - 1);

      c = a;
      c <<= 3;
      WD_TEST_BOOL(c.x() == WD_BIT(4) && c.y() == WD_BIT(5) && c.z() == WD_BIT(6) && c.w() == WD_BIT(7));

      c = a;
      c >>= 1;
      WD_TEST_BOOL(c.x() == WD_BIT(0) && c.y() == WD_BIT(1) && c.z() == WD_BIT(2) && c.w() == WD_BIT(3));

      c = wdSimdVec4u(-2, -4, -7, -8);
      WD_TEST_BOOL(c.x() == 0xFFFFFFFE && c.y() == 0xFFFFFFFC && c.z() == 0xFFFFFFF9 && c.w() == 0xFFFFFFF8);
      c >>= 1;
      WD_TEST_BOOL(c.x() == 0x7FFFFFFF && c.y() == 0x7FFFFFFE && c.z() == 0x7FFFFFFC && c.w() == 0x7FFFFFFC);
    }

    {
      wdSimdVec4u a(-3, 5, -7, 9);
      wdSimdVec4u b(8, 6, 4, 2);
      wdSimdVec4u c;

      c = a.CompMin(b);
      WD_TEST_BOOL(c.x() == 8 && c.y() == 5 && c.z() == 4 && c.w() == 2);

      c = a.CompMax(b);
      WD_TEST_BOOL(c.x() == -3 && c.y() == 6 && c.z() == -7 && c.w() == 9);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Comparison")
  {
    wdSimdVec4u a(-7, 5, 4, 3);
    wdSimdVec4u b(8, 6, 4, -2);
    wdSimdVec4b cmp;

    cmp = a == b;
    WD_TEST_BOOL(!cmp.x() && !cmp.y() && cmp.z() && !cmp.w());

    cmp = a != b;
    WD_TEST_BOOL(cmp.x() && cmp.y() && !cmp.z() && cmp.w());

    cmp = a <= b;
    WD_TEST_BOOL(!cmp.x() && cmp.y() && cmp.z() && cmp.w());

    cmp = a < b;
    WD_TEST_BOOL(!cmp.x() && cmp.y() && !cmp.z() && cmp.w());

    cmp = a >= b;
    WD_TEST_BOOL(cmp.x() && !cmp.y() && cmp.z() && !cmp.w());

    cmp = a > b;
    WD_TEST_BOOL(cmp.x() && !cmp.y() && !cmp.z() && !cmp.w());
  }
}
