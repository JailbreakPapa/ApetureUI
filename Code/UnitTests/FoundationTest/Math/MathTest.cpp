#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Logging/Log.h>
#include <Foundation/Math/Math.h>
#include <Foundation/Math/Vec2.h>

/// ********************* Binary to Int conversion *********************
/// Most significant bit comes first.
/// Adapted from http://bytes.com/topic/c/answers/219656-literal-binary
///
/// Sample usage:
/// WD_8BIT(01010101) == 85
/// WD_16BIT(10101010, 01010101) == 43605
/// WD_32BIT(10000000, 11111111, 10101010, 01010101) == 2164238933
/// ********************************************************************
#define OCT__(n) 0##n##LU

#define WD_8BIT__(iBits)                                                                                                           \
  (((iBits & 000000001) ? 1 : 0) + ((iBits & 000000010) ? 2 : 0) + ((iBits & 000000100) ? 4 : 0) + ((iBits & 000001000) ? 8 : 0) + \
    ((iBits & 000010000) ? 16 : 0) + ((iBits & 000100000) ? 32 : 0) + ((iBits & 001000000) ? 64 : 0) + ((iBits & 010000000) ? 128 : 0))

#define WD_8BIT(B) ((wdUInt8)WD_8BIT__(OCT__(B)))

#define WD_16BIT(B2, B1) (((wdUInt8)WD_8BIT(B2) << 8) + WD_8BIT(B1))

#define WD_32BIT(B4, B3, B2, B1) \
  ((unsigned long)WD_8BIT(B4) << 24) + ((unsigned long)WD_8BIT(B3) << 16) + ((unsigned long)WD_8BIT(B2) << 8) + ((unsigned long)WD_8BIT(B1))

namespace
{
  struct UniqueInt
  {
    int i, id;
    UniqueInt(int i, int iId)
      : i(i)
      , id(iId)
    {
    }

    bool operator<(const UniqueInt& rh) { return this->i < rh.i; }

    bool operator>(const UniqueInt& rh) { return this->i > rh.i; }
  };
}; // namespace


WD_CREATE_SIMPLE_TEST_GROUP(Math);

