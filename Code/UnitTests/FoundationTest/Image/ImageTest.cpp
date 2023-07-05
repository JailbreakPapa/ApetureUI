#include <FoundationTest/FoundationTestPCH.h>


#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Texture/Image/Formats/BmpFileFormat.h>
#include <Texture/Image/Formats/DdsFileFormat.h>
#include <Texture/Image/Formats/ImageFileFormat.h>
#include <Texture/Image/Image.h>
#include <Texture/Image/ImageConversion.h>
#include <Texture/Image/ImageUtils.h>

WD_CREATE_SIMPLE_TEST_GROUP(Image);

WD_CREATE_SIMPLE_TEST(Image, Image)
{
  const wdStringBuilder sReadDir(">sdk/", wdTestFramework::GetInstance()->GetRelTestDataPath());
  const wdStringBuilder sWriteDir = wdTestFramework::GetInstance()->GetAbsOutputPath();

  WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(sWriteDir) == WD_SUCCESS);

  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sReadDir, "ImageTest") == WD_SUCCESS);
  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sWriteDir, "ImageTest", "output", wdFileSystem::AllowWrites) == WD_SUCCESS);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "BMP - Good")
  {
    const char* testImagesGood[] = {
      "BMPTestImages/good/pal1", "BMPTestImages/good/pal1bg", "BMPTestImages/good/pal1wb", "BMPTestImages/good/pal4", "BMPTestImages/good/pal4rle",
      "BMPTestImages/good/pal8", "BMPTestImages/good/pal8-0", "BMPTestImages/good/pal8nonsquare",
      /*"BMPTestImages/good/pal8os2",*/ "BMPTestImages/good/pal8rle",
      /*"BMPTestImages/good/pal8topdown",*/ "BMPTestImages/good/pal8v4", "BMPTestImages/good/pal8v5", "BMPTestImages/good/pal8w124",
      "BMPTestImages/good/pal8w125", "BMPTestImages/good/pal8w126", "BMPTestImages/good/rgb16", "BMPTestImages/good/rgb16-565pal",
      "BMPTestImages/good/rgb24", "BMPTestImages/good/rgb24pal", "BMPTestImages/good/rgb32", /*"BMPTestImages/good/rgb32bf"*/
    };

    for (int i = 0; i < WD_ARRAY_SIZE(testImagesGood); i++)
    {
      wdImage image;
      {
        wdStringBuilder fileName;
        fileName.Format("{0}.bmp", testImagesGood[i]);

        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Image file does not exist: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(image.LoadFrom(fileName) == WD_SUCCESS, "Reading image failed: '%s'", fileName.GetData());
      }

      {
        wdStringBuilder fileName;
        fileName.Format(":output/{0}_out.bmp", testImagesGood[i]);

        WD_TEST_BOOL_MSG(image.SaveTo(fileName) == WD_SUCCESS, "Writing image failed: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Output image file is missing: '%s'", fileName.GetData());
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "BMP - Bad")
  {
    const char* testImagesBad[] = {"BMPTestImages/bad/badbitcount", "BMPTestImages/bad/badbitssize",
      /*"BMPTestImages/bad/baddens1", "BMPTestImages/bad/baddens2", "BMPTestImages/bad/badfilesize", "BMPTestImages/bad/badheadersize",*/
      "BMPTestImages/bad/badpalettesize",
      /*"BMPTestImages/bad/badplanes",*/ "BMPTestImages/bad/badrle", "BMPTestImages/bad/badwidth",
      /*"BMPTestImages/bad/pal2",*/ "BMPTestImages/bad/pal8badindex", "BMPTestImages/bad/reallybig", "BMPTestImages/bad/rletopdown",
      "BMPTestImages/bad/shortfile"};


    for (int i = 0; i < WD_ARRAY_SIZE(testImagesBad); i++)
    {
      wdImage image;
      {
        wdStringBuilder fileName;
        fileName.Format("{0}.bmp", testImagesBad[i]);

        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "File does not exist: '%s'", fileName.GetData());

        WD_LOG_BLOCK_MUTE();
        WD_TEST_BOOL_MSG(image.LoadFrom(fileName) == WD_FAILURE, "Reading image should have failed: '%s'", fileName.GetData());
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TGA")
  {
    const char* testImagesGood[] = {"TGATestImages/good/RGB", "TGATestImages/good/RGBA", "TGATestImages/good/RGB_RLE", "TGATestImages/good/RGBA_RLE"};

    for (int i = 0; i < WD_ARRAY_SIZE(testImagesGood); i++)
    {
      wdImage image;
      {
        wdStringBuilder fileName;
        fileName.Format("{0}.tga", testImagesGood[i]);

        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Image file does not exist: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(image.LoadFrom(fileName) == WD_SUCCESS, "Reading image failed: '%s'", fileName.GetData());
      }

      {
        wdStringBuilder fileName;
        fileName.Format(":output/{0}_out.bmp", testImagesGood[i]);

        wdStringBuilder fileNameExpected;
        fileNameExpected.Format("{0}_expected.bmp", testImagesGood[i]);

        WD_TEST_BOOL_MSG(image.SaveTo(fileName) == WD_SUCCESS, "Writing image failed: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Output image file is missing: '%s'", fileName.GetData());

        WD_TEST_FILES(fileName, fileNameExpected, "");
      }

      {
        wdStringBuilder fileName;
        fileName.Format(":output/{0}_out.tga", testImagesGood[i]);

        wdStringBuilder fileNameExpected;
        fileNameExpected.Format("{0}_expected.tga", testImagesGood[i]);

        WD_TEST_BOOL_MSG(image.SaveTo(fileName) == WD_SUCCESS, "Writing image failed: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Output image file is missing: '%s'", fileName.GetData());

        WD_TEST_FILES(fileName, fileNameExpected, "");
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Write Image Formats")
  {
    struct ImgTest
    {
      const char* szImage;
      const char* szFormat;
      wdUInt32 uiMSE;
    };

    ImgTest imgTests[] = {
      {"RGB", "tga", 0},
      {"RGBA", "tga", 0},
      {"RGB", "png", 0},
      {"RGBA", "png", 0},
      {"RGB", "jpg", 4650},
      {"RGBA", "jpeg", 16670},
#if WD_ENABLED(WD_PLATFORM_WINDOWS_DESKTOP)
      {"RGB", "tif", 0},
      {"RGBA", "tif", 0},
#endif
    };

    const char* szTestImagePath = "TGATestImages/good";

    for (int idx = 0; idx < WD_ARRAY_SIZE(imgTests); ++idx)
    {
      wdImage image;
      {
        wdStringBuilder fileName;
        fileName.Format("{}/{}.tga", szTestImagePath, imgTests[idx].szImage);

        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Image file does not exist: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(image.LoadFrom(fileName) == WD_SUCCESS, "Reading image failed: '%s'", fileName.GetData());
      }

      {
        wdStringBuilder fileName;
        fileName.Format(":output/WriteImageTest/{}.{}", imgTests[idx].szImage, imgTests[idx].szFormat);

        wdFileSystem::DeleteFile(fileName);

        WD_TEST_BOOL_MSG(image.SaveTo(fileName) == WD_SUCCESS, "Writing image failed: '%s'", fileName.GetData());
        WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "Output image file is missing: '%s'", fileName.GetData());

        wdImage image2;
        WD_TEST_BOOL_MSG(image2.LoadFrom(fileName).Succeeded(), "Reading written image failed: '%s'", fileName.GetData());

        image.Convert(wdImageFormat::R8G8B8A8_UNORM_SRGB).IgnoreResult();
        image2.Convert(wdImageFormat::R8G8B8A8_UNORM_SRGB).IgnoreResult();

        wdImage diff;
        wdImageUtils::ComputeImageDifferenceABS(image, image2, diff);

        const wdUInt32 uiMSE = wdImageUtils::ComputeMeanSquareError(diff, 32);

        WD_TEST_BOOL_MSG(uiMSE <= imgTests[idx].uiMSE, "MSE %u is larger than %u for image '%s'", uiMSE, imgTests[idx].uiMSE, fileName.GetData());
      }
    }
  }

  wdFileSystem::RemoveDataDirectoryGroup("ImageTest");
}
