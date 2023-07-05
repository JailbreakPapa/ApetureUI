#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/StaticArray.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Math/Random.h>

// only works when also linking against CoreUtils
//#define USE_WDIMAGE

#ifdef USE_WDIMAGE
#  include <Texture/Image/Image.h>
#endif


WD_CREATE_SIMPLE_TEST(Math, Random)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "UIntInRange")
  {
    wdRandom r;
    r.Initialize(0xAABBCCDDEEFF0011ULL);

    for (wdUInt32 i = 2; i < 10000; ++i)
    {
      const wdUInt32 val = r.UIntInRange(i);
      WD_TEST_BOOL(val < i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IntInRange")
  {
    wdRandom r;
    r.Initialize(0xBBCCDDEEFF0011AAULL);

    WD_TEST_INT(r.IntInRange(5, 1), 5);
    WD_TEST_INT(r.IntInRange(-5, 1), -5);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const wdInt32 val = r.IntInRange(i, i);
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val < i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const wdInt32 val = r.IntInRange(-i, 2 * i);
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val < -i + 2 * i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IntMinMax")
  {
    wdRandom r;
    r.Initialize(0xCCDDEEFF0011AABBULL);

    WD_TEST_INT(r.IntMinMax(5, 5), 5);
    WD_TEST_INT(r.IntMinMax(-5, -5), -5);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const wdInt32 val = r.IntMinMax(i, 2 * i);
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val <= i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const wdInt32 val = r.IntMinMax(-i, i);
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val <= i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Bool")
  {
    wdRandom r;
    r.Initialize(0x11AABBCCDDEEFFULL);

    wdUInt32 falseCount = 0;
    wdUInt32 trueCount = 0;
    wdDynamicArray<bool> values;
    values.SetCount(1000);

    for (int i = 0; i < 1000; ++i)
    {
      values[i] = r.Bool();
      if (values[i])
      {
        ++trueCount;
      }
      else
      {
        ++falseCount;
      }
    }

    // This could be more elaborate, one could also test the variance
    // and assert that approximately an uniform distribution is yielded
    WD_TEST_BOOL(trueCount > 0 && falseCount > 0);

    wdRandom r2;
    r2.Initialize(0x11AABBCCDDEEFFULL);

    for (int i = 0; i < 1000; ++i)
    {
      WD_TEST_BOOL(values[i] == r2.Bool());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DoubleZeroToOneExclusive")
  {
    wdRandom r;
    r.Initialize(0xDDEEFF0011AABBCCULL);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleZeroToOneExclusive();
      WD_TEST_BOOL(val >= 0.0);
      WD_TEST_BOOL(val < 1.0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DoubleZeroToOneInclusive")
  {
    wdRandom r;
    r.Initialize(0xEEFF0011AABBCCDDULL);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleZeroToOneInclusive();
      WD_TEST_BOOL(val >= 0.0);
      WD_TEST_BOOL(val <= 1.0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DoubleInRange")
  {
    wdRandom r;
    r.Initialize(0xFF0011AABBCCDDEEULL);

    WD_TEST_DOUBLE(r.DoubleInRange(5, 0), 5, 0.0);
    WD_TEST_DOUBLE(r.DoubleInRange(-5, 0), -5, 0.0);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleInRange(i, i);
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val < i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleInRange(-i, 2 * i);
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val < -i + 2 * i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "DoubleMinMax")
  {
    wdRandom r;
    r.Initialize(0x0011AABBCCDDEEFFULL);

    WD_TEST_DOUBLE(r.DoubleMinMax(5, 5), 5, 0.0);
    WD_TEST_DOUBLE(r.DoubleMinMax(-5, -5), -5, 0.0);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleMinMax(i, 2 * i);
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val <= i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const double val = r.DoubleMinMax(-i, i);
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val <= i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FloatZeroToOneExclusive")
  {
    wdRandom r;
    r.Initialize(0xDDEEFF0011AABBCCULL);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatZeroToOneExclusive();
      WD_TEST_BOOL(val >= 0.f);
      WD_TEST_BOOL(val < 1.f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FloatZeroToOneInclusive")
  {
    wdRandom r;
    r.Initialize(0xEEFF0011AABBCCDDULL);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatZeroToOneInclusive();
      WD_TEST_BOOL(val >= 0.f);
      WD_TEST_BOOL(val <= 1.f);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FloatInRange")
  {
    wdRandom r;
    r.Initialize(0xFF0011AABBCCDDEEULL);

    WD_TEST_FLOAT(r.FloatInRange(5, 0), 5, 0.f);
    WD_TEST_FLOAT(r.FloatInRange(-5, 0), -5, 0.f);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatInRange(static_cast<float>(i), static_cast<float>(i));
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val < i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatInRange(static_cast<float>(-i), 2 * static_cast<float>(i));
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val < -i + 2 * i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FloatMinMax")
  {
    wdRandom r;
    r.Initialize(0x0011AABBCCDDEEFFULL);

    WD_TEST_FLOAT(r.FloatMinMax(5, 5), 5, 0.f);
    WD_TEST_FLOAT(r.FloatMinMax(-5, -5), -5, 0.f);

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatMinMax(static_cast<float>(i), static_cast<float>(2 * i));
      WD_TEST_BOOL(val >= i);
      WD_TEST_BOOL(val <= i + i);
    }

    for (wdInt32 i = 2; i < 10000; ++i)
    {
      const float val = r.FloatMinMax(static_cast<float>(-i), static_cast<float>(i));
      WD_TEST_BOOL(val >= -i);
      WD_TEST_BOOL(val <= i);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Save / Load")
  {
    wdRandom r, r2;
    r.Initialize(0x0011AABBCCDDE11FULL);

    for (int i = 0; i < 1000; ++i)
      r.UInt();

    wdDefaultMemoryStreamStorage storage;
    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    r.Save(writer);

    wdDynamicArray<wdUInt32> temp;
    temp.SetCountUninitialized(1000);

    for (int i = 0; i < 1000; ++i)
      temp[i] = r.UInt();

    r2.Load(reader);

    for (int i = 0; i < 1000; ++i)
    {
      WD_TEST_INT(temp[i], r2.UInt());
    }
  }
}

static void SaveToImage(wdDynamicArray<wdUInt32>& ref_values, wdUInt32 uiMaxValue, const char* szFile)
{
#ifdef USE_WDIMAGE
  WD_TEST_BOOL(wdFileSystem::AddDataDirectory("", wdFileSystem::AllowWrites, "Clear") == WD_SUCCESS);

  wdImage img;
  img.SetWidth(Values.GetCount());
  img.SetHeight(100);
  img.SetImageFormat(wdImageFormat::B8G8R8A8_UNORM);
  img.AllocateImageData();

  for (wdUInt32 y = 0; y < img.GetHeight(); ++y)
  {
    for (wdUInt32 x = 0; x < img.GetWidth(); ++x)
    {
      wdUInt32* pPixel = img.GetPixelPointer<wdUInt32>(0, 0, 0, x, y);
      *pPixel = 0xFF000000;
    }
  }

  for (wdUInt32 i = 0; i < Values.GetCount(); ++i)
  {
    double val = ((double)Values[i] / (double)uiMaxValue) * 100.0;
    wdUInt32 y = 99 - wdMath::Clamp<wdUInt32>((wdUInt32)val, 0, 99);

    wdUInt32* pPixel = img.GetPixelPointer<wdUInt32>(0, 0, 0, i, y);
    *pPixel = 0xFFFFFFFF;
  }

  img.SaveTo(szFile);

  wdFileSystem::RemoveDataDirectoryGroup("Clear");
#endif
}

WD_CREATE_SIMPLE_TEST(Math, RandomGauss)
{
  const float fVariance = 1.0f;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UnsignedValue")
  {
    wdRandomGauss r;
    r.Initialize(0xABCDEF0012345678ULL, 100, fVariance);

    wdDynamicArray<wdUInt32> Values;
    Values.SetCount(100);

    wdUInt32 uiMaxValue = 0;

    const wdUInt32 factor = 10; // with a factor of 100 the bell curve becomes more pronounced, with less samples it has more exceptions
    for (wdUInt32 i = 0; i < 10000 * factor; ++i)
    {
      auto val = r.UnsignedValue();

      WD_TEST_BOOL(val < 100);

      if (val < Values.GetCount())
      {
        Values[val]++;

        uiMaxValue = wdMath::Max(uiMaxValue, Values[val]);
      }
    }

    SaveToImage(Values, uiMaxValue, "D:/GaussUnsigned.tga");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SignedValue")
  {
    wdRandomGauss r;
    r.Initialize(0xABCDEF0012345678ULL, 100, fVariance);

    wdDynamicArray<wdUInt32> Values;
    Values.SetCount(2 * 100);

    wdUInt32 uiMaxValue = 0;

    const wdUInt32 factor = 10; // with a factor of 100 the bell curve becomes more pronounced, with less samples it has more exceptions
    for (wdUInt32 i = 0; i < 10000 * factor; ++i)
    {
      auto val = r.SignedValue();

      WD_TEST_BOOL(val > -100 && val < 100);

      val += 100;

      if (val < (wdInt32)Values.GetCount())
      {
        Values[val]++;

        uiMaxValue = wdMath::Max(uiMaxValue, Values[val]);
      }
    }

    SaveToImage(Values, uiMaxValue, "D:/GaussSigned.tga");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Save / Load")
  {
    wdRandomGauss r, r2;
    r.Initialize(0x0011AABBCCDDE11FULL, 1000, 1.7f);

    for (int i = 0; i < 1000; ++i)
      r.UnsignedValue();

    wdDefaultMemoryStreamStorage storage;
    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    r.Save(writer);

    wdDynamicArray<wdUInt32> temp;
    temp.SetCountUninitialized(1000);

    for (int i = 0; i < 1000; ++i)
      temp[i] = r.UnsignedValue();

    r2.Load(reader);

    for (int i = 0; i < 1000; ++i)
    {
      WD_TEST_INT(temp[i], r2.UnsignedValue());
    }
  }
}
