#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/SimdMath/SimdNoise.h>
#include <Foundation/SimdMath/SimdRandom.h>
#include <Texture/Image/Image.h>

WD_CREATE_SIMPLE_TEST(SimdMath, SimdNoise)
{
  wdStringBuilder sReadDir(">sdk/", wdTestFramework::GetInstance()->GetRelTestDataPath());
  wdStringBuilder sWriteDir = wdTestFramework::GetInstance()->GetAbsOutputPath();

  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sReadDir, "SimdNoise") == WD_SUCCESS);
  WD_TEST_BOOL_MSG(wdFileSystem::AddDataDirectory(sWriteDir, "SimdNoise", "output", wdFileSystem::AllowWrites) == WD_SUCCESS,
    "Failed to mount data dir '%s'", sWriteDir.GetData());

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Perlin")
  {
    const wdUInt32 uiSize = 128;

    wdImageHeader imageHeader;
    imageHeader.SetWidth(uiSize);
    imageHeader.SetHeight(uiSize);
    imageHeader.SetImageFormat(wdImageFormat::R8G8B8A8_UNORM);

    wdImage image;
    image.ResetAndAlloc(imageHeader);

    wdSimdPerlinNoise perlin(12345);
    wdSimdVec4f xOffset(0, 1, 2, 3);
    wdSimdFloat scale(100);

    for (wdUInt32 uiNumOctaves = 1; uiNumOctaves <= 6; ++uiNumOctaves)
    {
      wdColorLinearUB* data = image.GetPixelPointer<wdColorLinearUB>();
      for (wdUInt32 y = 0; y < uiSize; ++y)
      {
        for (wdUInt32 x = 0; x < uiSize / 4; ++x)
        {
          wdSimdVec4f sX = (wdSimdVec4f(x * 4.0f) + xOffset) / scale;
          wdSimdVec4f sY = wdSimdVec4f(y * 1.0f) / scale;

          wdSimdVec4f noise = perlin.NoiseZeroToOne(sX, sY, wdSimdVec4f::ZeroVector(), uiNumOctaves);
          float p[4];
          p[0] = noise.x();
          p[1] = noise.y();
          p[2] = noise.z();
          p[3] = noise.w();

          wdUInt32 uiPixelIndex = y * uiSize + x * 4;
          for (wdUInt32 i = 0; i < 4; ++i)
          {
            data[uiPixelIndex + i] = wdColor(p[i], p[i], p[i]);
          }
        }
      }

      wdStringBuilder sOutFile;
      sOutFile.Format(":output/SimdNoise/result-perlin_{}.tga", uiNumOctaves);

      WD_TEST_BOOL(image.SaveTo(sOutFile).Succeeded());

      wdStringBuilder sInFile;
      sInFile.Format("SimdNoise/perlin_{}.tga", uiNumOctaves);
      WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(sInFile), "Noise image file is missing: '%s'", sInFile.GetData());

      WD_TEST_FILES(sOutFile, sInFile, "");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Random")
  {
    wdUInt32 histogram[256] = {};

    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      wdSimdVec4u seed = wdSimdVec4u(i);
      wdSimdVec4f randomValues = wdSimdRandom::FloatMinMax(wdSimdVec4i(0, 1, 2, 3), wdSimdVec4f::ZeroVector(), wdSimdVec4f(256.0f), seed);
      wdSimdVec4i randomValuesAsInt = wdSimdVec4i::Truncate(randomValues);

      ++histogram[randomValuesAsInt.x()];
      ++histogram[randomValuesAsInt.y()];
      ++histogram[randomValuesAsInt.z()];
      ++histogram[randomValuesAsInt.w()];

      randomValues = wdSimdRandom::FloatMinMax(wdSimdVec4i(32, 33, 34, 35), wdSimdVec4f::ZeroVector(), wdSimdVec4f(256.0f), seed);
      randomValuesAsInt = wdSimdVec4i::Truncate(randomValues);

      ++histogram[randomValuesAsInt.x()];
      ++histogram[randomValuesAsInt.y()];
      ++histogram[randomValuesAsInt.z()];
      ++histogram[randomValuesAsInt.w()];
    }

    const char* szOutFile = ":output/SimdNoise/result-random.csv";
    {
      wdFileWriter fileWriter;
      WD_TEST_BOOL(fileWriter.Open(szOutFile).Succeeded());

      wdStringBuilder sLine;
      for (wdUInt32 i = 0; i < WD_ARRAY_SIZE(histogram); ++i)
      {
        sLine.Format("{},\n", histogram[i]);
        fileWriter.WriteBytes(sLine.GetData(), sLine.GetElementCount()).IgnoreResult();
      }
    }

    const char* szInFile = "SimdNoise/random.csv";
    WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(szInFile), "Random histogram file is missing: '%s'", szInFile);

    WD_TEST_TEXT_FILES(szOutFile, szInFile, "");
  }

  wdFileSystem::RemoveDataDirectoryGroup("SimdNoise");
}
