#include <FoundationTest/FoundationTestPCH.h>


#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Memory/MemoryTracker.h>
#include <Texture/Image/Formats/BmpFileFormat.h>
#include <Texture/Image/Formats/DdsFileFormat.h>
#include <Texture/Image/Formats/ImageFileFormat.h>
#include <Texture/Image/Image.h>
#include <Texture/Image/ImageConversion.h>

static const wdImageFormat::Enum defaultFormat = wdImageFormat::R32G32B32A32_FLOAT;

class wdImageConversionTest : public wdTestBaseClass
{

public:
  virtual const char* GetTestName() const override { return "Image Conversion"; }

  virtual wdResult GetImage(wdImage& ref_img) override
  {
    ref_img.ResetAndMove(std::move(m_Image));
    return WD_SUCCESS;
  }

private:
  virtual void SetupSubTests() override
  {
    for (wdUInt32 i = 0; i < wdImageFormat::NUM_FORMATS; ++i)
    {
      wdImageFormat::Enum format = static_cast<wdImageFormat::Enum>(i);

      const char* name = wdImageFormat::GetName(format);
      WD_ASSERT_DEV(name != nullptr, "Missing format information for format {}", i);

      bool isEncodable = wdImageConversion::IsConvertible(defaultFormat, format);

      if (!isEncodable)
      {
        // If a format doesn't have an encoder, ignore
        continue;
      }

      AddSubTest(name, i);
    }
  }

  virtual wdTestAppRun RunSubTest(wdInt32 iIdentifier, wdUInt32 uiInvocationCount) override
  {
    wdImageFormat::Enum format = static_cast<wdImageFormat::Enum>(iIdentifier);

    bool isDecodable = wdImageConversion::IsConvertible(format, defaultFormat);

    if (!isDecodable)
    {
      WD_TEST_BOOL_MSG(false, "Format %s can be encoded from %s but not decoded - add a decoder for this format please", wdImageFormat::GetName(format), wdImageFormat::GetName(defaultFormat));

      return wdTestAppRun::Quit;
    }

    {
      wdHybridArray<wdImageConversion::ConversionPathNode, 16> decodingPath;
      wdUInt32 decodingPathScratchBuffers;
      wdImageConversion::BuildPath(format, defaultFormat, false, decodingPath, decodingPathScratchBuffers).IgnoreResult();

      // the [test] tag tells the test framework to output the log message in the GUI
      wdLog::Info("[test]Default decoding Path:");
      for (wdUInt32 i = 0; i < decodingPath.GetCount(); ++i)
      {
        wdLog::Info("[test]  {} -> {}", wdImageFormat::GetName(decodingPath[i].m_sourceFormat), wdImageFormat::GetName(decodingPath[i].m_targetFormat));
      }
    }

    {
      wdHybridArray<wdImageConversion::ConversionPathNode, 16> encodingPath;
      wdUInt32 encodingPathScratchBuffers;
      wdImageConversion::BuildPath(defaultFormat, format, false, encodingPath, encodingPathScratchBuffers).IgnoreResult();

      // the [test] tag tells the test framework to output the log message in the GUI
      wdLog::Info("[test]Default encoding Path:");
      for (wdUInt32 i = 0; i < encodingPath.GetCount(); ++i)
      {
        wdLog::Info("[test]  {} -> {}", wdImageFormat::GetName(encodingPath[i].m_sourceFormat), wdImageFormat::GetName(encodingPath[i].m_targetFormat));
      }
    }

    // Test LDR: Load, encode to target format, then do image comparison (which internally decodes to BGR8_UNORM again).
    // This visualizes quantization for low bit formats, block compression artifacts, or whether formats have fewer than 3 channels.
    {
      WD_TEST_BOOL(m_Image.LoadFrom("ImageConversions/reference.png").Succeeded());

      WD_TEST_BOOL(m_Image.Convert(format).Succeeded());

      WD_TEST_IMAGE(iIdentifier * 2, wdImageFormat::IsCompressed(format) ? 10 : 0);
    }

    // Test HDR: Load, decode to FLOAT32, stretch to [-range, range] and encode;
    // then decode to FLOAT32 again, bring back into LDR range and do image comparison.
    // If the format doesn't support negative values, the left half of the image will be black.
    // If the format doesn't support values with absolute value > 1, the image will appear clipped to fullbright.
    // Also, fill the first few rows in the top left with Infinity, -Infinity, and NaN, which should
    // show up as White, White, and Black, resp., in the comparison.
    {
      const float range = 8;

      WD_TEST_BOOL(m_Image.LoadFrom("ImageConversions/reference.png").Succeeded());

      WD_TEST_BOOL(m_Image.Convert(wdImageFormat::R32G32B32A32_FLOAT).Succeeded());

      float posInf = +wdMath::Infinity<float>();
      float negInf = -wdMath::Infinity<float>();
      float NaN = wdMath::NaN<float>();

      for (wdUInt32 y = 0; y < m_Image.GetHeight(); ++y)
      {
        wdColor* pPixelPointer = m_Image.GetPixelPointer<wdColor>(0, 0, 0, 0, y);

        for (wdUInt32 x = 0; x < m_Image.GetWidth(); ++x)
        {
          // Fill with Inf or Nan resp. scale the image into positive and negative HDR range
          if (x < 30 && y < 10)
          {
            *pPixelPointer = wdColor(posInf, posInf, posInf, posInf);
          }
          else if (x < 30 && y < 20)
          {
            *pPixelPointer = wdColor(negInf, negInf, negInf, negInf);
          }
          else if (x < 30 && y < 30)
          {
            *pPixelPointer = wdColor(NaN, NaN, NaN, NaN);
          }
          else
          {
            float scale = (x / float(m_Image.GetWidth()) - 0.5f) * 2.0f * range;

            if (wdMath::Abs(scale) > 0.5)
            {
              *pPixelPointer *= scale;
            }
          }

          pPixelPointer++;
        }
      }

      WD_TEST_BOOL(m_Image.Convert(format).Succeeded());

      WD_TEST_BOOL(m_Image.Convert(wdImageFormat::R32G32B32A32_FLOAT).Succeeded());

      for (wdUInt32 y = 0; y < m_Image.GetHeight(); ++y)
      {
        wdColor* pPixelPointer = m_Image.GetPixelPointer<wdColor>(0, 0, 0, 0, y);

        for (wdUInt32 x = 0; x < m_Image.GetWidth(); ++x)
        {
          // Scale the image back into LDR range if possible
          if (x < 30 && y < 10)
          {
            // Leave pos inf as is - this should be clipped to 1 in the LDR conversion for img cmp
          }
          else if (x < 30 && y < 20)
          {
            // Flip neg inf to pos inf
            *pPixelPointer *= -1.0f;
          }
          else if (x < 30 && y < 30)
          {
            // Leave nan as is - this should be clipped to 0 in the LDR conversion for img cmp
          }
          else
          {
            float scale = (x / float(m_Image.GetWidth()) - 0.5f) * 2.0f * range;
            if (wdMath::Abs(scale) > 0.5)
            {
              *pPixelPointer /= scale;
            }
          }

          pPixelPointer++;
        }
      }

      WD_TEST_IMAGE(iIdentifier * 2 + 1, wdImageFormat::IsCompressed(format) ? 10 : 0);
    }

    return wdTestAppRun::Quit;
  }

