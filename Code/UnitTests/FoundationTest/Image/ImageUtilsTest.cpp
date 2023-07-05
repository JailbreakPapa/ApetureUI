#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Texture/Image/ImageUtils.h>


WD_CREATE_SIMPLE_TEST(Image, ImageUtils)
{
  wdStringBuilder sReadDir(">sdk/", wdTestFramework::GetInstance()->GetRelTestDataPath());
  wdStringBuilder sWriteDir = wdTestFramework::GetInstance()->GetAbsOutputPath();

  WD_TEST_BOOL(wdOSFile::CreateDirectoryStructure(sWriteDir.GetData()) == WD_SUCCESS);

  wdResult addDir = wdFileSystem::AddDataDirectory(sReadDir.GetData(), "ImageTest");
  WD_TEST_BOOL(addDir == WD_SUCCESS);

  if (addDir.Failed())
    return;

  addDir = wdFileSystem::AddDataDirectory(sWriteDir.GetData(), "ImageTest", "output", wdFileSystem::AllowWrites);
  WD_TEST_BOOL(addDir == WD_SUCCESS);

  if (addDir.Failed())
    return;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeImageDifferenceABS RGB")
  {
    wdImage ImageA, ImageB, ImageDiff;
    ImageA.LoadFrom("ImageUtils/ImageA_RGB.tga").IgnoreResult();
    ImageB.LoadFrom("ImageUtils/ImageB_RGB.tga").IgnoreResult();

    wdImageUtils::ComputeImageDifferenceABS(ImageA, ImageB, ImageDiff);

    ImageDiff.SaveTo(":output/ImageUtils/Diff_RGB.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedDiff_RGB.tga", "ImageUtils/Diff_RGB.tga", "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeImageDifferenceABS RGBA")
  {
    wdImage ImageA, ImageB, ImageDiff;
    ImageA.LoadFrom("ImageUtils/ImageA_RGBA.tga").IgnoreResult();
    ImageB.LoadFrom("ImageUtils/ImageB_RGBA.tga").IgnoreResult();

    wdImageUtils::ComputeImageDifferenceABS(ImageA, ImageB, ImageDiff);

    ImageDiff.SaveTo(":output/ImageUtils/Diff_RGBA.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedDiff_RGBA.tga", "ImageUtils/Diff_RGBA.tga", "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Scaledown Half RGB")
  {
    wdImage ImageA, ImageAc;
    ImageA.LoadFrom("ImageUtils/ImageA_RGB.tga").IgnoreResult();
    wdImageUtils::Scale(ImageA, ImageAc, ImageA.GetWidth() / 2, ImageA.GetHeight() / 2).IgnoreResult();

    ImageAc.SaveTo(":output/ImageUtils/ScaledHalf_RGB.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedScaledHalf_RGB.tga", "ImageUtils/ScaledHalf_RGB.tga", "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Scaledown Half RGBA")
  {
    wdImage ImageA, ImageAc;
    ImageA.LoadFrom("ImageUtils/ImageA_RGBA.tga").IgnoreResult();
    wdImageUtils::Scale(ImageA, ImageAc, ImageA.GetWidth() / 2, ImageA.GetHeight() / 2).IgnoreResult();

    ImageAc.SaveTo(":output/ImageUtils/ScaledHalf_RGBA.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedScaledHalf_RGBA.tga", "ImageUtils/ScaledHalf_RGBA.tga", "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CropImage RGB")
  {
    wdImage ImageA, ImageAc;
    ImageA.LoadFrom("ImageUtils/ImageA_RGB.tga").IgnoreResult();
    wdImageUtils::CropImage(ImageA, wdVec2I32(100, 50), wdSizeU32(300, 200), ImageAc);

    ImageAc.SaveTo(":output/ImageUtils/Crop_RGB.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedCrop_RGB.tga", "ImageUtils/Crop_RGB.tga", "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CropImage RGBA")
  {
    wdImage ImageA, ImageAc;
    ImageA.LoadFrom("ImageUtils/ImageA_RGBA.tga").IgnoreResult();
    wdImageUtils::CropImage(ImageA, wdVec2I32(100, 75), wdSizeU32(300, 180), ImageAc);

    ImageAc.SaveTo(":output/ImageUtils/Crop_RGBA.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedCrop_RGBA.tga", "ImageUtils/Crop_RGBA.tga", "");
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeMeanSquareError")
  {
    wdImage ImageA, ImageB, ImageDiff;
    ImageA.LoadFrom("ImageUtils/ImageA_RGB.tga").IgnoreResult();
    ImageB.LoadFrom("ImageUtils/ImageB_RGB.tga").IgnoreResult();

    wdImage ImageAc, ImageBc;
    wdImageUtils::Scale(ImageA, ImageAc, ImageA.GetWidth() / 2, ImageA.GetHeight() / 2).IgnoreResult();
    wdImageUtils::Scale(ImageB, ImageBc, ImageB.GetWidth() / 2, ImageB.GetHeight() / 2).IgnoreResult();

    wdImageUtils::ComputeImageDifferenceABS(ImageAc, ImageBc, ImageDiff);

    ImageDiff.SaveTo(":output/ImageUtils/MeanSquareDiff_RGB.tga").IgnoreResult();

    WD_TEST_FILES("ImageUtils/ExpectedMeanSquareDiff_RGB.tga", "ImageUtils/MeanSquareDiff_RGB.tga", "");

    wdUInt32 uiError = wdImageUtils::ComputeMeanSquareError(ImageDiff, 4);
    WD_TEST_INT(uiError, 1433);
  }

  wdFileSystem::RemoveDataDirectoryGroup("ImageTest");
}
