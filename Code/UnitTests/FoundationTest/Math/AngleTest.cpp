#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Angle.h>

WD_CREATE_SIMPLE_TEST(Math, Angle)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "DegToRad")
  {
    WD_TEST_FLOAT(wdAngle::DegToRad(0.0f), 0.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(45.0f), 0.785398163f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(90.0f), 1.570796327f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(120.0f), 2.094395102f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(170.0f), 2.967059728f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(180.0f), 3.141592654f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(250.0f), 4.36332313f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(320.0f), 5.585053606f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(360.0f), 6.283185307f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(700.0f), 12.217304764f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(-123.0f), -2.14675498f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::DegToRad(-1234.0f), -21.53736297f, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RadToDeg")
  {
    WD_TEST_FLOAT(wdAngle::RadToDeg(0.0f), 0.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(0.785398163f), 45.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(1.570796327f), 90.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(2.094395102f), 120.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(2.967059728f), 170.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(3.141592654f), 180.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(4.36332313f), 250.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(5.585053606f), 320.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(6.283185307f), 360.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(12.217304764f), 700.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(-2.14675498f), -123.0f, 0.00001f);
    WD_TEST_FLOAT(wdAngle::RadToDeg(-21.53736297f), -1234.0f, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Init")
  {
    wdAngle a0;
    WD_TEST_FLOAT(a0.GetRadian(), 0.0f, 0.0f);
    WD_TEST_FLOAT(a0.GetDegree(), 0.0f, 0.0f);

    wdAngle a1 = wdAngle::Radian(1.570796327f);
    WD_TEST_FLOAT(a1.GetRadian(), 1.570796327f, 0.00001f);
    WD_TEST_FLOAT(a1.GetDegree(), 90.0f, 0.00001f);

    wdAngle a2 = wdAngle::Degree(90);
    WD_TEST_FLOAT(a2.GetRadian(), 1.570796327f, 0.00001f);
    WD_TEST_FLOAT(a2.GetDegree(), 90.0f, 0.00001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "NormalizeRange / IsEqual ")
  {
    wdAngle a;

    for (wdInt32 i = 1; i < 359; i++)
    {
      a = wdAngle::Degree((float)i);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i + 360.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i - 360.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i + 3600.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i - 3600.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i + 36000.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
      a = wdAngle::Degree((float)i - 36000.0f);
      a.NormalizeRange();
      WD_TEST_FLOAT(a.GetDegree(), (float)i, 0.01f);
    }

    for (wdInt32 i = 0; i < 360; i++)
    {
      a = wdAngle::Degree((float)i);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 360.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 360.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 3600.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 3600.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 36000.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 36000.0f);
      WD_TEST_BOOL(a.GetNormalizedRange().IsEqualSimple(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
    }

    for (wdInt32 i = 0; i < 360; i++)
    {
      a = wdAngle::Degree((float)i);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 360.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 360.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 3600.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 3600.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i + 36000.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
      a = wdAngle::Degree((float)i - 36000.0f);
      WD_TEST_BOOL(a.IsEqualNormalized(wdAngle::Degree((float)i), wdAngle::Degree(0.01f)));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "AngleBetween")
  {
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(0), wdAngle::Degree(0)).GetDegree(), 0.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(0), wdAngle::Degree(360)).GetDegree(), 0.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(360), wdAngle::Degree(360)).GetDegree(), 0.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(360), wdAngle::Degree(0)).GetDegree(), 0.0f, 0.0001f);

    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(5), wdAngle::Degree(186)).GetDegree(), 179.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(-5), wdAngle::Degree(-186)).GetDegree(), 179.0f, 0.0001f);

    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(360.0f + 5), wdAngle::Degree(360.0f + 186)).GetDegree(), 179.0f, 0.0001f);
    WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree(360.0f + -5), wdAngle::Degree(360.0f - 186)).GetDegree(), 179.0f, 0.0001f);

    for (wdInt32 i = 0; i <= 179; ++i)
      WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree((float)i), wdAngle::Degree((float)(i + i))).GetDegree(), (float)i, 0.0001f);

    for (wdInt32 i = -179; i <= 0; ++i)
      WD_TEST_FLOAT(wdAngle::AngleBetween(wdAngle::Degree((float)i), wdAngle::Degree((float)(i + i))).GetDegree(), (float)-i, 0.0001f);
  }
}