  virtual wdResult InitializeTest() override
  {
    wdStartup::StartupCoreSystems();

    const wdStringBuilder sReadDir(">sdk/", wdTestFramework::GetInstance()->GetRelTestDataPath());

    if (wdFileSystem::AddDataDirectory(sReadDir.GetData(), "ImageConversionTest").Failed())
    {
      return WD_FAILURE;
    }

    wdFileSystem::AddDataDirectory(">wdtest/", "ImageComparisonDataDir", "imgout", wdFileSystem::AllowWrites).IgnoreResult();

#if WD_ENABLED(WD_PLATFORM_LINUX)
    // On linux we use CPU based BC6 and BC7 compression, which sometimes gives slightly different results from the GPU compression on Windows.
    wdTestFramework::GetInstance()->SetImageReferenceOverrideFolderName("Images_Reference_Linux");
#endif

    return WD_SUCCESS;
  }

  virtual wdResult DeInitializeTest() override
  {
    wdFileSystem::RemoveDataDirectoryGroup("ImageConversionTest");
    wdFileSystem::RemoveDataDirectoryGroup("ImageComparisonDataDir");

    wdStartup::ShutdownCoreSystems();
    wdMemoryTracker::DumpMemoryLeaks();

    return WD_SUCCESS;
  }

  virtual wdResult InitializeSubTest(wdInt32 iIdentifier) override { return WD_SUCCESS; }

  virtual wdResult DeInitializeSubTest(wdInt32 iIdentifier) override { return WD_SUCCESS; }

  wdImage m_Image;
};

static wdImageConversionTest s_ImageConversionTest;
