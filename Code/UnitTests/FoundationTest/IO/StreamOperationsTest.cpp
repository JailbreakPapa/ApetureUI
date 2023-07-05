#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/StaticArray.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/Stream.h>

#include <Foundation/IO/StringDeduplicationContext.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Stopwatch.h>

namespace
{
  struct SerializableStructWithMethods
  {

    WD_DECLARE_POD_TYPE();

    wdResult Serialize(wdStreamWriter& inout_stream) const
    {
      inout_stream << m_uiMember1;
      inout_stream << m_uiMember2;

      return WD_SUCCESS;
    }

    wdResult Deserialize(wdStreamReader& inout_stream)
    {
      inout_stream >> m_uiMember1;
      inout_stream >> m_uiMember2;

      return WD_SUCCESS;
    }

    wdInt32 m_uiMember1 = 0x42;
    wdInt32 m_uiMember2 = 0x23;
  };
} // namespace

WD_CREATE_SIMPLE_TEST(IO, StreamOperation)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Binary Stream Basic Operations (built-in types)")
  {
    wdDefaultMemoryStreamStorage StreamStorage(4096);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    StreamWriter << (wdUInt8)0x42;
    StreamWriter << (wdUInt16)0x4223;
    StreamWriter << (wdUInt32)0x42232342;
    StreamWriter << (wdUInt64)0x4223234242232342;
    StreamWriter << 42.0f;
    StreamWriter << 23.0;
    StreamWriter << (wdInt8)0x23;
    StreamWriter << (wdInt16)0x2342;
    StreamWriter << (wdInt32)0x23422342;
    StreamWriter << (wdInt64)0x2342234242232342;

    // Arrays
    {
      wdDynamicArray<wdUInt32> DynamicArray;
      DynamicArray.PushBack(42);
      DynamicArray.PushBack(23);
      DynamicArray.PushBack(13);
      DynamicArray.PushBack(5);
      DynamicArray.PushBack(0);

      StreamWriter.WriteArray(DynamicArray).IgnoreResult();
    }

    // Create reader
    wdMemoryStreamReader StreamReader(&StreamStorage);

    // Read back
    {
      wdUInt8 uiVal;
      StreamReader >> uiVal;
      WD_TEST_BOOL(uiVal == (wdUInt8)0x42);
    }
    {
      wdUInt16 uiVal;
      StreamReader >> uiVal;
      WD_TEST_BOOL(uiVal == (wdUInt16)0x4223);
    }
    {
      wdUInt32 uiVal;
      StreamReader >> uiVal;
      WD_TEST_BOOL(uiVal == (wdUInt32)0x42232342);
    }
    {
      wdUInt64 uiVal;
      StreamReader >> uiVal;
      WD_TEST_BOOL(uiVal == (wdUInt64)0x4223234242232342);
    }

    {
      float fVal;
      StreamReader >> fVal;
      WD_TEST_BOOL(fVal == 42.0f);
    }
    {
      double dVal;
      StreamReader >> dVal;
      WD_TEST_BOOL(dVal == 23.0f);
    }


    {
      wdInt8 iVal;
      StreamReader >> iVal;
      WD_TEST_BOOL(iVal == (wdInt8)0x23);
    }
    {
      wdInt16 iVal;
      StreamReader >> iVal;
      WD_TEST_BOOL(iVal == (wdInt16)0x2342);
    }
    {
      wdInt32 iVal;
      StreamReader >> iVal;
      WD_TEST_BOOL(iVal == (wdInt32)0x23422342);
    }
    {
      wdInt64 iVal;
      StreamReader >> iVal;
      WD_TEST_BOOL(iVal == (wdInt64)0x2342234242232342);
    }

    {
      wdDynamicArray<wdUInt32> ReadBackDynamicArray;

      // This element will be removed by the ReadArray function
      ReadBackDynamicArray.PushBack(0xAAu);

      StreamReader.ReadArray(ReadBackDynamicArray).IgnoreResult();

      WD_TEST_INT(ReadBackDynamicArray.GetCount(), 5);

      WD_TEST_INT(ReadBackDynamicArray[0], 42);
      WD_TEST_INT(ReadBackDynamicArray[1], 23);
      WD_TEST_INT(ReadBackDynamicArray[2], 13);
      WD_TEST_INT(ReadBackDynamicArray[3], 5);
      WD_TEST_INT(ReadBackDynamicArray[4], 0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Binary Stream Arrays of Structs")
  {
    wdDefaultMemoryStreamStorage StreamStorage(4096);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    // Write out a couple of the structs
    {
      wdStaticArray<SerializableStructWithMethods, 16> WriteArray;
      WriteArray.ExpandAndGetRef().m_uiMember1 = 0x5;
      WriteArray.ExpandAndGetRef().m_uiMember1 = 0x6;

      StreamWriter.WriteArray(WriteArray).IgnoreResult();
    }

    // Read back in
    {
      // Create reader
      wdMemoryStreamReader StreamReader(&StreamStorage);

      // This intentionally uses a different array type for the read back
      // to verify that it is a) compatible and b) all arrays are somewhat tested
      wdHybridArray<SerializableStructWithMethods, 1> ReadArray;

      StreamReader.ReadArray(ReadArray).IgnoreResult();

      WD_TEST_INT(ReadArray.GetCount(), 2);

      WD_TEST_INT(ReadArray[0].m_uiMember1, 0x5);
      WD_TEST_INT(ReadArray[0].m_uiMember2, 0x23);

      WD_TEST_INT(ReadArray[1].m_uiMember1, 0x6);
      WD_TEST_INT(ReadArray[1].m_uiMember2, 0x23);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdSet Stream Operators")
  {
    wdDefaultMemoryStreamStorage StreamStorage(4096);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    wdSet<wdString> TestSet;
    TestSet.Insert("Hello");
    TestSet.Insert("World");
    TestSet.Insert("!");

    StreamWriter.WriteSet(TestSet).IgnoreResult();

    wdSet<wdString> TestSetReadBack;

    TestSetReadBack.Insert("Shouldn't be there after deserialization.");

    wdMemoryStreamReader StreamReader(&StreamStorage);

    StreamReader.ReadSet(TestSetReadBack).IgnoreResult();

    WD_TEST_INT(TestSetReadBack.GetCount(), 3);

    WD_TEST_BOOL(TestSetReadBack.Contains("Hello"));
    WD_TEST_BOOL(TestSetReadBack.Contains("!"));
    WD_TEST_BOOL(TestSetReadBack.Contains("World"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdMap Stream Operators")
  {
    wdDefaultMemoryStreamStorage StreamStorage(4096);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    wdMap<wdUInt64, wdString> TestMap;
    TestMap.Insert(42, "Hello");
    TestMap.Insert(23, "World");
    TestMap.Insert(5, "!");

    StreamWriter.WriteMap(TestMap).IgnoreResult();

    wdMap<wdUInt64, wdString> TestMapReadBack;

    TestMapReadBack.Insert(1, "Shouldn't be there after deserialization.");

    wdMemoryStreamReader StreamReader(&StreamStorage);

    StreamReader.ReadMap(TestMapReadBack).IgnoreResult();

    WD_TEST_INT(TestMapReadBack.GetCount(), 3);

    WD_TEST_BOOL(TestMapReadBack.Contains(42));
    WD_TEST_BOOL(TestMapReadBack.Contains(5));
    WD_TEST_BOOL(TestMapReadBack.Contains(23));

    WD_TEST_BOOL(TestMapReadBack.GetValue(42)->IsEqual("Hello"));
    WD_TEST_BOOL(TestMapReadBack.GetValue(5)->IsEqual("!"));
    WD_TEST_BOOL(TestMapReadBack.GetValue(23)->IsEqual("World"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdHashTable Stream Operators")
  {
    wdDefaultMemoryStreamStorage StreamStorage(4096);

    // Create writer
    wdMemoryStreamWriter StreamWriter(&StreamStorage);

    wdHashTable<wdUInt64, wdString> TestHashTable;
    TestHashTable.Insert(42, "Hello");
    TestHashTable.Insert(23, "World");
    TestHashTable.Insert(5, "!");

    StreamWriter.WriteHashTable(TestHashTable).IgnoreResult();

    wdMap<wdUInt64, wdString> TestHashTableReadBack;

    TestHashTableReadBack.Insert(1, "Shouldn't be there after deserialization.");

    wdMemoryStreamReader StreamReader(&StreamStorage);

    StreamReader.ReadMap(TestHashTableReadBack).IgnoreResult();

    WD_TEST_INT(TestHashTableReadBack.GetCount(), 3);

    WD_TEST_BOOL(TestHashTableReadBack.Contains(42));
    WD_TEST_BOOL(TestHashTableReadBack.Contains(5));
    WD_TEST_BOOL(TestHashTableReadBack.Contains(23));

    WD_TEST_BOOL(TestHashTableReadBack.GetValue(42)->IsEqual("Hello"));
    WD_TEST_BOOL(TestHashTableReadBack.GetValue(5)->IsEqual("!"));
    WD_TEST_BOOL(TestHashTableReadBack.GetValue(23)->IsEqual("World"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "String Deduplication")
  {
    wdDefaultMemoryStreamStorage StreamStorageNonDeduplicated(4096);
    wdDefaultMemoryStreamStorage StreamStorageDeduplicated(4096);

    wdHybridString<4> str1 = "Hello World";
    wdDynamicString str2 = "Hello World 2";
    wdStringBuilder str3 = "Hello Schlumpf";

    // Non deduplicated serialization
    {
      wdMemoryStreamWriter StreamWriter(&StreamStorageNonDeduplicated);

      StreamWriter << str1;
      StreamWriter << str2;
      StreamWriter << str1;
      StreamWriter << str3;
      StreamWriter << str1;
      StreamWriter << str2;
    }

    // Deduplicated serialization
    {
      wdMemoryStreamWriter StreamWriter(&StreamStorageDeduplicated);

      wdStringDeduplicationWriteContext StringDeduplicationContext(StreamWriter);
      auto& DeduplicationWriter = StringDeduplicationContext.Begin();

      DeduplicationWriter << str1;
      DeduplicationWriter << str2;
      DeduplicationWriter << str1;
      DeduplicationWriter << str3;
      DeduplicationWriter << str1;
      DeduplicationWriter << str2;

      StringDeduplicationContext.End().IgnoreResult();

      WD_TEST_INT(StringDeduplicationContext.GetUniqueStringCount(), 3);
    }

    WD_TEST_BOOL(StreamStorageDeduplicated.GetStorageSize64() < StreamStorageNonDeduplicated.GetStorageSize64());

    // Read the deduplicated strings back
    {
      wdMemoryStreamReader StreamReader(&StreamStorageDeduplicated);

      wdStringDeduplicationReadContext StringDeduplicationReadContext(StreamReader);

      wdHybridString<16> szRead0, szRead1, szRead2;
      wdStringBuilder szRead3, szRead4, szRead5;

      StreamReader >> szRead0;
      StreamReader >> szRead1;
      StreamReader >> szRead2;
      StreamReader >> szRead3;
      StreamReader >> szRead4;
      StreamReader >> szRead5;

      WD_TEST_STRING(szRead0, szRead2);
      WD_TEST_STRING(szRead0, szRead4);
      WD_TEST_STRING(szRead1, szRead5);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Array Serialization Performance (bytes)")
  {
    constexpr wdUInt32 uiCount = 1024 * 1024 * 10;

    wdContiguousMemoryStreamStorage storage(uiCount + 16);

    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    wdDynamicArray<wdUInt8> DynamicArray;
    DynamicArray.SetCountUninitialized(uiCount);

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      DynamicArray[i] = i & 0xFF;
    }

    {
      wdStopwatch sw;

      writer.WriteArray(DynamicArray).AssertSuccess();

      wdTime t = sw.GetRunningTotal();
      wdStringBuilder s;
      s.Format("Write {} byte array: {}", wdArgFileSize(uiCount), t);
      wdTestFramework::Output(wdTestOutput::Details, s);
    }

    {
      wdStopwatch sw;

      reader.ReadArray(DynamicArray).IgnoreResult();

      wdTime t = sw.GetRunningTotal();
      wdStringBuilder s;
      s.Format("Read {} byte array: {}", wdArgFileSize(uiCount), t);
      wdTestFramework::Output(wdTestOutput::Details, s);
    }

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      WD_TEST_INT(DynamicArray[i], i & 0xFF);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Array Serialization Performance (wdVec3)")
  {
    constexpr wdUInt32 uiCount = 1024 * 1024 * 10;

    wdContiguousMemoryStreamStorage storage(uiCount * sizeof(wdVec3) + 16);

    wdMemoryStreamWriter writer(&storage);
    wdMemoryStreamReader reader(&storage);

    wdDynamicArray<wdVec3> DynamicArray;
    DynamicArray.SetCountUninitialized(uiCount);

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      DynamicArray[i].Set(i, i + 1, i + 2);
    }

    {
      wdStopwatch sw;

      writer.WriteArray(DynamicArray).AssertSuccess();

      wdTime t = sw.GetRunningTotal();
      wdStringBuilder s;
      s.Format("Write {} vec3 array: {}", wdArgFileSize(uiCount * sizeof(wdVec3)), t);
      wdTestFramework::Output(wdTestOutput::Details, s);
    }

    {
      wdStopwatch sw;

      reader.ReadArray(DynamicArray).AssertSuccess();

      wdTime t = sw.GetRunningTotal();
      wdStringBuilder s;
      s.Format("Read {} vec3 array: {}", wdArgFileSize(uiCount * sizeof(wdVec3)), t);
      wdTestFramework::Output(wdTestOutput::Details, s);
    }

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      WD_TEST_VEC3(DynamicArray[i], wdVec3(i, i + 1, i + 2), 0.01f);
    }
  }
}
