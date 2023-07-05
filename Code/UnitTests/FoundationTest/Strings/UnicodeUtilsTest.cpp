#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Strings, UnicodeUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsASCII")
  {
    // test all ASCII Characters
    for (wdUInt32 i = 0; i < 128; ++i)
      WD_TEST_BOOL(wdUnicodeUtils::IsASCII(i));

    for (wdUInt32 i = 128; i < 0xFFFFF; ++i)
      WD_TEST_BOOL(!wdUnicodeUtils::IsASCII(i));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsUtf8StartByte")
  {
    wdStringUtf8 s(L"äöü€");
    // ä
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf8StartByte(s.GetData()[0]));
    WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8StartByte(s.GetData()[1]));

    // ö
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf8StartByte(s.GetData()[2]));
    WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8StartByte(s.GetData()[3]));

    // ü
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf8StartByte(s.GetData()[4]));
    WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8StartByte(s.GetData()[5]));

    // €
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf8StartByte(s.GetData()[6]));
    WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8StartByte(s.GetData()[7]));
    WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8StartByte(s.GetData()[8]));

    // \0
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf8StartByte(s.GetData()[9]));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsUtf8ContinuationByte")
  {
    // all ASCII Characters are not continuation bytes
    for (char i = 0; i < 127; ++i)
    {
      WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8ContinuationByte(i));
    }

    for (wdUInt32 i = 0; i < 255u; ++i)
    {
      const char uiContByte = static_cast<char>(0x80 | (i & 0x3f));
      const char uiNoContByte1 = static_cast<char>(i | 0x40);
      const char uiNoContByte2 = static_cast<char>(i | 0xC0);

      WD_TEST_BOOL(wdUnicodeUtils::IsUtf8ContinuationByte(uiContByte));
      WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8ContinuationByte(uiNoContByte1));
      WD_TEST_BOOL(!wdUnicodeUtils::IsUtf8ContinuationByte(uiNoContByte2));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetUtf8SequenceLength")
  {
    // All ASCII characters are 1 byte in length
    for (char i = 0; i < 127; ++i)
    {
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(i), 1);
    }

    {
      wdStringUtf8 s(L"ä");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 2);
    }

    {
      wdStringUtf8 s(L"ß");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 2);
    }

    {
      wdStringUtf8 s(L"€");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 3);
    }

    {
      wdStringUtf8 s(L"з");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 2);
    }

    {
      wdStringUtf8 s(L"г");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 2);
    }

    {
      wdStringUtf8 s(L"ы");
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 2);
    }

    {
      wdUInt32 u[2] = {L'\u0B87', 0};
      wdStringUtf8 s(u);
      WD_TEST_INT(wdUnicodeUtils::GetUtf8SequenceLength(s.GetData()[0]), 3);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ConvertUtf8ToUtf32")
  {
    // Just wraps around 'utf8::peek_next'
    // I think we can assume that that works.
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetSizeForCharacterInUtf8")
  {
    // All ASCII characters are 1 byte in length
    for (wdUInt32 i = 0; i < 128; ++i)
      WD_TEST_INT(wdUnicodeUtils::GetSizeForCharacterInUtf8(i), 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Decode")
  {
    char utf8[] = {(char)0xc3, (char)0xb6, 0};
    wdUInt16 utf16[] = {0xf6, 0};
    wchar_t wchar[] = {L'ö', 0};

    char* szUtf8 = &utf8[0];
    wdUInt16* szUtf16 = &utf16[0];
    wchar_t* szWChar = &wchar[0];

    wdUInt32 uiUtf321 = wdUnicodeUtils::DecodeUtf8ToUtf32(szUtf8);
    wdUInt32 uiUtf322 = wdUnicodeUtils::DecodeUtf16ToUtf32(szUtf16);
    wdUInt32 uiUtf323 = wdUnicodeUtils::DecodeWCharToUtf32(szWChar);

    WD_TEST_INT(uiUtf321, uiUtf322);
    WD_TEST_INT(uiUtf321, uiUtf323);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Encode")
  {
    char utf8[4] = {0};
    wdUInt16 utf16[4] = {0};
    wchar_t wchar[4] = {0};

    char* szUtf8 = &utf8[0];
    wdUInt16* szUtf16 = &utf16[0];
    wchar_t* szWChar = &wchar[0];

    wdUnicodeUtils::EncodeUtf32ToUtf8(0xf6, szUtf8);
    wdUnicodeUtils::EncodeUtf32ToUtf16(0xf6, szUtf16);
    wdUnicodeUtils::EncodeUtf32ToWChar(0xf6, szWChar);

    WD_TEST_BOOL(utf8[0] == (char)0xc3 && utf8[1] == (char)0xb6);
    WD_TEST_BOOL(utf16[0] == 0xf6);
    WD_TEST_BOOL(wchar[0] == L'ö');
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MoveToNextUtf8")
  {
    wdStringUtf8 s(L"aböäß€de");

    WD_TEST_INT(s.GetElementCount(), 13);

    const char* sz = s.GetData();

    // test how far it skips ahead

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[1]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[2]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[4]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[6]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[8]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[11]);

    wdUnicodeUtils::MoveToNextUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[12]);

    sz = s.GetData();
    const char* szEnd = s.GetView().GetEndPointer();


    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[1]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[2]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[4]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[6]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[8]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[11]);

    wdUnicodeUtils::MoveToNextUtf8(sz, szEnd);
    WD_TEST_BOOL(sz == &s.GetData()[12]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MoveToPriorUtf8")
  {
    wdStringUtf8 s(L"aböäß€de");

    const char* sz = &s.GetData()[13];

    WD_TEST_INT(s.GetElementCount(), 13);

    // test how far it skips ahead

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[12]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[11]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[8]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[6]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[4]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[2]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[1]);

    wdUnicodeUtils::MoveToPriorUtf8(sz);
    WD_TEST_BOOL(sz == &s.GetData()[0]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SkipUtf8Bom")
  {
    // C++ is really stupid, chars are signed, but Utf8 only works with unsigned values ... argh!

    char szWithBom[] = {(char)0xef, (char)0xbb, (char)0xbf, 'a'};
    char szNoBom[] = {'a'};
    const char* pString = szWithBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf8Bom(pString) == true);
    WD_TEST_BOOL(pString == &szWithBom[3]);

    pString = szNoBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf8Bom(pString) == false);
    WD_TEST_BOOL(pString == szNoBom);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SkipUtf16BomLE")
  {
    wdUInt16 szWithBom[] = {0xfffe, 'a'};
    wdUInt16 szNoBom[] = {'a'};

    const wdUInt16* pString = szWithBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf16BomLE(pString) == true);
    WD_TEST_BOOL(pString == &szWithBom[1]);

    pString = szNoBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf16BomLE(pString) == false);
    WD_TEST_BOOL(pString == szNoBom);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SkipUtf16BomBE")
  {
    wdUInt16 szWithBom[] = {0xfeff, 'a'};
    wdUInt16 szNoBom[] = {'a'};

    const wdUInt16* pString = szWithBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf16BomBE(pString) == true);
    WD_TEST_BOOL(pString == &szWithBom[1]);

    pString = szNoBom;

    WD_TEST_BOOL(wdUnicodeUtils::SkipUtf16BomBE(pString) == false);
    WD_TEST_BOOL(pString == szNoBom);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsUtf16Surrogate")
  {
    wdUInt16 szNoSurrogate[] = {0x2AD7};
    wdUInt16 szSurrogate[] = {0xd83e};

    WD_TEST_BOOL(wdUnicodeUtils::IsUtf16Surrogate(szNoSurrogate) == false);
    WD_TEST_BOOL(wdUnicodeUtils::IsUtf16Surrogate(szSurrogate) == true);
  }
}