WD_CREATE_SIMPLE_TEST(Math, General)
{
  // WD_TEST_BLOCK(wdTestBlock::Enabled, "Constants")
  //{
  //  // Macro test
  //  WD_TEST_BOOL(WD_8BIT(01010101) == 85);
  //  WD_TEST_BOOL(WD_16BIT(10101010, 01010101) == 43605);
  //  WD_TEST_BOOL(WD_32BIT(10000000, 11111111, 10101010, 01010101) == 2164238933);

  //  // Infinity test
  //  //                           Sign:_
  //  //                       Exponent: _______  _
  //  //                       Fraction:           _______  ________  ________
  //  wdIntFloatUnion uInf = { WD_32BIT(01111111, 10000000, 00000000, 00000000) };
  //  WD_TEST_BOOL(uInf.f == wdMath::FloatInfinity());

  //  // FloatMax_Pos test
  //  wdIntFloatUnion uMax = { WD_32BIT(01111111, 01111111, 11111111, 11111111) };
  //  WD_TEST_BOOL(uMax.f == wdMath::FloatMax_Pos());

  //  // FloatMax_Neg test
  //  wdIntFloatUnion uMin = { WD_32BIT(11111111, 01111111, 11111111, 11111111) };
  //  WD_TEST_BOOL(uMin.f == wdMath::FloatMax_Neg());
  //}

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sin")
  {
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(0.0f)), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(90.0f)), 1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(180.0f)), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(270.0f)), -1.0f, 0.000001f);

    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(45.0f)), 0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(135.0f)), 0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(225.0f)), -0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Sin(wdAngle::Degree(315.0f)), -0.7071067f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Cos")
  {
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(0.0f)), 1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(90.0f)), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(180.0f)), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(270.0f)), 0.0f, 0.000001f);

    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(45.0f)), 0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(135.0f)), -0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(225.0f)), -0.7071067f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Cos(wdAngle::Degree(315.0f)), 0.7071067f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Tan")
  {
    WD_TEST_FLOAT(wdMath::Tan(wdAngle::Degree(0.0f)), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Tan(wdAngle::Degree(45.0f)), 1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Tan(wdAngle::Degree(-45.0f)), -1.0f, 0.000001f);
    WD_TEST_BOOL(wdMath::Tan(wdAngle::Degree(90.00001f)) < 1000000.0f);
    WD_TEST_BOOL(wdMath::Tan(wdAngle::Degree(89.9999f)) > 100000.0f);

    // Testing the period of tan(x) centered at 0 and the adjacent ones
    wdAngle angle = wdAngle::Degree(-89.0f);
    while (angle.GetDegree() < 89.0f)
    {
      float fTan = wdMath::Tan(angle);
      float fTanPrev = wdMath::Tan(wdAngle::Degree(angle.GetDegree() - 180.0f));
      float fTanNext = wdMath::Tan(wdAngle::Degree(angle.GetDegree() + 180.0f));
      float fSin = wdMath::Sin(angle);
      float fCos = wdMath::Cos(angle);

      WD_TEST_FLOAT(fTan - fTanPrev, 0.0f, 0.002f);
      WD_TEST_FLOAT(fTan - fTanNext, 0.0f, 0.002f);
      WD_TEST_FLOAT(fTan - (fSin / fCos), 0.0f, 0.0005f);
      angle += wdAngle::Degree(1.234f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ASin")
  {
    WD_TEST_FLOAT(wdMath::ASin(0.0f).GetDegree(), 0.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ASin(1.0f).GetDegree(), 90.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ASin(-1.0f).GetDegree(), -90.0f, 0.00001f);

    WD_TEST_FLOAT(wdMath::ASin(0.7071067f).GetDegree(), 45.0f, 0.0001f);
    WD_TEST_FLOAT(wdMath::ASin(-0.7071067f).GetDegree(), -45.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ACos")
  {
    WD_TEST_FLOAT(wdMath::ACos(0.0f).GetDegree(), 90.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ACos(1.0f).GetDegree(), 0.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ACos(-1.0f).GetDegree(), 180.0f, 0.0001f);

    WD_TEST_FLOAT(wdMath::ACos(0.7071067f).GetDegree(), 45.0f, 0.0001f);
    WD_TEST_FLOAT(wdMath::ACos(-0.7071067f).GetDegree(), 135.0f, 0.0001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ATan")
  {
    WD_TEST_FLOAT(wdMath::ATan(0.0f).GetDegree(), 0.0f, 0.0000001f);
    WD_TEST_FLOAT(wdMath::ATan(1.0f).GetDegree(), 45.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ATan(-1.0f).GetDegree(), -45.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::ATan(10000000.0f).GetDegree(), 90.0f, 0.00002f);
    WD_TEST_FLOAT(wdMath::ATan(-10000000.0f).GetDegree(), -90.0f, 0.00002f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ATan2")
  {
    for (float fScale = 0.125f; fScale < 1000000.0f; fScale *= 2.0f)
    {
      WD_TEST_FLOAT(wdMath::ATan2(0.0f, fScale).GetDegree(), 0.0f, 0.0000001f);
      WD_TEST_FLOAT(wdMath::ATan2(fScale, fScale).GetDegree(), 45.0f, 0.00001f);
      WD_TEST_FLOAT(wdMath::ATan2(fScale, 0.0f).GetDegree(), 90.0f, 0.00001f);
      WD_TEST_FLOAT(wdMath::ATan2(-fScale, fScale).GetDegree(), -45.0f, 0.00001f);
      WD_TEST_FLOAT(wdMath::ATan2(-fScale, 0.0f).GetDegree(), -90.0f, 0.00001f);
      WD_TEST_FLOAT(wdMath::ATan2(0.0f, -fScale).GetDegree(), 180.0f, 0.0001f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Exp")
  {
    WD_TEST_FLOAT(1.0f, wdMath::Exp(0.0f), 0.000001f);
    WD_TEST_FLOAT(2.7182818284f, wdMath::Exp(1.0f), 0.000001f);
    WD_TEST_FLOAT(7.3890560989f, wdMath::Exp(2.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ln")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Ln(1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Ln(2.7182818284f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Ln(7.3890560989f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log2")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Log2(1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Log2(2.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Log2(4.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log2i")
  {
    WD_TEST_BOOL(wdMath::Log2i(0) == wdUInt32(-1));
    WD_TEST_BOOL(wdMath::Log2i(1) == 0);
    WD_TEST_BOOL(wdMath::Log2i(2) == 1);
    WD_TEST_BOOL(wdMath::Log2i(3) == 1);
    WD_TEST_BOOL(wdMath::Log2i(4) == 2);
    WD_TEST_BOOL(wdMath::Log2i(6) == 2);
    WD_TEST_BOOL(wdMath::Log2i(7) == 2);
    WD_TEST_BOOL(wdMath::Log2i(8) == 3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log10")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Log10(1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Log10(10.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Log10(100.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Log")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Log(2.7182818284f, 1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Log(2.7182818284f, 2.7182818284f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Log(2.7182818284f, 7.3890560989f), 0.000001f);

    WD_TEST_FLOAT(0.0f, wdMath::Log(2.0f, 1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Log(2.0f, 2.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Log(2.0f, 4.0f), 0.000001f);

    WD_TEST_FLOAT(0.0f, wdMath::Log(10.0f, 1.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Log(10.0f, 10.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Log(10.0f, 100.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Pow2")
  {
    WD_TEST_FLOAT(1.0f, wdMath::Pow2(0.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Pow2(1.0f), 0.000001f);
    WD_TEST_FLOAT(4.0f, wdMath::Pow2(2.0f), 0.000001f);

    WD_TEST_BOOL(wdMath::Pow2(0) == 1);
    WD_TEST_BOOL(wdMath::Pow2(1) == 2);
    WD_TEST_BOOL(wdMath::Pow2(2) == 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Pow")
  {
    WD_TEST_FLOAT(1.0f, wdMath::Pow(3.0f, 0.0f), 0.000001f);
    WD_TEST_FLOAT(3.0f, wdMath::Pow(3.0f, 1.0f), 0.000001f);
    WD_TEST_FLOAT(9.0f, wdMath::Pow(3.0f, 2.0f), 0.000001f);

    WD_TEST_BOOL(wdMath::Pow(3, 0) == 1);
    WD_TEST_BOOL(wdMath::Pow(3, 1) == 3);
    WD_TEST_BOOL(wdMath::Pow(3, 2) == 9);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Square")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Square(0.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Square(1.0f), 0.000001f);
    WD_TEST_FLOAT(4.0f, wdMath::Square(2.0f), 0.000001f);
    WD_TEST_FLOAT(4.0f, wdMath::Square(-2.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sqrt (float)")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Sqrt(0.0f), 0.000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Sqrt(1.0f), 0.000001f);
    WD_TEST_FLOAT(2.0f, wdMath::Sqrt(4.0f), 0.000001f);
    WD_TEST_FLOAT(4.0f, wdMath::Sqrt(16.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sqrt (double)")
  {
    WD_TEST_DOUBLE(0.0, wdMath::Sqrt(0.0), 0.000001);
    WD_TEST_DOUBLE(1.0, wdMath::Sqrt(1.0), 0.000001);
    WD_TEST_DOUBLE(2.0, wdMath::Sqrt(4.0), 0.000001);
    WD_TEST_DOUBLE(4.0, wdMath::Sqrt(16.0), 0.000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Root")
  {
    WD_TEST_FLOAT(3.0f, wdMath::Root(27.0f, 3.0f), 0.000001f);
    WD_TEST_FLOAT(3.0f, wdMath::Root(81.0f, 4.0f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sign")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Sign(0.0f), 0.00000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Sign(0.01f), 0.00000001f);
    WD_TEST_FLOAT(-1.0f, wdMath::Sign(-0.01f), 0.00000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Abs")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Abs(0.0f), 0.00000001f);
    WD_TEST_FLOAT(20.0f, wdMath::Abs(20.0f), 0.00000001f);
    WD_TEST_FLOAT(20.0f, wdMath::Abs(-20.0f), 0.00000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Min")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Min(0.0f, 23.0f), 0.00000001f);
    WD_TEST_FLOAT(-23.0f, wdMath::Min(0.0f, -23.0f), 0.00000001f);

    WD_TEST_BOOL(wdMath::Min(1, 2, 3) == 1);
    WD_TEST_BOOL(wdMath::Min(4, 2, 3) == 2);
    WD_TEST_BOOL(wdMath::Min(4, 5, 3) == 3);

    WD_TEST_BOOL(wdMath::Min(1, 2, 3, 4) == 1);
    WD_TEST_BOOL(wdMath::Min(5, 2, 3, 4) == 2);
    WD_TEST_BOOL(wdMath::Min(5, 6, 3, 4) == 3);
    WD_TEST_BOOL(wdMath::Min(5, 6, 7, 4) == 4);

    WD_TEST_BOOL(wdMath::Min(UniqueInt(1, 0), UniqueInt(1, 1)).id == 0);
    WD_TEST_BOOL(wdMath::Min(UniqueInt(1, 1), UniqueInt(1, 0)).id == 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Max")
  {
    WD_TEST_FLOAT(23.0f, wdMath::Max(0.0f, 23.0f), 0.00000001f);
    WD_TEST_FLOAT(0.0f, wdMath::Max(0.0f, -23.0f), 0.00000001f);

    WD_TEST_BOOL(wdMath::Max(1, 2, 3) == 3);
    WD_TEST_BOOL(wdMath::Max(1, 2, 0) == 2);
    WD_TEST_BOOL(wdMath::Max(1, 0, 0) == 1);

    WD_TEST_BOOL(wdMath::Max(1, 2, 3, 4) == 4);
    WD_TEST_BOOL(wdMath::Max(1, 2, 3, 0) == 3);
    WD_TEST_BOOL(wdMath::Max(1, 2, 0, 0) == 2);
    WD_TEST_BOOL(wdMath::Max(1, 0, 0, 0) == 1);

    WD_TEST_BOOL(wdMath::Max(UniqueInt(1, 0), UniqueInt(1, 1)).id == 0);
    WD_TEST_BOOL(wdMath::Max(UniqueInt(1, 1), UniqueInt(1, 0)).id == 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clamp")
  {
    WD_TEST_FLOAT(15.0f, wdMath::Clamp(23.0f, 12.0f, 15.0f), 0.00000001f);
    WD_TEST_FLOAT(12.0f, wdMath::Clamp(3.0f, 12.0f, 15.0f), 0.00000001f);
    WD_TEST_FLOAT(14.0f, wdMath::Clamp(14.0f, 12.0f, 15.0f), 0.00000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Saturate")
  {
    WD_TEST_FLOAT(0.0f, wdMath::Saturate(-1.5f), 0.00000001f);
    WD_TEST_FLOAT(0.5f, wdMath::Saturate(0.5f), 0.00000001f);
    WD_TEST_FLOAT(1.0f, wdMath::Saturate(12345.0f), 0.00000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Floor")
  {
    WD_TEST_BOOL(12 == wdMath::Floor(12.34f));
    WD_TEST_BOOL(-13 == wdMath::Floor(-12.34f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ceil")
  {
    WD_TEST_BOOL(13 == wdMath::Ceil(12.34f));
    WD_TEST_BOOL(-12 == wdMath::Ceil(-12.34f));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundDown (float)")
  {
    WD_TEST_FLOAT(10.0f, wdMath::RoundDown(12.34f, 5.0f), 0.0000001f);
    WD_TEST_FLOAT(-15.0f, wdMath::RoundDown(-12.34f, 5.0f), 0.0000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundUp (float)")
  {
    WD_TEST_FLOAT(15.0f, wdMath::RoundUp(12.34f, 5.0f), 0.0000001f);
    WD_TEST_FLOAT(-10.0f, wdMath::RoundUp(-12.34f, 5.0f), 0.0000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundDown (double)")
  {
    WD_TEST_DOUBLE(10.0, wdMath::RoundDown(12.34, 5.0), 0.0000001);
    WD_TEST_DOUBLE(-15.0, wdMath::RoundDown(-12.34, 5.0), 0.0000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundUp (double)")
  {
    WD_TEST_DOUBLE(15.0, wdMath::RoundUp(12.34, 5.0), 0.0000001);
    WD_TEST_DOUBLE(-10.0, wdMath::RoundUp(-12.34, 5.0), 0.0000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Trunc")
  {
    WD_TEST_BOOL(wdMath::Trunc(12.34f) == 12);
    WD_TEST_BOOL(wdMath::Trunc(-12.34f) == -12);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FloatToInt")
  {
    WD_TEST_BOOL(wdMath::FloatToInt(12.34f) == 12);
    WD_TEST_BOOL(wdMath::FloatToInt(-12.34f) == -12);

#if WD_DISABLED(WD_PLATFORM_ARCH_X86) || (_MSC_VER <= 1916)
    WD_TEST_BOOL(wdMath::FloatToInt(12000000000000.34) == 12000000000000);
    WD_TEST_BOOL(wdMath::FloatToInt(-12000000000000.34) == -12000000000000);
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Round")
  {
    WD_TEST_BOOL(wdMath::Round(12.34f) == 12);
    WD_TEST_BOOL(wdMath::Round(-12.34f) == -12);

    WD_TEST_BOOL(wdMath::Round(12.54f) == 13);
    WD_TEST_BOOL(wdMath::Round(-12.54f) == -13);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundClosest (float)")
  {
    WD_TEST_FLOAT(wdMath::RoundToMultiple(12.0f, 3.0f), 12.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::RoundToMultiple(-12.0f, 3.0f), -12.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::RoundToMultiple(12.34f, 7.0f), 14.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::RoundToMultiple(-12.34f, 7.0f), -14.0f, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundClosest (double)")
  {
    WD_TEST_DOUBLE(wdMath::RoundToMultiple(12.0, 3.0), 12.0, 0.00001);
    WD_TEST_DOUBLE(wdMath::RoundToMultiple(-12.0, 3.0), -12.0, 0.00001);
    WD_TEST_DOUBLE(wdMath::RoundToMultiple(12.34, 7.0), 14.0, 0.00001);
    WD_TEST_DOUBLE(wdMath::RoundToMultiple(-12.34, 7.0), -14.0, 0.00001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundUp (int)")
  {
    WD_TEST_INT(wdMath::RoundUp(12, 7), 14);
    WD_TEST_INT(wdMath::RoundUp(-12, 7), -7);
    WD_TEST_INT(wdMath::RoundUp(16, 4), 16);
    WD_TEST_INT(wdMath::RoundUp(-16, 4), -16);
    WD_TEST_INT(wdMath::RoundUp(17, 4), 20);
    WD_TEST_INT(wdMath::RoundUp(-17, 4), -16);
    WD_TEST_INT(wdMath::RoundUp(15, 4), 16);
    WD_TEST_INT(wdMath::RoundUp(-15, 4), -12);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundDown (int)")
  {
    WD_TEST_INT(wdMath::RoundDown(12, 7), 7);
    WD_TEST_INT(wdMath::RoundDown(-12, 7), -14);
    WD_TEST_INT(wdMath::RoundDown(16, 4), 16);
    WD_TEST_INT(wdMath::RoundDown(-16, 4), -16);
    WD_TEST_INT(wdMath::RoundDown(17, 4), 16);
    WD_TEST_INT(wdMath::RoundDown(-17, 4), -20);
    WD_TEST_INT(wdMath::RoundDown(15, 4), 12);
    WD_TEST_INT(wdMath::RoundDown(-15, 4), -16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundUp (unsigned int)")
  {
    WD_TEST_INT(wdMath::RoundUp(12u, 7), 14);
    WD_TEST_INT(wdMath::RoundUp(16u, 4), 16);
    WD_TEST_INT(wdMath::RoundUp(17u, 4), 20);
    WD_TEST_INT(wdMath::RoundUp(15u, 4), 16);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RoundDown (unsigned int)")
  {
    WD_TEST_INT(wdMath::RoundDown(12u, 7), 7);
    WD_TEST_INT(wdMath::RoundDown(16u, 4), 16);
    WD_TEST_INT(wdMath::RoundDown(17u, 4), 16);
    WD_TEST_INT(wdMath::RoundDown(15u, 4), 12);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Fraction")
  {
    WD_TEST_FLOAT(wdMath::Fraction(12.34f), 0.34f, 0.00001f);
    WD_TEST_FLOAT(wdMath::Fraction(-12.34f), -0.34f, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Mod (float)")
  {
    WD_TEST_FLOAT(2.34f, wdMath::Mod(12.34f, 2.5f), 0.000001f);
    WD_TEST_FLOAT(-2.34f, wdMath::Mod(-12.34f, 2.5f), 0.000001f);

    WD_TEST_FLOAT(2.34f, wdMath::Mod(12.34f, -2.5f), 0.000001f);
    WD_TEST_FLOAT(-2.34f, wdMath::Mod(-12.34f, -2.5f), 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Mod (double)")
  {
    WD_TEST_DOUBLE(2.34, wdMath::Mod(12.34, 2.5), 0.000001);
    WD_TEST_DOUBLE(-2.34, wdMath::Mod(-12.34, 2.5), 0.000001);

    WD_TEST_DOUBLE(2.34, wdMath::Mod(12.34, -2.5), 0.000001);
    WD_TEST_DOUBLE(-2.34, wdMath::Mod(-12.34, -2.5), 0.000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invert")
  {
    WD_TEST_FLOAT(wdMath::Invert(1.0f), 1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Invert(2.0f), 0.5f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Invert(4.0f), 0.25f, 0.000001f);

    WD_TEST_FLOAT(wdMath::Invert(-1.0f), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Invert(-2.0f), -0.5f, 0.000001f);
    WD_TEST_FLOAT(wdMath::Invert(-4.0f), -0.25f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Odd")
  {
    WD_TEST_BOOL(wdMath::IsOdd(0) == false);
    WD_TEST_BOOL(wdMath::IsOdd(1) == true);
    WD_TEST_BOOL(wdMath::IsOdd(2) == false);
    WD_TEST_BOOL(wdMath::IsOdd(-1) == true);
    WD_TEST_BOOL(wdMath::IsOdd(-2) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Even")
  {
    WD_TEST_BOOL(wdMath::IsEven(0) == true);
    WD_TEST_BOOL(wdMath::IsEven(1) == false);
    WD_TEST_BOOL(wdMath::IsEven(2) == true);
    WD_TEST_BOOL(wdMath::IsEven(-1) == false);
    WD_TEST_BOOL(wdMath::IsEven(-2) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Swap")
  {
    wdInt32 a = 1;
    wdInt32 b = 2;
    wdMath::Swap(a, b);
    WD_TEST_BOOL((a == 2) && (b == 1));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Lerp")
  {
    WD_TEST_FLOAT(wdMath::Lerp(-5.0f, 5.0f, 0.5f), 0.0f, 0.000001);
    WD_TEST_FLOAT(wdMath::Lerp(0.0f, 5.0f, 0.5f), 2.5f, 0.000001);
    WD_TEST_FLOAT(wdMath::Lerp(-5.0f, 5.0f, 0.0f), -5.0f, 0.000001);
    WD_TEST_FLOAT(wdMath::Lerp(-5.0f, 5.0f, 1.0f), 5.0f, 0.000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Step")
  {
    WD_TEST_FLOAT(wdMath::Step(0.5f, 0.4f), 1.0f, 0.00001f);
    WD_TEST_FLOAT(wdMath::Step(0.3f, 0.4f), 0.0f, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SmoothStep")
  {
    // Only test values that must be true for any symmetric step function.
    // How should one test smoothness?
    for (int iScale = -19; iScale <= 19; iScale += 2)
    {
      WD_TEST_FLOAT(wdMath::SmoothStep(0.0f * iScale, 0.1f * iScale, 0.4f * iScale), 0.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.1f * iScale, 0.1f * iScale, 0.4f * iScale), 0.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.4f * iScale, 0.1f * iScale, 0.4f * iScale), 1.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.25f * iScale, 0.1f * iScale, 0.4f * iScale), 0.5f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.5f * iScale, 0.1f * iScale, 0.4f * iScale), 1.0f, 0.000001);

      WD_TEST_FLOAT(wdMath::SmoothStep(0.5f * iScale, 0.4f * iScale, 0.1f * iScale), 0.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.4f * iScale, 0.4f * iScale, 0.1f * iScale), 0.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.1f * iScale, 0.4f * iScale, 0.1f * iScale), 1.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.25f * iScale, 0.1f * iScale, 0.4f * iScale), 0.5f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.0f * iScale, 0.4f * iScale, 0.1f * iScale), 1.0f, 0.000001);

      // For edge1 == edge2 SmoothStep should behave like Step
      WD_TEST_FLOAT(wdMath::SmoothStep(0.0f * iScale, 0.1f * iScale, 0.1f * iScale), iScale > 0 ? 0.0f : 1.0f, 0.000001);
      WD_TEST_FLOAT(wdMath::SmoothStep(0.2f * iScale, 0.1f * iScale, 0.1f * iScale), iScale < 0 ? 0.0f : 1.0f, 0.000001);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsPowerOf")
  {
    WD_TEST_BOOL(wdMath::IsPowerOf(4, 2) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf(5, 2) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf(0, 2) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf(1, 2) == true);

    WD_TEST_BOOL(wdMath::IsPowerOf(4, 3) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf(3, 3) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf(1, 3) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf(27, 3) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf(28, 3) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsPowerOf2")
  {
    WD_TEST_BOOL(wdMath::IsPowerOf2(4) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf2(5) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf2(0) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf2(1) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf2(0x7FFFFFFFu) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf2(0x80000000u) == true);
    WD_TEST_BOOL(wdMath::IsPowerOf2(0x80000001u) == false);
    WD_TEST_BOOL(wdMath::IsPowerOf2(0xFFFFFFFFu) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PowerOf2_Floor")
  {
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(64), 64);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(33), 32);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(4), 4);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(5), 4);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(1), 1);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(0x80000000), 0x80000000);
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(0x80000001), 0x80000000);
    // strange case...
    WD_TEST_INT(wdMath::PowerOfTwo_Floor(0), 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PowerOf2_Ceil")
  {
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(64), 64);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(33), 64);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(4), 4);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(5), 8);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(1), 1);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(0), 1);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(0x7FFFFFFF), 0x80000000);
    WD_TEST_INT(wdMath::PowerOfTwo_Ceil(0x80000000), 0x80000000);
    // anything above 0x80000000 is undefined behavior due to how left-shift works
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GreatestCommonDivisor")
  {
    WD_TEST_INT(wdMath::GreatestCommonDivisor(13, 13), 13);
    WD_TEST_INT(wdMath::GreatestCommonDivisor(37, 600), 1);
    WD_TEST_INT(wdMath::GreatestCommonDivisor(20, 100), 20);
    WD_TEST_INT(wdMath::GreatestCommonDivisor(624129, 2061517), 18913);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    WD_TEST_BOOL(wdMath::IsEqual(1.0f, 0.999f, 0.01f) == true);
    WD_TEST_BOOL(wdMath::IsEqual(1.0f, 1.001f, 0.01f) == true);
    WD_TEST_BOOL(wdMath::IsEqual(1.0f, 0.999f, 0.0001f) == false);
    WD_TEST_BOOL(wdMath::IsEqual(1.0f, 1.001f, 0.0001f) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "NaN_Infinity")
  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      WD_TEST_BOOL(wdMath::IsNaN(wdMath::NaN<wdMathTestType>()) == true);

      WD_TEST_BOOL(wdMath::Infinity<wdMathTestType>() == wdMath::Infinity<wdMathTestType>() - (wdMathTestType)1);
      WD_TEST_BOOL(wdMath::Infinity<wdMathTestType>() == wdMath::Infinity<wdMathTestType>() + (wdMathTestType)1);

      WD_TEST_BOOL(wdMath::IsNaN(wdMath::Infinity<wdMathTestType>() - wdMath::Infinity<wdMathTestType>()));

      WD_TEST_BOOL(!wdMath::IsFinite(wdMath::Infinity<wdMathTestType>()));
      WD_TEST_BOOL(!wdMath::IsFinite(-wdMath::Infinity<wdMathTestType>()));
      WD_TEST_BOOL(!wdMath::IsFinite(wdMath::NaN<wdMathTestType>()));
      WD_TEST_BOOL(!wdMath::IsNaN(wdMath::Infinity<wdMathTestType>()));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsInRange")
  {
    WD_TEST_BOOL(wdMath::IsInRange(1.0f, 0.0f, 2.0f) == true);
    WD_TEST_BOOL(wdMath::IsInRange(1.0f, 0.0f, 1.0f) == true);
    WD_TEST_BOOL(wdMath::IsInRange(1.0f, 1.0f, 2.0f) == true);
    WD_TEST_BOOL(wdMath::IsInRange(0.0f, 1.0f, 2.0f) == false);
    WD_TEST_BOOL(wdMath::IsInRange(3.0f, 0.0f, 2.0f) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsZero")
  {
    WD_TEST_BOOL(wdMath::IsZero(0.009f, 0.01f) == true);
    WD_TEST_BOOL(wdMath::IsZero(0.001f, 0.01f) == true);
    WD_TEST_BOOL(wdMath::IsZero(0.009f, 0.0001f) == false);
    WD_TEST_BOOL(wdMath::IsZero(0.001f, 0.0001f) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorFloatToByte")
  {
    WD_TEST_INT(wdMath::ColorFloatToByte(wdMath::NaN<float>()), 0);
    WD_TEST_INT(wdMath::ColorFloatToByte(-1.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToByte(0.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToByte(0.4f), 102);
    WD_TEST_INT(wdMath::ColorFloatToByte(1.0f), 255);
    WD_TEST_INT(wdMath::ColorFloatToByte(1.5f), 255);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorFloatToShort")
  {
    WD_TEST_INT(wdMath::ColorFloatToShort(wdMath::NaN<float>()), 0);
    WD_TEST_INT(wdMath::ColorFloatToShort(-1.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToShort(0.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToShort(0.4f), 26214);
    WD_TEST_INT(wdMath::ColorFloatToShort(1.0f), 65535);
    WD_TEST_INT(wdMath::ColorFloatToShort(1.5f), 65535);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorFloatToSignedByte")
  {
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(wdMath::NaN<float>()), 0);
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(-1.0f), -127);
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(0.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(0.4f), 51);
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(1.0f), 127);
    WD_TEST_INT(wdMath::ColorFloatToSignedByte(1.5f), 127);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorFloatToSignedShort")
  {
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(wdMath::NaN<float>()), 0);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(-1.0f), -32767);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(0.0f), 0);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(0.4f), 13107);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(0.5f), 16384);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(1.0f), 32767);
    WD_TEST_INT(wdMath::ColorFloatToSignedShort(1.5f), 32767);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorByteToFloat")
  {
    WD_TEST_FLOAT(wdMath::ColorByteToFloat(0), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorByteToFloat(128), 0.501960784f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorByteToFloat(255), 1.0f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorShortToFloat")
  {
    WD_TEST_FLOAT(wdMath::ColorShortToFloat(0), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorShortToFloat(32768), 0.5000076f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorShortToFloat(65535), 1.0f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorSignedByteToFloat")
  {
    WD_TEST_FLOAT(wdMath::ColorSignedByteToFloat(-128), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedByteToFloat(-127), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedByteToFloat(0), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedByteToFloat(64), 0.50393700787f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedByteToFloat(127), 1.0f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ColorSignedShortToFloat")
  {
    WD_TEST_FLOAT(wdMath::ColorSignedShortToFloat(-32768), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedShortToFloat(-32767), -1.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedShortToFloat(0), 0.0f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedShortToFloat(16384), 0.50001526f, 0.000001f);
    WD_TEST_FLOAT(wdMath::ColorSignedShortToFloat(32767), 1.0f, 0.000001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EvaluateBwdierCurve")
  {
    // Determined through the scientific method of manually comparing the result of the function with an online Bwdier curve generator:
    // https://www.desmos.com/calculator/cahqdxeshd
    const wdVec2 res[] = {wdVec2(1, 5), wdVec2(0.893f, 4.455f), wdVec2(1.112f, 4.008f), wdVec2(1.557f, 3.631f), wdVec2(2.136f, 3.304f), wdVec2(2.750f, 3.000f),
      wdVec2(3.303f, 2.695f), wdVec2(3.701f, 2.368f), wdVec2(3.847f, 1.991f), wdVec2(3.645f, 1.543f), wdVec2(3, 1)};

    const float step = 1.0f / (WD_ARRAY_SIZE(res) - 1);
    for (int i = 0; i < WD_ARRAY_SIZE(res); ++i)
    {
      const wdVec2 r = wdMath::EvaluateBwdierCurve<wdVec2>(step * i, wdVec2(1, 5), wdVec2(0, 3), wdVec2(6, 3), wdVec2(3, 1));
      WD_TEST_VEC2(r, res[i], 0.002f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FirstBitLow")
  {
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt32(0b1111)), 0);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt32(0b1110)), 1);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt32(0b1100)), 2);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt32(0b1000)), 3);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt32(0xFFFFFFFF)), 0);

    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFF000000FF00000F)), 0);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFF000000FF00000E)), 1);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFF000000FF00000C)), 2);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFF000000FF000008)), 3);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFFFFFFFFFFFFFFFF)), 0);

    // Edge cases specifically for 32-bit systems where upper and lower 32-bit are handled individually.
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0x00000000FFFFFFFF)), 0);
    WD_TEST_INT(wdMath::FirstBitLow(wdUInt64(0xFFFFFFFF00000000)), 32);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FirstBitHigh")
  {
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt32(0b1111)), 3);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt32(0b0111)), 2);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt32(0b0011)), 1);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt32(0b0001)), 0);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt32(0xFFFFFFFF)), 31);

    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0x00FF000000FF000F)), 55);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0x007F000000FF000F)), 54);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0x003F000000FF000F)), 53);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0x001F000000FF000F)), 52);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0xFFFFFFFFFFFFFFFF)), 63);

    // Edge cases specifically for 32-bit systems where upper and lower 32-bit are handled individually.
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0x00000000FFFFFFFF)), 31);
    WD_TEST_INT(wdMath::FirstBitHigh(wdUInt64(0xFFFFFFFF00000000)), 63);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CountTrailingZeros (32)")
  {
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1111u), 0);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1110u), 1);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1100u), 2);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1000u), 3);
    WD_TEST_INT(wdMath::CountTrailingZeros(0xFFFFFFFF), 0);
    WD_TEST_INT(wdMath::CountTrailingZeros(0u), 32);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CountTrailingZeros (64)")
  {
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1111llu), 0);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1110llu), 1);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1100llu), 2);
    WD_TEST_INT(wdMath::CountTrailingZeros(0b1000llu), 3);
    WD_TEST_INT(wdMath::CountTrailingZeros(0xFFFFFFFF0llu), 4);
    WD_TEST_INT(wdMath::CountTrailingZeros(0llu), 64);
    WD_TEST_INT(wdMath::CountTrailingZeros(0xFFFFFFFF00llu), 8);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CountLeadingZeros")
  {
    WD_TEST_INT(wdMath::CountLeadingZeros(0b1111), 28);
    WD_TEST_INT(wdMath::CountLeadingZeros(0b0111), 29);
    WD_TEST_INT(wdMath::CountLeadingZeros(0b0011), 30);
    WD_TEST_INT(wdMath::CountLeadingZeros(0b0001), 31);
    WD_TEST_INT(wdMath::CountLeadingZeros(0xFFFFFFFF), 0);
    WD_TEST_INT(wdMath::CountLeadingZeros(0), 32);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TryMultiply32")
  {
    wdUInt32 res;

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 1, 1, 2, 3).Succeeded());
    WD_TEST_INT(res, 6);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 1, 1, 1, 0xFFFFFFFF).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFF);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 0xFFFF, 0x10001).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFF);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 0x3FFFFFF, 2, 4, 8).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFC0);

    res = 1;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 0xFFFFFFFF, 2).Failed());
    WD_TEST_BOOL(res == 1);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply32(res, 0x80000000, 2).Failed()); // slightly above 0xFFFFFFFF
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TryMultiply64")
  {
    wdUInt64 res;

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 1, 1, 2, 3).Succeeded());
    WD_TEST_INT(res, 6);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 1, 1, 1, 0xFFFFFFFF).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFF);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0xFFFF, 0x10001).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFF);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0x3FFFFFF, 2, 4, 8).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFC0);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0xFFFFFFFF, 2).Succeeded());
    WD_TEST_BOOL(res == 0x1FFFFFFFE);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0x80000000, 2).Succeeded());
    WD_TEST_BOOL(res == 0x100000000);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0xFFFFFFFF, 0xFFFFFFFF).Succeeded());
    WD_TEST_BOOL(res == 0xFFFFFFFE00000001);

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0xFFFFFFFFFFFFFFFF, 2).Failed());

    res = 0;
    WD_TEST_BOOL(wdMath::TryMultiply64(res, 0xFFFFFFFF, 0xFFFFFFFF, 2).Failed());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TryConvertToSizeT")
  {
    wdUInt64 x = wdMath::MaxValue<wdUInt32>();
    wdUInt64 y = x + 1;

    size_t res = 0;

    WD_TEST_BOOL(wdMath::TryConvertToSizeT(res, x).Succeeded());
    WD_TEST_BOOL(res == x);

    res = 0;
#if WD_ENABLED(WD_PLATFORM_32BIT)
    WD_TEST_BOOL(wdMath::TryConvertToSizeT(res, y).Failed());
#else
    WD_TEST_BOOL(wdMath::TryConvertToSizeT(res, y).Succeeded());
    WD_TEST_BOOL(res == y);
#endif
  }
}
