#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Algorithm/HashingUtils.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/IO/MemoryStream.h>

WD_CREATE_SIMPLE_TEST_GROUP(IO);

WD_CREATE_SIMPLE_TEST(IO, MemoryStream)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Memory Stream Reading / Writing")
  {
    wdDefaultMemoryStreamStorage StreamStorage;

    // Create reader
    wdMemoryStreamReader StreamReader(&StreamStorage);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    // Temp read pointer
    wdUInt8* pPointer = reinterpret_cast<wdUInt8*>(0x41); // Should crash when accessed

    // Try reading from an empty stream (should not crash, just return 0 bytes read)
    wdUInt64 uiBytesRead = StreamReader.ReadBytes(pPointer, 128);

    WD_TEST_BOOL(uiBytesRead == 0);


    // Now try writing data to the stream and reading it back
    wdUInt32 uiData[1024];
    for (wdUInt32 i = 0; i < 1024; i++)
      uiData[i] = rand();

    // Calculate the hash so we can reuse the array
    const wdUInt32 uiHashBeforeWriting = wdHashingUtils::xxHash32(uiData, sizeof(wdUInt32) * 1024);

    // Write the data
    WD_TEST_BOOL(StreamWriter.WriteBytes(reinterpret_cast<const wdUInt8*>(uiData), sizeof(wdUInt32) * 1024) == WD_SUCCESS);

    WD_TEST_BOOL(StreamWriter.GetByteCount64() == sizeof(wdUInt32) * 1024);
    WD_TEST_BOOL(StreamWriter.GetByteCount64() == StreamReader.GetByteCount64());
    WD_TEST_BOOL(StreamWriter.GetByteCount64() == StreamStorage.GetStorageSize64());


    // Clear the array for the read back
    wdMemoryUtils::ZeroFill(uiData, 1024);

    uiBytesRead = StreamReader.ReadBytes(reinterpret_cast<wdUInt8*>(uiData), sizeof(wdUInt32) * 1024);

    WD_TEST_BOOL(uiBytesRead == sizeof(wdUInt32) * 1024);

    const wdUInt32 uiHashAfterReading = wdHashingUtils::xxHash32(uiData, sizeof(wdUInt32) * 1024);

    WD_TEST_BOOL(uiHashAfterReading == uiHashBeforeWriting);

    // Modify data and test the Rewind() functionality of the writer
    uiData[0] = 0x42;
    uiData[1] = 0x23;

    const wdUInt32 uiHashOfModifiedData = wdHashingUtils::xxHash32(uiData, sizeof(wdUInt32) * 4); // Only test the first 4 elements now

    StreamWriter.SetWritePosition(0);

    StreamWriter.WriteBytes(uiData, sizeof(wdUInt32) * 4).IgnoreResult();

    // Clear the array for the read back
    wdMemoryUtils::ZeroFill(uiData, 4);

    // Test the rewind of the reader as well
    StreamReader.SetReadPosition(0);

    uiBytesRead = StreamReader.ReadBytes(uiData, sizeof(wdUInt32) * 4);

    WD_TEST_BOOL(uiBytesRead == sizeof(wdUInt32) * 4);

    const wdUInt32 uiHashAfterReadingOfModifiedData = wdHashingUtils::xxHash32(uiData, sizeof(wdUInt32) * 4);

    WD_TEST_BOOL(uiHashAfterReadingOfModifiedData == uiHashOfModifiedData);

    // Test skipping
    StreamReader.SetReadPosition(0);

    StreamReader.SkipBytes(sizeof(wdUInt32));

    wdUInt32 uiTemp;

    uiBytesRead = StreamReader.ReadBytes(&uiTemp, sizeof(wdUInt32));

    WD_TEST_BOOL(uiBytesRead == sizeof(wdUInt32));

    // We skipped over the first 0x42 element, so this should be 0x23
    WD_TEST_BOOL(uiTemp == 0x23);

    // Skip more bytes than available
    wdUInt64 uiBytesSkipped = StreamReader.SkipBytes(0xFFFFFFFFFF);

    WD_TEST_BOOL(uiBytesSkipped < 0xFFFFFFFFFF);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Raw Memory Stream Reading")
  {
    wdDynamicArray<wdUInt8> OrigStorage;
    OrigStorage.SetCountUninitialized(1000);

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      OrigStorage[i] = i % 256;
    }

    {
      wdRawMemoryStreamReader reader(OrigStorage);

      wdDynamicArray<wdUInt8> CopyStorage;
      CopyStorage.SetCountUninitialized(static_cast<wdUInt32>(reader.GetByteCount()));
      reader.ReadBytes(CopyStorage.GetData(), reader.GetByteCount());

      WD_TEST_BOOL(OrigStorage == CopyStorage);
    }

    {
      wdRawMemoryStreamReader reader(OrigStorage.GetData() + 510, 490);

      wdDynamicArray<wdUInt8> CopyStorage;
      CopyStorage.SetCountUninitialized(static_cast<wdUInt32>(reader.GetByteCount()));
      reader.ReadBytes(CopyStorage.GetData(), reader.GetByteCount());

      WD_TEST_BOOL(OrigStorage != CopyStorage);

      for (wdUInt32 i = 0; i < 490; ++i)
      {
        CopyStorage[i] = (i + 10) % 256;
      }
    }

    {
      wdRawMemoryStreamReader reader(OrigStorage.GetData(), 1000);
      reader.SkipBytes(510);

      wdDynamicArray<wdUInt8> CopyStorage;
      CopyStorage.SetCountUninitialized(490);
      reader.ReadBytes(CopyStorage.GetData(), 490);

      WD_TEST_BOOL(OrigStorage != CopyStorage);

      for (wdUInt32 i = 0; i < 490; ++i)
      {
        CopyStorage[i] = (i + 10) % 256;
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Raw Memory Stream Writing")
  {
    wdDynamicArray<wdUInt8> OrigStorage;
    OrigStorage.SetCountUninitialized(1000);

    wdRawMemoryStreamWriter writer0;
    WD_TEST_INT(writer0.GetNumWrittenBytes(), 0);
    WD_TEST_INT(writer0.GetStorageSize(), 0);

    wdRawMemoryStreamWriter writer(OrigStorage.GetData(), OrigStorage.GetCount());

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      writer << static_cast<wdUInt8>(i % 256);

      WD_TEST_INT(writer.GetNumWrittenBytes(), i + 1);
      WD_TEST_INT(writer.GetStorageSize(), 1000);
    }

    for (wdUInt32 i = 0; i < 1000; ++i)
    {
      WD_TEST_INT(OrigStorage[i], i % 256);
    }

    {
      wdRawMemoryStreamWriter writer2(OrigStorage);
      WD_TEST_INT(writer2.GetNumWrittenBytes(), 0);
      WD_TEST_INT(writer2.GetStorageSize(), 1000);
    }
  }
}

WD_CREATE_SIMPLE_TEST(IO, LargeMemoryStream)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Large Memory Stream Reading / Writing")
  {
    wdDefaultMemoryStreamStorage storage;
    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    const wdUInt8 pattern[] = {11, 10, 27, 4, 14, 3, 21, 6};

    wdUInt64 uiSize = 0;
    constexpr wdUInt64 bytesToTest = 0x8000000llu; // tested with up to 8 GB, but that just takes too long

    // writes n gigabyte
    for (wdUInt32 n = 0; n < 8; ++n)
    {
      // writes one gigabyte
      for (wdUInt32 gb = 0; gb < 1024; ++gb)
      {
        // writes one megabyte
        for (wdUInt32 mb = 0; mb < 1024 * 1024 / WD_ARRAY_SIZE(pattern); ++mb)
        {
          writer.WriteBytes(pattern, WD_ARRAY_SIZE(pattern)).IgnoreResult();
          uiSize += WD_ARRAY_SIZE(pattern);

          if (uiSize == bytesToTest)
            goto check;
        }
      }
    }

  check:
    WD_TEST_BOOL(uiSize == bytesToTest);
    WD_TEST_BOOL(writer.GetWritePosition() == bytesToTest);
    uiSize = 0;

    // reads n gigabyte
    for (wdUInt32 n = 0; n < 8; ++n)
    {
      // reads one gigabyte
      for (wdUInt32 gb = 0; gb < 1024; ++gb)
      {
        // reads one megabyte
        for (wdUInt32 mb = 0; mb < 1024 * 1024 / WD_ARRAY_SIZE(pattern); ++mb)
        {
          wdUInt8 pattern2[WD_ARRAY_SIZE(pattern)];

          const wdUInt64 uiRead = reader.ReadBytes(pattern2, WD_ARRAY_SIZE(pattern));

          if (uiRead != WD_ARRAY_SIZE(pattern))
          {
            WD_TEST_BOOL(uiRead == 0);
            WD_TEST_BOOL(uiSize == bytesToTest);
            goto endTest;
          }

          uiSize += uiRead;

          if (wdMemoryUtils::RawByteCompare(pattern, pattern2, WD_ARRAY_SIZE(pattern)) != 0)
          {
            WD_TEST_BOOL_MSG(false, "Memory read comparison failed.");
            goto endTest;
          }
        }
      }
    }

  endTest:;
    WD_TEST_BOOL(reader.GetReadPosition() == bytesToTest);
  }
}
