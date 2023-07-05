#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Algorithm/HashHelperString.h>
#include <Foundation/Algorithm/HashStream.h>
#include <Foundation/Algorithm/HashableStruct.h>
#include <Foundation/Strings/HashedString.h>

WD_CREATE_SIMPLE_TEST_GROUP(Algorithm);

// Warning for overflow in compile time executed static_assert(wdHashingUtils::MurmurHash32...)
// Todo: Why is this not happening elsewhere?
#pragma warning(disable : 4307)

WD_CREATE_SIMPLE_TEST(Algorithm, Hashing)
{
  // check whether compile time hashing gives the same value as runtime hashing
  const char* szString = "This is a test string. 1234";
  const char* szStringLower = "this is a test string. 1234";
  const char* szString2 = "THiS iS A TESt sTrInG. 1234";
  wdStringBuilder sb = szString;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Hashfunction")
  {
    wdUInt32 uiHashRT = wdHashingUtils::MurmurHash32String(sb.GetData());
    constexpr wdUInt32 uiHashCT = wdHashingUtils::MurmurHash32String("This is a test string. 1234");
    WD_TEST_INT(uiHashRT, 0xb999d6c4);
    WD_TEST_INT(uiHashRT, uiHashCT);

    // Static assert to ensure this is happening at compile time!
    static_assert(wdHashingUtils::MurmurHash32String("This is a test string. 1234") == static_cast<wdUInt32>(0xb999d6c4), "Error in compile time murmur hash calculation!");

    {
      // Test short inputs (< 16 characters) of xx hash at compile time
      wdUInt32 uixxHashRT = wdHashingUtils::xxHash32("Test string", 11, 0);
      wdUInt32 uixxHashCT = wdHashingUtils::xxHash32String("Test string", 0);
      WD_TEST_INT(uixxHashRT, uixxHashCT);
      static_assert(wdHashingUtils::xxHash32String("Test string") == 0x1b50ee03);

      // Test long inputs ( > 16 characters) of xx hash at compile time
      wdUInt32 uixxHashRTLong = wdHashingUtils::xxHash32String(sb.GetData());
      wdUInt32 uixxHashCTLong = wdHashingUtils::xxHash32String("This is a test string. 1234");
      WD_TEST_INT(uixxHashRTLong, uixxHashCTLong);
      static_assert(wdHashingUtils::xxHash32String("This is a test string. 1234") == 0xff35b049);
    }

    {
      // Test short inputs (< 32 characters) of xx hash 64 at compile time
      wdUInt64 uixxHash64RT = wdHashingUtils::xxHash64("Test string", 11, 0);
      wdUInt64 uixxHash64CT = wdHashingUtils::xxHash64String("Test string", 0);
      WD_TEST_INT(uixxHash64RT, uixxHash64CT);
      static_assert(wdHashingUtils::xxHash64String("Test string") == 0xcf0f91eece7c88feULL);

      // Test long inputs ( > 32 characters) of xx hash 64 at compile time
      wdUInt64 uixxHash64RTLong = wdHashingUtils::xxHash64String(wdStringView("This is a longer test string for 64-bit. 123456"));
      wdUInt64 uixxHash64CTLong = wdHashingUtils::xxHash64String("This is a longer test string for 64-bit. 123456");
      WD_TEST_INT(uixxHash64RTLong, uixxHash64CTLong);
      static_assert(wdHashingUtils::xxHash64String("This is a longer test string for 64-bit. 123456") == 0xb85d007925299bacULL);
    }

    {
      // Test short inputs (< 32 characters) of xx hash 64 at compile time
      wdUInt64 uixxHash64RT = wdHashingUtils::StringHash(wdStringView("Test string"));
      wdUInt64 uixxHash64CT = wdHashingUtils::StringHash("Test string");
      WD_TEST_INT(uixxHash64RT, uixxHash64CT);
      static_assert(wdHashingUtils::StringHash("Test string") == 0xcf0f91eece7c88feULL);

      // Test long inputs ( > 32 characters) of xx hash 64 at compile time
      wdUInt64 uixxHash64RTLong = wdHashingUtils::StringHash(wdStringView("This is a longer test string for 64-bit. 123456"));
      wdUInt64 uixxHash64CTLong = wdHashingUtils::StringHash("This is a longer test string for 64-bit. 123456");
      WD_TEST_INT(uixxHash64RTLong, uixxHash64CTLong);
      static_assert(wdHashingUtils::StringHash("This is a longer test string for 64-bit. 123456") == 0xb85d007925299bacULL);
    }

    // Check MurmurHash for unaligned inputs
    const char* alignmentTestString = "12345678_12345678__12345678___12345678";
    wdUInt32 uiHash1 = wdHashingUtils::MurmurHash32(alignmentTestString, 8);
    wdUInt32 uiHash2 = wdHashingUtils::MurmurHash32(alignmentTestString + 9, 8);
    wdUInt32 uiHash3 = wdHashingUtils::MurmurHash32(alignmentTestString + 19, 8);
    wdUInt32 uiHash4 = wdHashingUtils::MurmurHash32(alignmentTestString + 30, 8);
    WD_TEST_INT(uiHash1, uiHash2);
    WD_TEST_INT(uiHash1, uiHash3);
    WD_TEST_INT(uiHash1, uiHash4);

    // check 64bit hashes
    const wdUInt64 uiMurmurHash64 = wdHashingUtils::MurmurHash64(sb.GetData(), sb.GetElementCount());
    WD_TEST_INT(uiMurmurHash64, 0xf8ebc5e8cb110786);

    // Check MurmurHash64 for unaligned inputs
    wdUInt64 uiHash1_64 = wdHashingUtils::MurmurHash64(alignmentTestString, 8);
    wdUInt64 uiHash2_64 = wdHashingUtils::MurmurHash64(alignmentTestString + 9, 8);
    wdUInt64 uiHash3_64 = wdHashingUtils::MurmurHash64(alignmentTestString + 19, 8);
    wdUInt64 uiHash4_64 = wdHashingUtils::MurmurHash64(alignmentTestString + 30, 8);
    WD_TEST_INT(uiHash1_64, uiHash2_64);
    WD_TEST_INT(uiHash1_64, uiHash3_64);
    WD_TEST_INT(uiHash1_64, uiHash4_64);

    // test crc32
    const wdUInt32 uiCrc32 = wdHashingUtils::CRC32Hash(sb.GetData(), sb.GetElementCount());
    WD_TEST_INT(uiCrc32, 0x73b5e898);

    // Check crc32 for unaligned inputs
    uiHash1 = wdHashingUtils::CRC32Hash(alignmentTestString, 8);
    uiHash2 = wdHashingUtils::CRC32Hash(alignmentTestString + 9, 8);
    uiHash3 = wdHashingUtils::CRC32Hash(alignmentTestString + 19, 8);
    uiHash4 = wdHashingUtils::CRC32Hash(alignmentTestString + 30, 8);
    WD_TEST_INT(uiHash1, uiHash2);
    WD_TEST_INT(uiHash1, uiHash3);
    WD_TEST_INT(uiHash1, uiHash4);

    // 32 Bit xxHash
    const wdUInt32 uiXXHash32 = wdHashingUtils::xxHash32(sb.GetData(), sb.GetElementCount());
    WD_TEST_INT(uiXXHash32, 0xff35b049);

    // Check xxHash for unaligned inputs
    uiHash1 = wdHashingUtils::xxHash32(alignmentTestString, 8);
    uiHash2 = wdHashingUtils::xxHash32(alignmentTestString + 9, 8);
    uiHash3 = wdHashingUtils::xxHash32(alignmentTestString + 19, 8);
    uiHash4 = wdHashingUtils::xxHash32(alignmentTestString + 30, 8);
    WD_TEST_INT(uiHash1, uiHash2);
    WD_TEST_INT(uiHash1, uiHash3);
    WD_TEST_INT(uiHash1, uiHash4);

    // 64 Bit xxHash
    const wdUInt64 uiXXHash64 = wdHashingUtils::xxHash64(sb.GetData(), sb.GetElementCount());
    WD_TEST_INT(uiXXHash64, 0x141fb89c0bf32020);
    // Check xxHash64 for unaligned inputs
    uiHash1_64 = wdHashingUtils::xxHash64(alignmentTestString, 8);
    uiHash2_64 = wdHashingUtils::xxHash64(alignmentTestString + 9, 8);
    uiHash3_64 = wdHashingUtils::xxHash64(alignmentTestString + 19, 8);
    uiHash4_64 = wdHashingUtils::xxHash64(alignmentTestString + 30, 8);
    WD_TEST_INT(uiHash1_64, uiHash2_64);
    WD_TEST_INT(uiHash1_64, uiHash3_64);
    WD_TEST_INT(uiHash1_64, uiHash4_64);

    wdUInt32 uixxHash32RTEmpty = wdHashingUtils::xxHash32("", 0, 0);
    wdUInt32 uixxHash32CTEmpty = wdHashingUtils::xxHash32String("", 0);
    WD_TEST_BOOL(uixxHash32RTEmpty == uixxHash32CTEmpty);

    wdUInt64 uixxHash64RTEmpty = wdHashingUtils::xxHash64("", 0, 0);
    wdUInt64 uixxHash64CTEmpty = wdHashingUtils::xxHash64String("", 0);
    WD_TEST_BOOL(uixxHash64RTEmpty == uixxHash64CTEmpty);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HashHelper")
  {
    wdUInt32 uiHash = wdHashHelper<wdStringBuilder>::Hash(sb);
    WD_TEST_INT(uiHash, 0x0bf32020);

    const char* szTest = "This is a test string. 1234";
    uiHash = wdHashHelper<const char*>::Hash(szTest);
    WD_TEST_INT(uiHash, 0x0bf32020);
    WD_TEST_BOOL(wdHashHelper<const char*>::Equal(szTest, sb.GetData()));

    wdHashedString hs;
    hs.Assign(szTest);
    uiHash = wdHashHelper<wdHashedString>::Hash(hs);
    WD_TEST_INT(uiHash, 0x0bf32020);

    wdTempHashedString ths(szTest);
    uiHash = wdHashHelper<wdHashedString>::Hash(ths);
    WD_TEST_INT(uiHash, 0x0bf32020);
    WD_TEST_BOOL(wdHashHelper<wdHashedString>::Equal(hs, ths));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HashHelperString_NoCase")
  {
    const wdUInt32 uiHash = wdHashHelper<const char*>::Hash(szStringLower);
    WD_TEST_INT(uiHash, 0x19404167);
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(szString));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(szStringLower));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(szString2));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(sb));
    wdStringBuilder sb2 = szString2;
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(sb2));
    wdString sL = szStringLower;
    wdString s1 = sb;
    wdString s2 = sb2;
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(s1));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(s2));
    wdStringView svL = szStringLower;
    wdStringView sv1 = szString;
    wdStringView sv2 = szString2;
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(svL));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(sv1));
    WD_TEST_INT(uiHash, wdHashHelperString_NoCase::Hash(sv2));

    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sb, sb2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sb, szString2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sb, sv2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(s1, sb2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(s1, szString2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(s1, sv2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sv1, sb2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sv1, szString2));
    WD_TEST_BOOL(wdHashHelperString_NoCase::Equal(sv1, sv2));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HashStream32")
  {
    const char* szTest = "This is a test string. 1234";
    const char* szTestHalf1 = "This is a test";
    const char* szTestHalf2 = " string. 1234";

    auto test = [szTest, szTestHalf1, szTestHalf2](bool bFlush, wdUInt32* pHash) {
      wdHashStreamWriter32 writer1;
      writer1.WriteBytes(szTest, std::strlen(szTest)).IgnoreResult();
      if (bFlush)
      {
        writer1.Flush().IgnoreResult();
      }

      const wdUInt32 uiHash1 = writer1.GetHashValue();

      wdHashStreamWriter32 writer2;
      writer2.WriteBytes(szTestHalf1, std::strlen(szTestHalf1)).IgnoreResult();
      if (bFlush)
      {
        writer2.Flush().IgnoreResult();
      }

      writer2.WriteBytes(szTestHalf2, std::strlen(szTestHalf2)).IgnoreResult();
      if (bFlush)
      {
        writer2.Flush().IgnoreResult();
      }

      const wdUInt32 uiHash2 = writer2.GetHashValue();

      wdHashStreamWriter32 writer3;
      for (wdUInt64 i = 0; szTest[i] != 0; ++i)
      {
        writer3.WriteBytes(szTest + i, 1).IgnoreResult();

        if (bFlush)
        {
          writer3.Flush().IgnoreResult();
        }
      }
      const wdUInt32 uiHash3 = writer3.GetHashValue();

      WD_TEST_INT(uiHash1, uiHash2);
      WD_TEST_INT(uiHash1, uiHash3);

      *pHash = uiHash1;
    };

    wdUInt32 uiHash1 = 0, uiHash2 = 1;
    test(true, &uiHash1);
    test(false, &uiHash2);
    WD_TEST_INT(uiHash1, uiHash2);

    const wdUInt64 uiHash3 = wdHashingUtils::xxHash32(szTest, std::strlen(szTest));
    WD_TEST_INT(uiHash1, uiHash3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HashStream64")
  {
    const char* szTest = "This is a test string. 1234";
    const char* szTestHalf1 = "This is a test";
    const char* szTestHalf2 = " string. 1234";

    auto test = [szTest, szTestHalf1, szTestHalf2](bool bFlush, wdUInt64* pHash) {
      wdHashStreamWriter64 writer1;
      writer1.WriteBytes(szTest, std::strlen(szTest)).IgnoreResult();

      if (bFlush)
      {
        writer1.Flush().IgnoreResult();
      }

      const wdUInt64 uiHash1 = writer1.GetHashValue();

      wdHashStreamWriter64 writer2;
      writer2.WriteBytes(szTestHalf1, std::strlen(szTestHalf1)).IgnoreResult();
      if (bFlush)
        writer2.Flush().IgnoreResult();
      writer2.WriteBytes(szTestHalf2, std::strlen(szTestHalf2)).IgnoreResult();
      if (bFlush)
        writer2.Flush().IgnoreResult();

      const wdUInt64 uiHash2 = writer2.GetHashValue();

      wdHashStreamWriter64 writer3;
      for (wdUInt64 i = 0; szTest[i] != 0; ++i)
      {
        writer3.WriteBytes(szTest + i, 1).IgnoreResult();
        if (bFlush)
          writer3.Flush().IgnoreResult();
      }
      const wdUInt64 uiHash3 = writer3.GetHashValue();

      WD_TEST_INT(uiHash1, uiHash2);
      WD_TEST_INT(uiHash1, uiHash3);

      *pHash = uiHash1;
    };

    wdUInt64 uiHash1 = 0, uiHash2 = 1;
    test(true, &uiHash1);
    test(false, &uiHash2);
    WD_TEST_INT(uiHash1, uiHash2);

    const wdUInt64 uiHash3 = wdHashingUtils::xxHash64(szTest, std::strlen(szTest));
    WD_TEST_INT(uiHash1, uiHash3);
  }
}

struct SimpleHashableStruct : public wdHashableStruct<SimpleHashableStruct>
{
  wdUInt32 m_uiTestMember1;
  wdUInt8 m_uiTestMember2;
  wdUInt64 m_uiTestMember3;
};

struct SimpleStruct
{
  wdUInt32 m_uiTestMember1;
  wdUInt8 m_uiTestMember2;
  wdUInt64 m_uiTestMember3;
};

WD_CREATE_SIMPLE_TEST(Algorithm, HashableStruct)
{
  SimpleHashableStruct AutomaticInst;
  WD_TEST_INT(AutomaticInst.m_uiTestMember1, 0);
  WD_TEST_INT(AutomaticInst.m_uiTestMember2, 0);
  WD_TEST_INT(AutomaticInst.m_uiTestMember3, 0);

  SimpleStruct NonAutomaticInst;
  wdMemoryUtils::ZeroFill(&NonAutomaticInst, 1);

  WD_CHECK_AT_COMPILETIME(sizeof(AutomaticInst) == sizeof(NonAutomaticInst));

  WD_TEST_INT(wdMemoryUtils::Compare<wdUInt8>((wdUInt8*)&AutomaticInst, (wdUInt8*)&NonAutomaticInst, sizeof(AutomaticInst)), 0);

  AutomaticInst.m_uiTestMember2 = 0x42u;
  AutomaticInst.m_uiTestMember3 = 0x23u;

  wdUInt32 uiAutomaticHash = AutomaticInst.CalculateHash();

  NonAutomaticInst.m_uiTestMember2 = 0x42u;
  NonAutomaticInst.m_uiTestMember3 = 0x23u;

  wdUInt32 uiNonAutomaticHash = wdHashingUtils::xxHash32(&NonAutomaticInst, sizeof(NonAutomaticInst));

  WD_TEST_INT(uiAutomaticHash, uiNonAutomaticHash);

  AutomaticInst.m_uiTestMember1 = 0x5u;
  uiAutomaticHash = AutomaticInst.CalculateHash();

  WD_TEST_BOOL(uiAutomaticHash != uiNonAutomaticHash);
}
