#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/SimdMath/SimdMath.h>

namespace
{
  wdSimdVec4f SimdDegree(float fDegree)
  {
    return wdSimdVec4f(wdAngle::Degree(fDegree));
  }
} // namespace

WD_CREATE_SIMPLE_TEST(SimdMath, SimdMath)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Exp")
  {
    float testVals[] = {0.0f, 1.0f, 2.0f};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const float v = testVals[i];
      const float r = wdMath::Exp(v);
      WD_TEST_BOOL(wdSimdMath::Exp(wdSimdVec4f(v)).IsEqual(wdSimdVec4f(r), 0.000001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ln")
  {
    float testVals[] = {1.0f, 2.7182818284f, 7.3890560989f};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const float v = testVals[i];
      const float r = wdMath::Ln(v);
      WD_TEST_BOOL(wdSimdMath::Ln(wdSimdVec4f(v)).IsEqual(wdSimdVec4f(r), 0.000001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log2")
  {
    float testVals[] = {1.0f, 2.0f, 4.0f};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const float v = testVals[i];
      const float r = wdMath::Log2(v);
      WD_TEST_BOOL(wdSimdMath::Log2(wdSimdVec4f(v)).IsEqual(wdSimdVec4f(r), 0.000001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log2i")
  {
    int testVals[] = {0, 1, 2, 3, 4, 6, 7, 8};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const int v = testVals[i];
      const int r = wdMath::Log2i(v);
      WD_TEST_BOOL((wdSimdMath::Log2i(wdSimdVec4i(v)) == wdSimdVec4i(r)).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log10")
  {
    float testVals[] = {1.0f, 10.0f, 100.0f};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const float v = testVals[i];
      const float r = wdMath::Log10(v);
      WD_TEST_BOOL(wdSimdMath::Log10(wdSimdVec4f(v)).IsEqual(wdSimdVec4f(r), 0.000001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Pow2")
  {
    float testVals[] = {0.0f, 1.0f, 2.0f};
    for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(testVals); ++i)
    {
      const float v = testVals[i];
      const float r = wdMath::Pow2(v);
      WD_TEST_BOOL(wdSimdMath::Pow2(wdSimdVec4f(v)).IsEqual(wdSimdVec4f(r), 0.000001f).AllSet());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sin")
  {
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(0.0f)).IsEqual(wdSimdVec4f(0.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(90.0f)).IsEqual(wdSimdVec4f(1.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(180.0f)).IsEqual(wdSimdVec4f(0.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(270.0f)).IsEqual(wdSimdVec4f(-1.0f), 0.000001f).AllSet());

    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(45.0f)).IsEqual(wdSimdVec4f(0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(135.0f)).IsEqual(wdSimdVec4f(0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(225.0f)).IsEqual(wdSimdVec4f(-0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Sin(SimdDegree(315.0f)).IsEqual(wdSimdVec4f(-0.7071067f), 0.000001f).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Cos")
  {
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(0.0f)).IsEqual(wdSimdVec4f(1.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(90.0f)).IsEqual(wdSimdVec4f(0.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(180.0f)).IsEqual(wdSimdVec4f(-1.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(270.0f)).IsEqual(wdSimdVec4f(0.0f), 0.000001f).AllSet());

    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(45.0f)).IsEqual(wdSimdVec4f(0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(135.0f)).IsEqual(wdSimdVec4f(-0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(225.0f)).IsEqual(wdSimdVec4f(-0.7071067f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Cos(SimdDegree(315.0f)).IsEqual(wdSimdVec4f(0.7071067f), 0.000001f).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Tan")
  {
    WD_TEST_BOOL(wdSimdMath::Tan(SimdDegree(0.0f)).IsEqual(wdSimdVec4f(0.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Tan(SimdDegree(45.0f)).IsEqual(wdSimdVec4f(1.0f), 0.000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::Tan(SimdDegree(-45.0f)).IsEqual(wdSimdVec4f(-1.0f), 0.000001f).AllSet());
    WD_TEST_BOOL((wdSimdMath::Tan(SimdDegree(90.00001f)) < wdSimdVec4f(1000000.0f)).AllSet());
    WD_TEST_BOOL((wdSimdMath::Tan(SimdDegree(89.9999f)) > wdSimdVec4f(100000.0f)).AllSet());

    // Testing the period of tan(x) centered at 0 and the adjacent ones
    wdAngle angle = wdAngle::Degree(-89.0f);
    while (angle.GetDegree() < 89.0f)
    {
      wdSimdVec4f simdAngle(angle.GetRadian());

      wdSimdVec4f fTan = wdSimdMath::Tan(simdAngle);
      wdSimdVec4f fTanPrev = wdSimdMath::Tan(SimdDegree(angle.GetDegree() - 180.0f));
      wdSimdVec4f fTanNext = wdSimdMath::Tan(SimdDegree(angle.GetDegree() + 180.0f));
      wdSimdVec4f fSin = wdSimdMath::Sin(simdAngle);
      wdSimdVec4f fCos = wdSimdMath::Cos(simdAngle);

      WD_TEST_BOOL((fTan - fTanPrev).IsEqual(wdSimdVec4f::ZeroVector(), 0.002f).AllSet());
      WD_TEST_BOOL((fTan - fTanNext).IsEqual(wdSimdVec4f::ZeroVector(), 0.002f).AllSet());
      WD_TEST_BOOL((fTan - fSin.CompDiv(fCos)).IsEqual(wdSimdVec4f::ZeroVector(), 0.0005f).AllSet());
      angle += wdAngle::Degree(1.234f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ASin")
  {
    WD_TEST_BOOL(wdSimdMath::ASin(wdSimdVec4f(0.0f)).IsEqual(SimdDegree(0.0f), 0.0001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ASin(wdSimdVec4f(1.0f)).IsEqual(SimdDegree(90.0f), 0.00001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ASin(wdSimdVec4f(-1.0f)).IsEqual(SimdDegree(-90.0f), 0.00001f).AllSet());

    WD_TEST_BOOL(wdSimdMath::ASin(wdSimdVec4f(0.7071067f)).IsEqual(SimdDegree(45.0f), 0.0001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ASin(wdSimdVec4f(-0.7071067f)).IsEqual(SimdDegree(-45.0f), 0.0001f).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ACos")
  {
    WD_TEST_BOOL(wdSimdMath::ACos(wdSimdVec4f(0.0f)).IsEqual(SimdDegree(90.0f), 0.0001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ACos(wdSimdVec4f(1.0f)).IsEqual(SimdDegree(0.0f), 0.00001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ACos(wdSimdVec4f(-1.0f)).IsEqual(SimdDegree(180.0f), 0.0001f).AllSet());

    WD_TEST_BOOL(wdSimdMath::ACos(wdSimdVec4f(0.7071067f)).IsEqual(SimdDegree(45.0f), 0.0001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ACos(wdSimdVec4f(-0.7071067f)).IsEqual(SimdDegree(135.0f), 0.0001f).AllSet());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ATan")
  {
    WD_TEST_BOOL(wdSimdMath::ATan(wdSimdVec4f(0.0f)).IsEqual(SimdDegree(0.0f), 0.0000001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ATan(wdSimdVec4f(1.0f)).IsEqual(SimdDegree(45.0f), 0.00001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ATan(wdSimdVec4f(-1.0f)).IsEqual(SimdDegree(-45.0f), 0.00001f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ATan(wdSimdVec4f(10000000.0f)).IsEqual(SimdDegree(90.0f), 0.00002f).AllSet());
    WD_TEST_BOOL(wdSimdMath::ATan(wdSimdVec4f(-10000000.0f)).IsEqual(SimdDegree(-90.0f), 0.00002f).AllSet());
  }
}
