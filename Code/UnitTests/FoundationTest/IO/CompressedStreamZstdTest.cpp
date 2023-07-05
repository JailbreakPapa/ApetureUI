#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/CompressedStreamZstd.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/Stream.h>

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT

WD_CREATE_SIMPLE_TEST(IO, CompressedStreamZstd)
{
  wdDynamicArray<wdUInt32> TestData;

  // create the test data
  // a repetition of a counting sequence that is getting longer and longer, ie:
  // 0, 0,1, 0,1,2, 0,1,2,3, 0,1,2,3,4, ...
  {
    TestData.SetCountUninitialized(1024 * 1024 * 8);

    const wdUInt32 uiItems = TestData.GetCount();
    wdUInt32 uiStartPos = 0;

    for (wdUInt32 uiWrite = 1; uiWrite < uiItems; ++uiWrite)
    {
      uiWrite = wdMath::Min(uiWrite, uiItems - uiStartPos);

      if (uiWrite == 0)
        break;

      for (wdUInt32 i = 0; i < uiWrite; ++i)
      {
        TestData[uiStartPos + i] = i;
      }

      uiStartPos += uiWrite;
    }
  }


  wdDefaultMemoryStreamStorage StreamStorage;

  wdMemoryStreamWriter MemoryWriter(&StreamStorage);
  wdMemoryStreamReader MemoryReader(&StreamStorage);

  wdCompressedStreamReaderZstd CompressedReader;
  wdCompressedStreamWriterZstd CompressedWriter;

  const float fExpectedCompressionRatio = 900.0f; // this is a guess that is based on the current input data and size

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compress Data")
  {
    CompressedWriter.SetOutputStream(&MemoryWriter);

    bool bFlush = true;

    wdUInt32 uiWrite = 1;
    for (wdUInt32 i = 0; i < TestData.GetCount();)
    {
      uiWrite = wdMath::Min<wdUInt32>(uiWrite, TestData.GetCount() - i);

      WD_TEST_BOOL(CompressedWriter.WriteBytes(&TestData[i], sizeof(wdUInt32) * uiWrite) == WD_SUCCESS);

      if (bFlush)
      {
        // this actually hurts compression rates
        WD_TEST_BOOL(CompressedWriter.Flush() == WD_SUCCESS);
      }

      bFlush = !bFlush;

      i += uiWrite;
      uiWrite += 17; // try different sizes to write
    }

    // flush all data
    CompressedWriter.FinishCompressedStream().AssertSuccess();

    const wdUInt64 uiCompressed = CompressedWriter.GetCompressedSize();
    const wdUInt64 uiUncompressed = CompressedWriter.GetUncompressedSize();
    const wdUInt64 uiBytesWritten = CompressedWriter.GetWrittenBytes();

    WD_TEST_INT(uiUncompressed, TestData.GetCount() * sizeof(wdUInt32));
    WD_TEST_BOOL(uiBytesWritten > uiCompressed);
    WD_TEST_BOOL(uiBytesWritten < uiUncompressed);

    const float fRatio = (float)uiUncompressed / (float)uiCompressed;
    WD_TEST_BOOL(fRatio >= fExpectedCompressionRatio);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Uncompress Data")
  {
    CompressedReader.SetInputStream(&MemoryReader);

    bool bSkip = false;
    wdUInt32 uiStartPos = 0;

    wdDynamicArray<wdUInt32> TestDataRead = TestData; // initialize with identical data, makes comparing the skipped parts easier

    // read the data in blocks that get larger and larger
    for (wdUInt32 iRead = 1; iRead < TestData.GetCount(); ++iRead)
    {
      wdUInt32 iToRead = wdMath::Min(iRead, TestData.GetCount() - uiStartPos);

      if (iToRead == 0)
        break;

      if (bSkip)
      {
        const wdUInt64 uiReadFromStream = CompressedReader.SkipBytes(sizeof(wdUInt32) * iToRead);
        WD_TEST_BOOL(uiReadFromStream == sizeof(wdUInt32) * iToRead);
      }
      else
      {
        // overwrite part we are going to read from the stream, to make sure it re-reads the correct data
        for (wdUInt32 i = 0; i < iToRead; ++i)
        {
          TestDataRead[uiStartPos + i] = 0;
        }

        const wdUInt64 uiReadFromStream = CompressedReader.ReadBytes(&TestDataRead[uiStartPos], sizeof(wdUInt32) * iToRead);
        WD_TEST_BOOL(uiReadFromStream == sizeof(wdUInt32) * iToRead);
      }

      bSkip = !bSkip;

      uiStartPos += iToRead;
    }

    WD_TEST_BOOL(TestData == TestDataRead);

    // test reading after the end of the stream
    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      wdUInt32 uiTemp = 0;
      WD_TEST_BOOL(CompressedReader.ReadBytes(&uiTemp, sizeof(wdUInt32)) == 0);
    }
  }
}

#endif
