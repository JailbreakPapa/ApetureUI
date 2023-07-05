#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Color.h>
#include <Foundation/Math/Color8UNorm.h>


WD_CREATE_SIMPLE_TEST(Math, Color8UNorm)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor empty")
  {
    // Placement new of the default constructor should not have any effect on the previous data.
    wdUInt8 testBlock[4] = {0, 64, 128, 255};
    wdColorLinearUB* pDefCtor = ::new ((void*)&testBlock[0]) wdColorLinearUB;
    WD_TEST_BOOL(pDefCtor->r == 0 && pDefCtor->g == 64 && pDefCtor->b == 128 && pDefCtor->a == 255);

    // Make sure the class didn't accidentally change in size
    WD_TEST_BOOL(sizeof(wdColorLinearUB) == sizeof(wdUInt8) * 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor components")
  {
    wdColorLinearUB init3(100, 123, 255);
    WD_TEST_BOOL(init3.r == 100 && init3.g == 123 && init3.b == 255 && init3.a == 255);

    wdColorLinearUB init4(100, 123, 255, 42);
    WD_TEST_BOOL(init4.r == 100 && init4.g == 123 && init4.b == 255 && init4.a == 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor copy")
  {
    wdColorLinearUB init4(100, 123, 255, 42);
    wdColorLinearUB copy(init4);
    WD_TEST_BOOL(copy.r == 100 && copy.g == 123 && copy.b == 255 && copy.a == 42);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor wdColor")
  {
    wdColorLinearUB fromColor32f(wdColor(0.39f, 0.58f, 0.93f));
    WD_TEST_BOOL(wdMath::IsEqual<wdUInt8>(fromColor32f.r, static_cast<wdUInt8>(wdColor(0.39f, 0.58f, 0.93f).r * 255), 2) &&
                 wdMath::IsEqual<wdUInt8>(fromColor32f.g, static_cast<wdUInt8>(wdColor(0.39f, 0.58f, 0.93f).g * 255), 2) &&
                 wdMath::IsEqual<wdUInt8>(fromColor32f.b, static_cast<wdUInt8>(wdColor(0.39f, 0.58f, 0.93f).b * 255), 2) &&
                 wdMath::IsEqual<wdUInt8>(fromColor32f.a, static_cast<wdUInt8>(wdColor(0.39f, 0.58f, 0.93f).a * 255), 2));
  }

  // conversion
  {
    wdColorLinearUB cornflowerBlue(wdColor(0.39f, 0.58f, 0.93f));

    WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion wdColor")
    {
      wdColor color32f = cornflowerBlue;
      WD_TEST_BOOL(wdMath::IsEqual<float>(color32f.r, wdColor(0.39f, 0.58f, 0.93f).r, 2.0f / 255.0f) &&
                   wdMath::IsEqual<float>(color32f.g, wdColor(0.39f, 0.58f, 0.93f).g, 2.0f / 255.0f) &&
                   wdMath::IsEqual<float>(color32f.b, wdColor(0.39f, 0.58f, 0.93f).b, 2.0f / 255.0f) &&
                   wdMath::IsEqual<float>(color32f.a, wdColor(0.39f, 0.58f, 0.93f).a, 2.0f / 255.0f));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion wdUInt*")
    {
      const wdUInt8* pUIntsConst = cornflowerBlue.GetData();
      WD_TEST_BOOL(pUIntsConst[0] == cornflowerBlue.r && pUIntsConst[1] == cornflowerBlue.g && pUIntsConst[2] == cornflowerBlue.b &&
                   pUIntsConst[3] == cornflowerBlue.a);

      wdUInt8* pUInts = cornflowerBlue.GetData();
      WD_TEST_BOOL(pUInts[0] == cornflowerBlue.r && pUInts[1] == cornflowerBlue.g && pUInts[2] == cornflowerBlue.b && pUInts[3] == cornflowerBlue.a);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdColorGammaUB: Constructor")
  {
    wdColorGammaUB c(50, 150, 200, 100);
    WD_TEST_INT(c.r, 50);
    WD_TEST_INT(c.g, 150);
    WD_TEST_INT(c.b, 200);
    WD_TEST_INT(c.a, 100);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdColorGammaUB: Constructor (wdColor)")
  {
    wdColorGammaUB c2 = wdColor::RebeccaPurple;

    wdColor c3 = c2;

    WD_TEST_BOOL(c3.IsEqualRGBA(wdColor::RebeccaPurple, 0.001f));
  }
}
