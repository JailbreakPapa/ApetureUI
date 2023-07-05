#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Color.h>
#include <Foundation/Math/Color8UNorm.h>
#include <Foundation/Math/Mat4.h>

WD_CREATE_SIMPLE_TEST(Math, Color)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor empty")
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      // In debug the default constructor initializes everything with NaN.
      wdColor defCtor;
      WD_TEST_BOOL(wdMath::IsNaN(defCtor.r) && wdMath::IsNaN(defCtor.g) && wdMath::IsNaN(defCtor.b) && wdMath::IsNaN(defCtor.a));
    }
#else
    // Placement new of the default constructor should not have any effect on the previous data.
    float testBlock[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    wdColor* pDefCtor = ::new ((void*)&testBlock[0]) wdColor;
    WD_TEST_BOOL(pDefCtor->r == 1.0f && pDefCtor->g == 2.0f && pDefCtor->b == 3.0f && pDefCtor->a == 4.0f);
#endif

    // Make sure the class didn't accidentally change in size
    WD_TEST_BOOL(sizeof(wdColor) == sizeof(float) * 4);
  }
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor components")
  {
    wdColor init3F(0.5f, 0.6f, 0.7f);
    WD_TEST_BOOL(init3F.r == 0.5f && init3F.g == 0.6f && init3F.b == 0.7f && init3F.a == 1.0f);

    wdColor init4F(0.5f, 0.6f, 0.7f, 0.8f);
    WD_TEST_BOOL(init4F.r == 0.5f && init4F.g == 0.6f && init4F.b == 0.7f && init4F.a == 0.8f);
  }
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor copy")
  {
    wdColor init4F(0.5f, 0.6f, 0.7f, 0.8f);
    wdColor copy(init4F);
    WD_TEST_BOOL(copy.r == 0.5f && copy.g == 0.6f && copy.b == 0.7f && copy.a == 0.8f);
  }

  {
    wdColor cornflowerBlue(wdColor(0.39f, 0.58f, 0.93f));

    WD_TEST_BLOCK(wdTestBlock::Enabled, "Conversion float")
    {
      float* pFloats = cornflowerBlue.GetData();
      WD_TEST_BOOL(
        pFloats[0] == cornflowerBlue.r && pFloats[1] == cornflowerBlue.g && pFloats[2] == cornflowerBlue.b && pFloats[3] == cornflowerBlue.a);

      const float* pConstFloats = cornflowerBlue.GetData();
      WD_TEST_BOOL(pConstFloats[0] == cornflowerBlue.r && pConstFloats[1] == cornflowerBlue.g && pConstFloats[2] == cornflowerBlue.b &&
                   pConstFloats[3] == cornflowerBlue.a);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HSV conversion")
  {
    wdColor normalizedColor(0.0f, 1.0f, 0.999f, 0.0001f);
    WD_TEST_BOOL(normalizedColor.IsNormalized());
    wdColor notNormalizedColor0(-0.01f, 1.0f, 0.999f, 0.0001f);
    WD_TEST_BOOL(!notNormalizedColor0.IsNormalized());
    wdColor notNormalizedColor1(0.5f, 1.1f, 0.9f, 0.1f);
    WD_TEST_BOOL(!notNormalizedColor1.IsNormalized());
    wdColor notNormalizedColor2(0.1f, 1.0f, 1.999f, 0.1f);
    WD_TEST_BOOL(!notNormalizedColor2.IsNormalized());
    wdColor notNormalizedColor3(0.1f, 1.0f, 1.0f, -0.1f);
    WD_TEST_BOOL(!notNormalizedColor3.IsNormalized());


    // hsv test - took some samples from http://www.javascripter.net/faq/rgb2hsv.htm
    const wdColorGammaUB rgb[] = {wdColorGammaUB(255, 255, 255), wdColorGammaUB(0, 0, 0), wdColorGammaUB(123, 12, 1), wdColorGammaUB(31, 112, 153)};
    const wdVec3 hsv[] = {wdVec3(0, 0, 1), wdVec3(0, 0, 0), wdVec3(5.4f, 0.991f, 0.48f), wdVec3(200.2f, 0.797f, 0.600f)};

    for (int i = 0; i < 4; ++i)
    {
      const wdColor color = rgb[i];
      float hue, sat, val;
      color.GetHSV(hue, sat, val);

      WD_TEST_FLOAT(hue, hsv[i].x, 0.1f);
      WD_TEST_FLOAT(sat, hsv[i].y, 0.1f);
      WD_TEST_FLOAT(val, hsv[i].z, 0.1f);

      wdColor fromHSV;
      fromHSV.SetHSV(hsv[i].x, hsv[i].y, hsv[i].z);
      WD_TEST_FLOAT(fromHSV.r, color.r, 0.01f);
      WD_TEST_FLOAT(fromHSV.g, color.g, 0.01f);
      WD_TEST_FLOAT(fromHSV.b, color.b, 0.01f);
    }
  }

  {
    if (wdMath::SupportsNaN<wdMathTestType>())
    {
      float fNaN = wdMath::NaN<float>();
      const wdColor nanArray[4] = {
        wdColor(fNaN, 0.0f, 0.0f, 0.0f), wdColor(0.0f, fNaN, 0.0f, 0.0f), wdColor(0.0f, 0.0f, fNaN, 0.0f), wdColor(0.0f, 0.0f, 0.0f, fNaN)};
      const wdColor compArray[4] = {
        wdColor(1.0f, 0.0f, 0.0f, 0.0f), wdColor(0.0f, 1.0f, 0.0f, 0.0f), wdColor(0.0f, 0.0f, 1.0f, 0.0f), wdColor(0.0f, 0.0f, 0.0f, 1.0f)};


      WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNaN")
      {
        for (int i = 0; i < 4; ++i)
        {
          WD_TEST_BOOL(nanArray[i].IsNaN());
          WD_TEST_BOOL(!compArray[i].IsNaN());
        }
      }

      WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValid")
      {
        for (int i = 0; i < 4; ++i)
        {
          WD_TEST_BOOL(!nanArray[i].IsValid());
          WD_TEST_BOOL(compArray[i].IsValid());

          WD_TEST_BOOL(!(compArray[i] * wdMath::Infinity<float>()).IsValid());
          WD_TEST_BOOL(!(compArray[i] * -wdMath::Infinity<float>()).IsValid());
        }
      }
    }
  }

  {
    const wdColor op1(-4.0, 0.2f, -7.0f, -0.0f);
    const wdColor op2(2.0, 0.3f, 0.0f, 1.0f);
    const wdColor compArray[4] = {
      wdColor(1.0f, 0.0f, 0.0f, 0.0f), wdColor(0.0f, 1.0f, 0.0f, 0.0f), wdColor(0.0f, 0.0f, 1.0f, 0.0f), wdColor(0.0f, 0.0f, 0.0f, 1.0f)};

    WD_TEST_BLOCK(wdTestBlock::Enabled, "SetRGB / SetRGBA")
    {
      wdColor c1(0, 0, 0, 0);

      c1.SetRGBA(1, 2, 3, 4);

      WD_TEST_BOOL(c1 == wdColor(1, 2, 3, 4));

      c1.SetRGB(5, 6, 7);

      WD_TEST_BOOL(c1 == wdColor(5, 6, 7, 4));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdenticalRGB")
    {
      wdColor c1(0, 0, 0, 0);
      wdColor c2(0, 0, 0, 1);

      WD_TEST_BOOL(c1.IsIdenticalRGB(c2));
      WD_TEST_BOOL(!c1.IsIdenticalRGBA(c2));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsIdenticalRGBA")
    {
      WD_TEST_BOOL(op1.IsIdenticalRGBA(op1));
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(!op1.IsIdenticalRGBA(op1 + wdMath::SmallEpsilon<float>() * compArray[i]));
        WD_TEST_BOOL(!op1.IsIdenticalRGBA(op1 - wdMath::SmallEpsilon<float>() * compArray[i]));
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualRGB")
    {
      wdColor c1(0, 0, 0, 0);
      wdColor c2(0, 0, 0.2f, 1);

      WD_TEST_BOOL(!c1.IsEqualRGB(c2, 0.1f));
      WD_TEST_BOOL(c1.IsEqualRGB(c2, 0.3f));
      WD_TEST_BOOL(!c1.IsEqualRGBA(c2, 0.3f));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualRGBA")
    {
      WD_TEST_BOOL(op1.IsEqualRGBA(op1, 0.0f));
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(op1.IsEqualRGBA(op1 + wdMath::SmallEpsilon<float>() * compArray[i], 2 * wdMath::SmallEpsilon<float>()));
        WD_TEST_BOOL(op1.IsEqualRGBA(op1 - wdMath::SmallEpsilon<float>() * compArray[i], 2 * wdMath::SmallEpsilon<float>()));
        WD_TEST_BOOL(op1.IsEqualRGBA(op1 + wdMath::DefaultEpsilon<float>() * compArray[i], 2 * wdMath::DefaultEpsilon<float>()));
        WD_TEST_BOOL(op1.IsEqualRGBA(op1 - wdMath::DefaultEpsilon<float>() * compArray[i], 2 * wdMath::DefaultEpsilon<float>()));
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+= (wdColor)")
    {
      wdColor plusAssign = op1;
      plusAssign += op2;
      WD_TEST_BOOL(plusAssign.IsEqualRGBA(wdColor(-2.0f, 0.5f, -7.0f, 1.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator-= (wdColor)")
    {
      wdColor minusAssign = op1;
      minusAssign -= op2;
      WD_TEST_BOOL(minusAssign.IsEqualRGBA(wdColor(-6.0f, -0.1f, -7.0f, -1.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "ooperator*= (float)")
    {
      wdColor mulFloat = op1;
      mulFloat *= 2.0f;
      WD_TEST_BOOL(mulFloat.IsEqualRGBA(wdColor(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<float>()));
      mulFloat *= 0.0f;
      WD_TEST_BOOL(mulFloat.IsEqualRGBA(wdColor(0.0f, 0.0f, 0.0f, 0.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/= (float)")
    {
      wdColor vDivFloat = op1;
      vDivFloat /= 2.0f;
      WD_TEST_BOOL(vDivFloat.IsEqualRGBA(wdColor(-2.0f, 0.1f, -3.5f, -0.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+ (wdColor, wdColor)")
    {
      wdColor plus = (op1 + op2);
      WD_TEST_BOOL(plus.IsEqualRGBA(wdColor(-2.0f, 0.5f, -7.0f, 1.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator- (wdColor, wdColor)")
    {
      wdColor minus = (op1 - op2);
      WD_TEST_BOOL(minus.IsEqualRGBA(wdColor(-6.0f, -0.1f, -7.0f, -1.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator* (float, wdColor)")
    {
      wdColor mulFloatVec4 = 2 * op1;
      WD_TEST_BOOL(mulFloatVec4.IsEqualRGBA(wdColor(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<float>()));
      mulFloatVec4 = ((float)0 * op1);
      WD_TEST_BOOL(mulFloatVec4.IsEqualRGBA(wdColor(0.0f, 0.0f, 0.0f, 0.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator* (wdColor, float)")
    {
      wdColor mulVec4Float = op1 * 2;
      WD_TEST_BOOL(mulVec4Float.IsEqualRGBA(wdColor(-8.0f, 0.4f, -14.0f, -0.0f), wdMath::SmallEpsilon<float>()));
      mulVec4Float = (op1 * (float)0);
      WD_TEST_BOOL(mulVec4Float.IsEqualRGBA(wdColor(0.0f, 0.0f, 0.0f, 0.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator/ (wdColor, float)")
    {
      wdColor vDivVec4Float = op1 / 2;
      WD_TEST_BOOL(vDivVec4Float.IsEqualRGBA(wdColor(-2.0f, 0.1f, -3.5f, -0.0f), wdMath::SmallEpsilon<float>()));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== (wdColor, wdColor)")
    {
      WD_TEST_BOOL(op1 == op1);
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(!(op1 == (op1 + wdMath::SmallEpsilon<float>() * compArray[i])));
        WD_TEST_BOOL(!(op1 == (op1 - wdMath::SmallEpsilon<float>() * compArray[i])));
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator< (wdColor, wdColor)")
    {
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

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator!= (wdColor, wdColor)")
    {
      WD_TEST_BOOL(!(op1 != op1));
      for (int i = 0; i < 4; ++i)
      {
        WD_TEST_BOOL(op1 != (op1 + wdMath::SmallEpsilon<float>() * compArray[i]));
        WD_TEST_BOOL(op1 != (op1 - wdMath::SmallEpsilon<float>() * compArray[i]));
      }
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator= (wdColorLinearUB)")
    {
      wdColor c;
      wdColorLinearUB lin(50, 100, 150, 255);

      c = lin;

      WD_TEST_FLOAT(c.r, 50 / 255.0f, 0.001f);
      WD_TEST_FLOAT(c.g, 100 / 255.0f, 0.001f);
      WD_TEST_FLOAT(c.b, 150 / 255.0f, 0.001f);
      WD_TEST_FLOAT(c.a, 1.0f, 0.001f);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator= (wdColorGammaUB) / constructor(wdColorGammaUB)")
    {
      wdColor c;
      wdColorGammaUB gamma(50, 100, 150, 255);

      c = gamma;
      wdColor c3 = gamma;

      WD_TEST_BOOL(c == c3);

      WD_TEST_FLOAT(c.r, 0.031f, 0.001f);
      WD_TEST_FLOAT(c.g, 0.127f, 0.001f);
      WD_TEST_FLOAT(c.b, 0.304f, 0.001f);
      WD_TEST_FLOAT(c.a, 1.0f, 0.001f);

      wdColorGammaUB c2 = c;

      WD_TEST_INT(c2.r, 50);
      WD_TEST_INT(c2.g, 100);
      WD_TEST_INT(c2.b, 150);
      WD_TEST_INT(c2.a, 255);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetInvertedColor")
    {
      const wdColor c1(0.1f, 0.3f, 0.7f, 0.9f);

      wdColor c2 = c1.GetInvertedColor();

      WD_TEST_BOOL(c2.IsEqualRGBA(wdColor(0.9f, 0.7f, 0.3f, 0.1f), 0.01f));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLuminance")
    {
      WD_TEST_FLOAT(wdColor::Black.GetLuminance(), 0.0f, 0.001f);
      WD_TEST_FLOAT(wdColor::White.GetLuminance(), 1.0f, 0.001f);

      WD_TEST_FLOAT(wdColor(0.5f, 0.5f, 0.5f).GetLuminance(), 0.2126f * 0.5f + 0.7152f * 0.5f + 0.0722f * 0.5f, 0.001f);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetComplementaryColor")
    {
      // black and white have no complementary colors, or rather, they are their own complementary colors, apparently
      WD_TEST_BOOL(wdColor::Black.GetComplementaryColor().IsEqualRGBA(wdColor::Black, 0.001f));
      WD_TEST_BOOL(wdColor::White.GetComplementaryColor().IsEqualRGBA(wdColor::White, 0.001f));

      WD_TEST_BOOL(wdColor::Red.GetComplementaryColor().IsEqualRGBA(wdColor::Cyan, 0.001f));
      WD_TEST_BOOL(wdColor::Lime.GetComplementaryColor().IsEqualRGBA(wdColor::Magenta, 0.001f));
      WD_TEST_BOOL(wdColor::Blue.GetComplementaryColor().IsEqualRGBA(wdColor::Yellow, 0.001f));
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "GetSaturation")
    {
      WD_TEST_FLOAT(wdColor::Black.GetSaturation(), 0.0f, 0.001f);
      WD_TEST_FLOAT(wdColor::White.GetSaturation(), 0.0f, 0.001f);
      WD_TEST_FLOAT(wdColor::Red.GetSaturation(), 1.0f, 0.001f);
      WD_TEST_FLOAT(wdColor::Lime.GetSaturation(), 1.0f, 0.001f);
      ;
      WD_TEST_FLOAT(wdColor::Blue.GetSaturation(), 1.0f, 0.001f);
    }

    WD_TEST_BLOCK(wdTestBlock::Enabled, "operator * / *= (wdMat4)")
    {
      wdMat4 m;
      m.SetIdentity();
      m.SetScalingMatrix(wdVec3(0.5f, 0.75f, 0.25f));
      m.SetTranslationVector(wdVec3(0.1f, 0.2f, 0.3f));

      wdColor c1 = m * wdColor::White;

      WD_TEST_BOOL(c1.IsEqualRGBA(wdColor(0.6f, 0.95f, 0.55f, 1.0f), 0.01f));
    }
  }
}
