#include <FoundationTest/FoundationTestPCH.h>

// NOTE: Always save this file as "Unicode (UTF-8 with signature)"
// otherwise important Unicode characters are not encoded

#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST_GROUP(Strings);

WD_CREATE_SIMPLE_TEST(Strings, StringUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsNullOrEmpty")
  {
    WD_TEST_BOOL(wdStringUtils::IsNullOrEmpty((char*)nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::IsNullOrEmpty("") == true);

    // all other characters are not empty
    for (wdUInt8 c = 1; c < 255; c++)
      WD_TEST_BOOL(wdStringUtils::IsNullOrEmpty(&c) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetStringElementCount")
  {
    WD_TEST_INT(wdStringUtils::GetStringElementCount((char*)nullptr), 0);

    // Counts the Bytes
    WD_TEST_INT(wdStringUtils::GetStringElementCount(""), 0);
    WD_TEST_INT(wdStringUtils::GetStringElementCount("a"), 1);
    WD_TEST_INT(wdStringUtils::GetStringElementCount("ab"), 2);
    WD_TEST_INT(wdStringUtils::GetStringElementCount("abc"), 3);

    // Counts the number of wchar_t's
    WD_TEST_INT(wdStringUtils::GetStringElementCount(L""), 0);
    WD_TEST_INT(wdStringUtils::GetStringElementCount(L"a"), 1);
    WD_TEST_INT(wdStringUtils::GetStringElementCount(L"ab"), 2);
    WD_TEST_INT(wdStringUtils::GetStringElementCount(L"abc"), 3);

    // test with a sub-string
    const char* sz = "abc def ghi";
    WD_TEST_INT(wdStringUtils::GetStringElementCount(sz, sz + 0), 0);
    WD_TEST_INT(wdStringUtils::GetStringElementCount(sz, sz + 3), 3);
    WD_TEST_INT(wdStringUtils::GetStringElementCount(sz, sz + 6), 6);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UpdateStringEnd")
  {
    const char* sz = "Test test";
    const char* szEnd = wdUnicodeUtils::GetMaxStringEnd<char>();

    wdStringUtils::UpdateStringEnd(sz, szEnd);
    WD_TEST_BOOL(szEnd == sz + wdStringUtils::GetStringElementCount(sz));

    wdStringUtils::UpdateStringEnd(sz, szEnd);
    WD_TEST_BOOL(szEnd == sz + wdStringUtils::GetStringElementCount(sz));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCharacterCount")
  {
    WD_TEST_INT(wdStringUtils::GetCharacterCount(nullptr), 0);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(""), 0);
    WD_TEST_INT(wdStringUtils::GetCharacterCount("a"), 1);
    WD_TEST_INT(wdStringUtils::GetCharacterCount("abc"), 3);

    wdStringUtf8 s(L"äöü"); // 6 Bytes

    WD_TEST_INT(wdStringUtils::GetStringElementCount(s.GetData()), 6);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(s.GetData()), 3);

    // test with a sub-string
    const char* sz = "abc def ghi";
    WD_TEST_INT(wdStringUtils::GetCharacterCount(sz, sz + 0), 0);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(sz, sz + 3), 3);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(sz, sz + 6), 6);

    WD_TEST_INT(wdStringUtils::GetCharacterCount(s.GetData(), s.GetData() + 0), 0);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(s.GetData(), s.GetData() + 2), 1);
    WD_TEST_INT(wdStringUtils::GetCharacterCount(s.GetData(), s.GetData() + 4), 2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCharacterAndElementCount")
  {
    wdUInt32 uiCC, uiEC;

    wdStringUtils::GetCharacterAndElementCount(nullptr, uiCC, uiEC);
    WD_TEST_INT(uiCC, 0);
    WD_TEST_INT(uiEC, 0);

    wdStringUtils::GetCharacterAndElementCount("", uiCC, uiEC);
    WD_TEST_INT(uiCC, 0);
    WD_TEST_INT(uiEC, 0);

    wdStringUtils::GetCharacterAndElementCount("a", uiCC, uiEC);
    WD_TEST_INT(uiCC, 1);
    WD_TEST_INT(uiEC, 1);

    wdStringUtils::GetCharacterAndElementCount("abc", uiCC, uiEC);
    WD_TEST_INT(uiCC, 3);
    WD_TEST_INT(uiEC, 3);

    wdStringUtf8 s(L"äöü"); // 6 Bytes

    wdStringUtils::GetCharacterAndElementCount(s.GetData(), uiCC, uiEC);
    WD_TEST_INT(uiCC, 3);
    WD_TEST_INT(uiEC, 6);

    wdStringUtils::GetCharacterAndElementCount(s.GetData(), uiCC, uiEC, s.GetData() + 0);
    WD_TEST_INT(uiCC, 0);
    WD_TEST_INT(uiEC, 0);

    wdStringUtils::GetCharacterAndElementCount(s.GetData(), uiCC, uiEC, s.GetData() + 4);
    WD_TEST_INT(uiCC, 2);
    WD_TEST_INT(uiEC, 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Copy (full)")
  {
    char szDest[256] = "";

    // large enough
    WD_TEST_INT(wdStringUtils::Copy(szDest, 256, "Test ABC"), 8);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, "Test ABC"));

    // exactly fitting
    WD_TEST_INT(wdStringUtils::Copy(szDest, 13, "Humpf, humpf"), 12);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, "Humpf, humpf"));

    // too small
    WD_TEST_INT(wdStringUtils::Copy(szDest, 8, "Test ABC"), 7);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, "Test AB"));

    const char* szUTF8 = "ABC \xe6\x97\xa5\xd1\x88"; // contains 'ABC ' + two UTF-8 chars (first is three bytes, second is two bytes)

    // large enough
    WD_TEST_INT(wdStringUtils::Copy(szDest, 256, szUTF8), 9);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, szUTF8));

    // exactly fitting
    WD_TEST_INT(wdStringUtils::Copy(szDest, 10, szUTF8), 9);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, szUTF8));

    // These tests are disabled as previously valid behavior was now turned into an assert.
    // Comment them in to test the assert.
    // too small 1
    /*WD_TEST_INT(wdStringUtils::Copy(szDest, 9, szUTF8), 7);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 5)); // one character less

    // too small 2
    WD_TEST_INT(wdStringUtils::Copy(szDest, 7, szUTF8), 4);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 4)); // two characters less*/


    // copy only from a subset
    WD_TEST_INT(wdStringUtils::Copy(szDest, 256, szUTF8, szUTF8 + 7), 7);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 5)); // two characters less
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CopyN")
  {
    char szDest[256] = "";

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, "Test ABC", 4), 4);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, "Test"));

    const char* szUTF8 = "ABC \xe6\x97\xa5\xd1\x88"; // contains 'ABC ' + two UTF-8 chars (first is three bytes, second is two bytes)

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 6), 9);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 6));

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 5), 7);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 5));

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 4), 4);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 4));

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 1), 1);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 1));

    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 0), 0);
    WD_TEST_BOOL(wdStringUtils::IsEqual(szDest, ""));

    // copy only from a subset
    WD_TEST_INT(wdStringUtils::CopyN(szDest, 256, szUTF8, 6, szUTF8 + 7), 7);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(szDest, szUTF8, 5));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToUpperChar")
  {
    // this only tests the ASCII range
    for (wdInt32 i = 0; i < 128; ++i)
      WD_TEST_INT(wdStringUtils::ToUpperChar(i), toupper(i));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToLowerChar")
  {
    // this only tests the ASCII range
    for (wdInt32 i = 0; i < 128; ++i)
      WD_TEST_INT(wdStringUtils::ToLowerChar(i), tolower(i));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToUpperString")
  {
    wdStringUtf8 sL(L"abc öäü ß €");
    wdStringUtf8 sU(L"ABC ÖÄÜ ß €");

    char szCopy[256];
    wdStringUtils::Copy(szCopy, 256, sL.GetData());

    wdStringUtils::ToUpperString(szCopy);

    WD_TEST_BOOL(wdStringUtils::IsEqual(szCopy, sU.GetData()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToLowerString")
  {
    wdStringUtf8 sL(L"abc öäü ß €");
    wdStringUtf8 sU(L"ABC ÖÄÜ ß €");

    char szCopy[256];
    wdStringUtils::Copy(szCopy, 256, sU.GetData());

    wdStringUtils::ToLowerString(szCopy);

    WD_TEST_BOOL(wdStringUtils::IsEqual(szCopy, sL.GetData()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareChars")
  {
    WD_TEST_BOOL(wdStringUtils::CompareChars('a', 'a') == 0); // make sure the order is right
    WD_TEST_BOOL(wdStringUtils::CompareChars('a', 'b') < 0);  // a smaller than b -> negative
    WD_TEST_BOOL(wdStringUtils::CompareChars('b', 'a') > 0);  // b bigger than a  -> positive
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareChars(utf8)")
  {
    WD_TEST_BOOL(wdStringUtils::CompareChars("a", "a") == 0); // make sure the order is right
    WD_TEST_BOOL(wdStringUtils::CompareChars("a", "b") < 0);  // a smaller than b -> negative
    WD_TEST_BOOL(wdStringUtils::CompareChars("b", "a") > 0);  // b bigger than a  -> positive
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareChars_NoCase")
  {
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('a', 'A') == 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('a', 'B') < 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('B', 'a') > 0);

    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('A', 'a') == 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('A', 'b') < 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase('b', 'A') > 0);

    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase(L'ä', L'Ä') == 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase(L'ä', L'Ö') < 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase(L'ö', L'Ä') > 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareChars_NoCase(utf8)")
  {
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("a", "A") == 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("a", "B") < 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("B", "a") > 0);

    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("A", "a") == 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("A", "b") < 0);
    WD_TEST_BOOL(wdStringUtils::CompareChars_NoCase("b", "A") > 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    WD_TEST_BOOL(wdStringUtils::IsEqual(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual("", "") == true);

    WD_TEST_BOOL(wdStringUtils::IsEqual("abc", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual("abc", "abcd") == false);
    WD_TEST_BOOL(wdStringUtils::IsEqual("abcd", "abc") == false);

    WD_TEST_BOOL(wdStringUtils::IsEqual("a", nullptr) == false);
    WD_TEST_BOOL(wdStringUtils::IsEqual(nullptr, "a") == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualN")
  {
    WD_TEST_BOOL(wdStringUtils::IsEqualN(nullptr, nullptr, 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(nullptr, "", 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("", nullptr, 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("", "", 1) == true);

    // as long as we compare 'nothing' the strings must be equal
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", nullptr, 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", "", 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN(nullptr, "abc", 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("", "abc", 0) == true);

    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", "abcdef", 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", "abcdef", 2) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", "abcdef", 3) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abc", "abcdef", 4) == false);

    WD_TEST_BOOL(wdStringUtils::IsEqualN("abcdef", "abc", 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abcdef", "abc", 2) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abcdef", "abc", 3) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN("abcdef", "abc", 4) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual_NoCase")
  {
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase("", "") == true);


    wdStringUtf8 sL(L"abc öäü ß €");
    wdStringUtf8 sU(L"ABC ÖÄÜ ß €");
    wdStringUtf8 sU2(L"ABC ÖÄÜ ß € ");

    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase(sL.GetData(), sU.GetData()) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase(sL.GetData(), sU2.GetData()) == false);
    WD_TEST_BOOL(wdStringUtils::IsEqual_NoCase(sU2.GetData(), sL.GetData()) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualN_NoCase")
  {
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(nullptr, nullptr, 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(nullptr, "", 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase("", nullptr, 1) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase("", "", 1) == true);

    // as long as we compare 'nothing' the strings must be equal
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase("abc", nullptr, 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase("abc", "", 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(nullptr, "abc", 0) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase("", "abc", 0) == true);

    wdStringUtf8 sL(L"abc öäü ß €");
    wdStringUtf8 sU(L"ABC ÖÄÜ ß € moep");

    for (wdInt32 i = 0; i < 12; ++i)
      WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(sL.GetData(), sU.GetData(), i) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(sL.GetData(), sU.GetData(), 12) == false);

    for (wdInt32 i = 0; i < 12; ++i)
      WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(sU.GetData(), sL.GetData(), i) == true);
    WD_TEST_BOOL(wdStringUtils::IsEqualN_NoCase(sU.GetData(), sL.GetData(), 12) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare")
  {
    WD_TEST_BOOL(wdStringUtils::Compare(nullptr, nullptr) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare(nullptr, "") == 0);
    WD_TEST_BOOL(wdStringUtils::Compare("", nullptr) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare("", "") == 0);

    WD_TEST_BOOL(wdStringUtils::Compare("abc", "abc") == 0);
    WD_TEST_BOOL(wdStringUtils::Compare("abc", "abcd") < 0);
    WD_TEST_BOOL(wdStringUtils::Compare("abcd", "abc") > 0);

    WD_TEST_BOOL(wdStringUtils::Compare("a", nullptr) > 0);
    WD_TEST_BOOL(wdStringUtils::Compare(nullptr, "a") < 0);

    // substring compare
    const char* sz = "abc def ghi bla";
    WD_TEST_BOOL(wdStringUtils::Compare(sz, "abc", sz + 3) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare(sz, "abc def", sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare(sz, sz, sz + 7, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare(sz, sz, sz + 7, sz + 6) > 0);
    WD_TEST_BOOL(wdStringUtils::Compare(sz, sz, sz + 7, sz + 8) < 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareN")
  {
    WD_TEST_BOOL(wdStringUtils::CompareN(nullptr, nullptr, 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(nullptr, "", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("", nullptr, 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("", "", 1) == 0);

    // as long as we compare 'nothing' the strings must be equal
    WD_TEST_BOOL(wdStringUtils::CompareN("abc", nullptr, 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abc", "", 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(nullptr, "abc", 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("", "abc", 0) == 0);

    WD_TEST_BOOL(wdStringUtils::CompareN("abc", "abcdef", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abc", "abcdef", 2) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abc", "abcdef", 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abc", "abcdef", 4) < 0);

    WD_TEST_BOOL(wdStringUtils::CompareN("abcdef", "abc", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abcdef", "abc", 2) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abcdef", "abc", 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN("abcdef", "abc", 4) > 0);

    // substring compare
    const char* sz = "abc def ghi bla";
    WD_TEST_BOOL(wdStringUtils::CompareN(sz, "abc", 10, sz + 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(sz, "abc def", 10, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(sz, sz, 10, sz + 7, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(sz, sz, 10, sz + 7, sz + 6) > 0);
    WD_TEST_BOOL(wdStringUtils::CompareN(sz, sz, 10, sz + 7, sz + 8) < 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare_NoCase")
  {
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(nullptr, nullptr) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(nullptr, "") == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("", nullptr) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("", "") == 0);

    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("abc", "aBc") == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("ABC", "abcd") < 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("abcd", "ABC") > 0);

    WD_TEST_BOOL(wdStringUtils::Compare_NoCase("a", nullptr) > 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(nullptr, "a") < 0);

    // substring compare
    const char* sz = "abc def ghi bla";
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(sz, "ABC", sz + 3) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(sz, "ABC def", sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(sz, sz, sz + 7, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(sz, sz, sz + 7, sz + 6) > 0);
    WD_TEST_BOOL(wdStringUtils::Compare_NoCase(sz, sz, sz + 7, sz + 8) < 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareN_NoCase")
  {
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(nullptr, nullptr, 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(nullptr, "", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("", nullptr, 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("", "", 1) == 0);

    // as long as we compare 'nothing' the strings must be equal
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abc", nullptr, 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abc", "", 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(nullptr, "abc", 0) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("", "abc", 0) == 0);

    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("aBc", "abcdef", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("aBc", "abcdef", 2) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("aBc", "abcdef", 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("aBc", "abcdef", 4) < 0);

    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abcdef", "Abc", 1) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abcdef", "Abc", 2) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abcdef", "Abc", 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase("abcdef", "Abc", 4) > 0);

    // substring compare
    const char* sz = "abc def ghi bla";
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(sz, "ABC", 10, sz + 3) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(sz, "ABC def", 10, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(sz, sz, 10, sz + 7, sz + 7) == 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(sz, sz, 10, sz + 7, sz + 6) > 0);
    WD_TEST_BOOL(wdStringUtils::CompareN_NoCase(sz, sz, 10, sz + 7, sz + 8) < 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "snprintf")
  {
    // This function has been tested to death during its implementation.
    // That test-code would require several pages, if one would try to test it properly.
    // I am not going to do that here, I am quite confident the function works as expected with pure ASCII strings.
    // So I'm only testing a bit of Utf8 stuff.

    wdStringUtf8 s(L"Abc %s äöü ß %i %s %.4f");
    wdStringUtf8 s2(L"ÄÖÜ");

    char sz[256];
    wdStringUtils::snprintf(sz, 256, s.GetData(), "ASCII", 42, s2.GetData(), 23.31415);

    wdStringUtf8 sC(L"Abc ASCII äöü ß 42 ÄÖÜ 23.3142"); // notice the correct float rounding ;-)

    WD_TEST_STRING(sz, sC.GetData());


    // NaN and Infinity
    wdStringUtils::snprintf(sz, 256, "NaN Value: %.2f", wdMath::NaN<float>());
    WD_TEST_STRING(sz, "NaN Value: NaN");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %.2f", +wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value: Infinity");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %.2f", -wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value: -Infinity");

    wdStringUtils::snprintf(sz, 256, "NaN Value: %.2e", wdMath::NaN<float>());
    WD_TEST_STRING(sz, "NaN Value: NaN");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %.2e", +wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value: Infinity");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %.2e", -wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value: -Infinity");

    wdStringUtils::snprintf(sz, 256, "NaN Value: %+10.2f", wdMath::NaN<float>());
    WD_TEST_STRING(sz, "NaN Value:       +NaN");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %+10.2f", +wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value:  +Infinity");

    wdStringUtils::snprintf(sz, 256, "Inf Value: %+10.2f", -wdMath::Infinity<float>());
    WD_TEST_STRING(sz, "Inf Value:  -Infinity");

    // extended stuff
    wdStringUtils::snprintf(sz, 256, "size: %zu", (size_t)12345678);
    WD_TEST_STRING(sz, "size: 12345678");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StartsWith")
  {
    WD_TEST_BOOL(wdStringUtils::StartsWith(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith("", "") == true);

    WD_TEST_BOOL(wdStringUtils::StartsWith("abc", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith("abc", "") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith(nullptr, "abc") == false);
    WD_TEST_BOOL(wdStringUtils::StartsWith("", "abc") == false);

    WD_TEST_BOOL(wdStringUtils::StartsWith("abc", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith("abcdef", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith("abcdef", "Abc") == false);

    // substring test
    const char* sz = u8"äbc def ghi";
    const wdUInt32 uiByteCount = wdStringUtils::GetStringElementCount(u8"äbc");

    WD_TEST_BOOL(wdStringUtils::StartsWith(sz, u8"äbc", sz + uiByteCount) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith(sz, u8"äbc", sz + uiByteCount - 1) == false);
    WD_TEST_BOOL(wdStringUtils::StartsWith(sz, u8"äbc", sz + 0) == false);

    const char* sz2 = u8"äbc def";
    WD_TEST_BOOL(wdStringUtils::StartsWith(sz, sz2, sz + uiByteCount, sz2 + uiByteCount) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StartsWith_NoCase")
  {
    wdStringUtf8 sL(L"äöü");
    wdStringUtf8 sU(L"ÄÖÜ");

    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("", "") == true);

    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("abc", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("abc", "") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(nullptr, "abc") == false);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("", "abc") == false);

    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("abc", "ABC") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("aBCdef", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase("aBCdef", "bc") == false);

    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(sL.GetData(), sU.GetData()) == true);

    // substring test
    const char* sz = u8"äbc def ghi";
    const wdUInt32 uiByteCount = wdStringUtils::GetStringElementCount(u8"äbc");
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(sz, u8"ÄBC", sz + uiByteCount) == true);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(sz, u8"ÄBC", sz + uiByteCount - 1) == false);
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(sz, u8"ÄBC", sz + 0) == false);

    const char* sz2 = u8"Äbc def";
    WD_TEST_BOOL(wdStringUtils::StartsWith_NoCase(sz, sz2, sz + uiByteCount, sz2 + uiByteCount) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EndsWith")
  {
    WD_TEST_BOOL(wdStringUtils::EndsWith(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith("", "") == true);

    WD_TEST_BOOL(wdStringUtils::EndsWith("abc", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith("abc", "") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith(nullptr, "abc") == false);
    WD_TEST_BOOL(wdStringUtils::EndsWith("", "abc") == false);

    WD_TEST_BOOL(wdStringUtils::EndsWith("abc", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith("abcdef", "def") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith("abcdef", "Def") == false);
    WD_TEST_BOOL(wdStringUtils::EndsWith("def", "abcdef") == false);

    // substring test
    const char* sz = "abc def ghi";
    WD_TEST_BOOL(wdStringUtils::EndsWith(sz, "abc", sz + 3) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith(sz, "def", sz + 7) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith(sz, "def", sz + 8) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EndsWith_NoCase")
  {
    wdStringUtf8 sL(L"äöü");
    wdStringUtf8 sU(L"ÄÖÜ");

    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(nullptr, nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(nullptr, "") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("", "") == true);

    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("abc", nullptr) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("abc", "") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(nullptr, "abc") == false);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("", "abc") == false);

    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("abc", "abc") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("abcdef", "def") == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("abcdef", "Def") == true);

    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase("def", "abcdef") == false);

    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(sL.GetData(), sU.GetData()) == true);

    // substring test
    const char* sz = "abc def ghi";
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(sz, "ABC", sz + 3) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(sz, "DEF", sz + 7) == true);
    WD_TEST_BOOL(wdStringUtils::EndsWith_NoCase(sz, "DEF", sz + 8) == false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindSubString")
  {
    wdStringUtf8 s(L"abc def ghi äöü jkl ßßß abc2 def2 ghi2 äöü2 ß");
    wdStringUtf8 s2(L"äöü");
    wdStringUtf8 s3(L"äöü2");

    const char* szABC = "abc";

    WD_TEST_BOOL(wdStringUtils::FindSubString(szABC, szABC) == szABC);
    WD_TEST_BOOL(wdStringUtils::FindSubString("abc", "") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString("abc", nullptr) == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString(nullptr, "abc") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString("", "abc") == nullptr);

    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "abc") == s.GetData());
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "def") == &s.GetData()[4]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "ghi") == &s.GetData()[8]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), s2.GetData()) == &s.GetData()[12]);

    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "abc2") == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "def2") == &s.GetData()[35]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "ghi2") == &s.GetData()[40]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), s3.GetData()) == &s.GetData()[45]);

    // substring test
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "abc2", s.GetData() + 34) == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindSubString(s.GetData(), "abc2", s.GetData() + 33) == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindSubString_NoCase")
  {
    wdStringUtf8 s(L"abc def ghi äöü jkl ßßß abc2 def2 ghi2 äöü2 ß");
    wdStringUtf8 s2(L"äÖü");
    wdStringUtf8 s3(L"ÄöÜ2");

    const char* szABC = "abc";

    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(szABC, "aBc") == szABC);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase("abc", "") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase("abc", nullptr) == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(nullptr, "abc") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase("", "abc") == nullptr);

    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "Abc") == s.GetData());
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "dEf") == &s.GetData()[4]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "ghI") == &s.GetData()[8]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), s2.GetData()) == &s.GetData()[12]);

    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "abC2") == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "dEf2") == &s.GetData()[35]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "Ghi2") == &s.GetData()[40]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), s3.GetData()) == &s.GetData()[45]);

    // substring test
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "aBc2", s.GetData() + 34) == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindSubString_NoCase(s.GetData(), "abC2", s.GetData() + 33) == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindLastSubString")
  {
    wdStringUtf8 s(L"abc def ghi äöü jkl ßßß abc2 def2 ghi2 äöü2 ß");
    wdStringUtf8 s2(L"äöü");
    wdStringUtf8 s3(L"äöü2");

    const char* szABC = "abc";

    WD_TEST_BOOL(wdStringUtils::FindLastSubString(szABC, szABC) == szABC);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString("abc", "") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString("abc", nullptr) == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(nullptr, "abc") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString("", "abc") == nullptr);

    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), "abc") == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), "def") == &s.GetData()[35]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), "ghi") == &s.GetData()[40]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), s2.GetData()) == &s.GetData()[45]);

    // substring test
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), "abc", nullptr, s.GetData() + 33) == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString(s.GetData(), "abc", nullptr, s.GetData() + 32) == &s.GetData()[0]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindLastSubString_NoCase")
  {
    wdStringUtf8 s(L"abc def ghi äöü jkl ßßß abc2 def2 ghi2 äöü2 ß");
    wdStringUtf8 s2(L"äÖü");
    wdStringUtf8 s3(L"ÄöÜ2");

    const char* szABC = "abc";

    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(szABC, "aBC") == szABC);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase("abc", "") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase("abc", nullptr) == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(nullptr, "abc") == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase("", "abc") == nullptr);

    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), "Abc") == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), "dEf") == &s.GetData()[35]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), "ghI") == &s.GetData()[40]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), s2.GetData()) == &s.GetData()[45]);

    // substring test
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), "ABC", nullptr, s.GetData() + 33) == &s.GetData()[30]);
    WD_TEST_BOOL(wdStringUtils::FindLastSubString_NoCase(s.GetData(), "ABC", nullptr, s.GetData() + 32) == &s.GetData()[0]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindWholeWord")
  {
    wdStringUtf8 s(L"mompfhüßß ßßß öäü abcdef abc def");

    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "abc", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[34]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "def", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[38]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "mompfh", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[0]); // ü is not english

    // substring test
    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "abc", wdStringUtils::IsWordDelimiter_English, s.GetData() + 37) == &s.GetData()[34]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "abc", wdStringUtils::IsWordDelimiter_English, s.GetData() + 36) == nullptr);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord(s.GetData(), "abc", wdStringUtils::IsWordDelimiter_English, s.GetData() + 30) == s.GetData() + 27);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindWholeWord_NoCase")
  {
    wdStringUtf8 s(L"mompfhüßß ßßß öäü abcdef abc def");

    WD_TEST_BOOL(wdStringUtils::FindWholeWord_NoCase(s.GetData(), "ABC", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[34]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord_NoCase(s.GetData(), "DEF", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[38]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord_NoCase(s.GetData(), "momPFH", wdStringUtils::IsWordDelimiter_English) == &s.GetData()[0]);

    // substring test
    WD_TEST_BOOL(
      wdStringUtils::FindWholeWord_NoCase(s.GetData(), "ABC", wdStringUtils::IsWordDelimiter_English, s.GetData() + 37) == &s.GetData()[34]);
    WD_TEST_BOOL(wdStringUtils::FindWholeWord_NoCase(s.GetData(), "ABC", wdStringUtils::IsWordDelimiter_English, s.GetData() + 36) == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindUIntAtTheEnd")
  {
    wdUInt32 uiTestValue = 0;
    wdUInt32 uiCharactersFromStart = 0;

    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(nullptr, uiTestValue, &uiCharactersFromStart).Failed());

    wdStringUtf8 noNumberAtTheEnd(L"ThisStringContainsNoNumberAtTheEnd");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(noNumberAtTheEnd.GetData(), uiTestValue, &uiCharactersFromStart).Failed());

    wdStringUtf8 noNumberAtTheEnd2(L"ThisStringContainsNoNumberAtTheEndBut42InBetween");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(noNumberAtTheEnd.GetData(), uiTestValue, &uiCharactersFromStart).Failed());

    wdStringUtf8 aNumberAtTheEnd(L"ThisStringContainsANumberAtTheEnd1");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(aNumberAtTheEnd.GetData(), uiTestValue, &uiCharactersFromStart).Succeeded());
    WD_TEST_INT(uiTestValue, 1);
    WD_TEST_INT(uiCharactersFromStart, aNumberAtTheEnd.GetElementCount() - 1);

    wdStringUtf8 aZeroLeadingNumberAtTheEnd(L"ThisStringContainsANumberAtTheEnd011129");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(aZeroLeadingNumberAtTheEnd.GetData(), uiTestValue, &uiCharactersFromStart).Succeeded());
    WD_TEST_INT(uiTestValue, 11129);
    WD_TEST_INT(uiCharactersFromStart, aZeroLeadingNumberAtTheEnd.GetElementCount() - 6);

    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(aNumberAtTheEnd.GetData(), uiTestValue, nullptr).Succeeded());
    WD_TEST_INT(uiTestValue, 1);

    wdStringUtf8 twoNumbersInOneString(L"FirstANumber23AndThen42");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(twoNumbersInOneString.GetData(), uiTestValue, &uiCharactersFromStart).Succeeded());
    WD_TEST_INT(uiTestValue, 42);

    wdStringUtf8 onlyANumber(L"55566553");
    WD_TEST_BOOL(wdStringUtils::FindUIntAtTheEnd(onlyANumber.GetData(), uiTestValue, &uiCharactersFromStart).Succeeded());
    WD_TEST_INT(uiTestValue, 55566553);
    WD_TEST_INT(uiCharactersFromStart, 0);
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "SkipCharacters")
  {
    wdStringUtf8 s(L"mompf   hüßß ßßß öäü abcdef abc def");
    const char* szEmpty = "";

    WD_TEST_BOOL(wdStringUtils::SkipCharacters(s.GetData(), wdStringUtils::IsWhiteSpace, false) == &s.GetData()[0]);
    WD_TEST_BOOL(wdStringUtils::SkipCharacters(s.GetData(), wdStringUtils::IsWhiteSpace, true) == &s.GetData()[1]);
    WD_TEST_BOOL(wdStringUtils::SkipCharacters(&s.GetData()[5], wdStringUtils::IsWhiteSpace, false) == &s.GetData()[8]);
    WD_TEST_BOOL(wdStringUtils::SkipCharacters(&s.GetData()[5], wdStringUtils::IsWhiteSpace, true) == &s.GetData()[8]);
    WD_TEST_BOOL(wdStringUtils::SkipCharacters(szEmpty, wdStringUtils::IsWhiteSpace, false) == szEmpty);
    WD_TEST_BOOL(wdStringUtils::SkipCharacters(szEmpty, wdStringUtils::IsWhiteSpace, true) == szEmpty);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindWordEnd")
  {
    wdStringUtf8 s(L"mompf   hüßß ßßß öäü abcdef abc def");
    const char* szEmpty = "";

    WD_TEST_BOOL(wdStringUtils::FindWordEnd(s.GetData(), wdStringUtils::IsWhiteSpace, true) == &s.GetData()[5]);
    WD_TEST_BOOL(wdStringUtils::FindWordEnd(s.GetData(), wdStringUtils::IsWhiteSpace, false) == &s.GetData()[5]);
    WD_TEST_BOOL(wdStringUtils::FindWordEnd(&s.GetData()[5], wdStringUtils::IsWhiteSpace, true) == &s.GetData()[6]);
    WD_TEST_BOOL(wdStringUtils::FindWordEnd(&s.GetData()[5], wdStringUtils::IsWhiteSpace, false) == &s.GetData()[5]);
    WD_TEST_BOOL(wdStringUtils::FindWordEnd(szEmpty, wdStringUtils::IsWhiteSpace, true) == szEmpty);
    WD_TEST_BOOL(wdStringUtils::FindWordEnd(szEmpty, wdStringUtils::IsWhiteSpace, false) == szEmpty);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsWhitespace")
  {
    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace(' '));
    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace('\t'));
    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace('\n'));
    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace('\r'));
    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace('\v'));

    WD_TEST_BOOL(wdStringUtils::IsWhiteSpace('\0') == false);

    for (wdUInt32 i = 33; i < 256; ++i)
    {
      WD_TEST_BOOL(wdStringUtils::IsWhiteSpace(i) == false);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsDecimalDigit / IsHexDigit")
  {
    WD_TEST_BOOL(wdStringUtils::IsDecimalDigit('0'));
    WD_TEST_BOOL(wdStringUtils::IsDecimalDigit('4'));
    WD_TEST_BOOL(wdStringUtils::IsDecimalDigit('9'));
    WD_TEST_BOOL(!wdStringUtils::IsDecimalDigit('/'));
    WD_TEST_BOOL(!wdStringUtils::IsDecimalDigit('A'));

    WD_TEST_BOOL(wdStringUtils::IsHexDigit('0'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('4'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('9'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('A'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('E'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('a'));
    WD_TEST_BOOL(wdStringUtils::IsHexDigit('f'));
    WD_TEST_BOOL(!wdStringUtils::IsHexDigit('g'));
    WD_TEST_BOOL(!wdStringUtils::IsHexDigit('/'));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsWordDelimiter_English / IsIdentifierDelimiter_C_Code")
  {
    for (wdUInt32 i = 0; i < 256; ++i)
    {
      const bool alpha = (i >= 'a' && i <= 'z');
      const bool alpha2 = (i >= 'A' && i <= 'Z');
      const bool num = (i >= '0' && i <= '9');
      const bool dash = i == '-';
      const bool underscore = i == '_';

      const bool bCode = alpha || alpha2 || num || underscore;
      const bool bWord = bCode || dash;


      WD_TEST_BOOL(wdStringUtils::IsWordDelimiter_English(i) == !bWord);
      WD_TEST_BOOL(wdStringUtils::IsIdentifierDelimiter_C_Code(i) == !bCode);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsValidIdentifierName")
  {
    WD_TEST_BOOL(!wdStringUtils::IsValidIdentifierName(""));
    WD_TEST_BOOL(!wdStringUtils::IsValidIdentifierName("1asdf"));
    WD_TEST_BOOL(!wdStringUtils::IsValidIdentifierName("as df"));
    WD_TEST_BOOL(!wdStringUtils::IsValidIdentifierName("asdf!"));

    WD_TEST_BOOL(wdStringUtils::IsValidIdentifierName("asdf1"));
    WD_TEST_BOOL(wdStringUtils::IsValidIdentifierName("_asdf"));
  }
}
