#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Strings, StringBase)
{
  // These tests need not be very through, as wdStringBase only passes through to wdStringUtil
  // which has been tested elsewhere already.
  // Here it is only assured that wdStringBases passes its own pointers properly through,
  // such that the wdStringUtil functions are called correctly.

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEmpty")
  {
    wdStringView it(nullptr);
    WD_TEST_BOOL(it.IsEmpty());

    wdStringView it2("");
    WD_TEST_BOOL(it2.IsEmpty());

    wdStringView it3(nullptr, nullptr);
    WD_TEST_BOOL(it3.IsEmpty());

    const char* sz = "abcdef";

    wdStringView it4(sz, sz);
    WD_TEST_BOOL(it4.IsEmpty());

    wdStringView it5(sz, sz + 1);
    WD_TEST_BOOL(!it5.IsEmpty());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StartsWith")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.StartsWith("abc"));
    WD_TEST_BOOL(it.StartsWith("abcdef"));
    WD_TEST_BOOL(it.StartsWith("")); // empty strings always return true

    wdStringView it2(sz + 3);

    WD_TEST_BOOL(it2.StartsWith("def"));
    WD_TEST_BOOL(it2.StartsWith(""));

    wdStringView it3(sz + 2, sz + 4);
    it3.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it3.StartsWith("d"));
    WD_TEST_BOOL(!it3.StartsWith("de"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "StartsWith_NoCase")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.StartsWith_NoCase("ABC"));
    WD_TEST_BOOL(it.StartsWith_NoCase("abcDEF"));
    WD_TEST_BOOL(it.StartsWith_NoCase("")); // empty strings always return true

    wdStringView it2(sz + 3);

    WD_TEST_BOOL(it2.StartsWith_NoCase("DEF"));
    WD_TEST_BOOL(it2.StartsWith_NoCase(""));

    wdStringView it3(sz + 2, sz + 4);
    it3.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it3.StartsWith_NoCase("D"));
    WD_TEST_BOOL(!it3.StartsWith_NoCase("DE"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EndsWith")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.EndsWith("def"));
    WD_TEST_BOOL(it.EndsWith("abcdef"));
    WD_TEST_BOOL(it.EndsWith("")); // empty strings always return true

    wdStringView it2(sz + 3);

    WD_TEST_BOOL(it2.EndsWith("def"));
    WD_TEST_BOOL(it2.EndsWith(""));

    wdStringView it3(sz + 2, sz + 4);
    it3.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it3.EndsWith("d"));
    WD_TEST_BOOL(!it3.EndsWith("cd"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "EndsWith_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.EndsWith_NoCase("def"));
    WD_TEST_BOOL(it.EndsWith_NoCase("abcdef"));
    WD_TEST_BOOL(it.EndsWith_NoCase("")); // empty strings always return true

    wdStringView it2(sz + 3);

    WD_TEST_BOOL(it2.EndsWith_NoCase("def"));
    WD_TEST_BOOL(it2.EndsWith_NoCase(""));

    wdStringView it3(sz + 2, sz + 4);
    it3.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it3.EndsWith_NoCase("d"));
    WD_TEST_BOOL(!it3.EndsWith_NoCase("cd"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindSubString")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.FindSubString("abcdef") == sz);
    WD_TEST_BOOL(it.FindSubString("abc") == sz);
    WD_TEST_BOOL(it.FindSubString("def") == sz + 3);
    WD_TEST_BOOL(it.FindSubString("cd") == sz + 2);
    WD_TEST_BOOL(it.FindSubString("") == nullptr);
    WD_TEST_BOOL(it.FindSubString(nullptr) == nullptr);
    WD_TEST_BOOL(it.FindSubString("g") == nullptr);

    WD_TEST_BOOL(it.FindSubString("abcdef", sz) == sz);
    WD_TEST_BOOL(it.FindSubString("abcdef", sz + 1) == nullptr);
    WD_TEST_BOOL(it.FindSubString("def", sz + 2) == sz + 3);
    WD_TEST_BOOL(it.FindSubString("def", sz + 3) == sz + 3);
    WD_TEST_BOOL(it.FindSubString("def", sz + 4) == nullptr);
    WD_TEST_BOOL(it.FindSubString("", sz + 3) == nullptr);

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.FindSubString("abcdef") == nullptr);
    WD_TEST_BOOL(it2.FindSubString("abc") == nullptr);
    WD_TEST_BOOL(it2.FindSubString("de") == sz + 3);
    WD_TEST_BOOL(it2.FindSubString("cd") == sz + 2);
    WD_TEST_BOOL(it2.FindSubString("") == nullptr);
    WD_TEST_BOOL(it2.FindSubString(nullptr) == nullptr);
    WD_TEST_BOOL(it2.FindSubString("g") == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindSubString_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.FindSubString_NoCase("abcdef") == sz);
    WD_TEST_BOOL(it.FindSubString_NoCase("abc") == sz);
    WD_TEST_BOOL(it.FindSubString_NoCase("def") == sz + 3);
    WD_TEST_BOOL(it.FindSubString_NoCase("cd") == sz + 2);
    WD_TEST_BOOL(it.FindSubString_NoCase("") == nullptr);
    WD_TEST_BOOL(it.FindSubString_NoCase(nullptr) == nullptr);
    WD_TEST_BOOL(it.FindSubString_NoCase("g") == nullptr);

    WD_TEST_BOOL(it.FindSubString_NoCase("abcdef", sz) == sz);
    WD_TEST_BOOL(it.FindSubString_NoCase("abcdef", sz + 1) == nullptr);
    WD_TEST_BOOL(it.FindSubString_NoCase("def", sz + 2) == sz + 3);
    WD_TEST_BOOL(it.FindSubString_NoCase("def", sz + 3) == sz + 3);
    WD_TEST_BOOL(it.FindSubString_NoCase("def", sz + 4) == nullptr);
    WD_TEST_BOOL(it.FindSubString_NoCase("", sz + 3) == nullptr);


    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.FindSubString_NoCase("abcdef") == nullptr);
    WD_TEST_BOOL(it2.FindSubString_NoCase("abc") == nullptr);
    WD_TEST_BOOL(it2.FindSubString_NoCase("de") == sz + 3);
    WD_TEST_BOOL(it2.FindSubString_NoCase("cd") == sz + 2);
    WD_TEST_BOOL(it2.FindSubString_NoCase("") == nullptr);
    WD_TEST_BOOL(it2.FindSubString_NoCase(nullptr) == nullptr);
    WD_TEST_BOOL(it2.FindSubString_NoCase("g") == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindLastSubString")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.FindLastSubString("abcdef") == sz);
    WD_TEST_BOOL(it.FindLastSubString("abc") == sz);
    WD_TEST_BOOL(it.FindLastSubString("def") == sz + 3);
    WD_TEST_BOOL(it.FindLastSubString("cd") == sz + 2);
    WD_TEST_BOOL(it.FindLastSubString("") == nullptr);
    WD_TEST_BOOL(it.FindLastSubString(nullptr) == nullptr);
    WD_TEST_BOOL(it.FindLastSubString("g") == nullptr);

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.FindLastSubString("abcdef") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString("abc") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString("de") == sz + 3);
    WD_TEST_BOOL(it2.FindLastSubString("cd") == sz + 2);
    WD_TEST_BOOL(it2.FindLastSubString("") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString(nullptr) == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString("g") == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindLastSubString_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.FindLastSubString_NoCase("abcdef") == sz);
    WD_TEST_BOOL(it.FindLastSubString_NoCase("abc") == sz);
    WD_TEST_BOOL(it.FindLastSubString_NoCase("def") == sz + 3);
    WD_TEST_BOOL(it.FindLastSubString_NoCase("cd") == sz + 2);
    WD_TEST_BOOL(it.FindLastSubString_NoCase("") == nullptr);
    WD_TEST_BOOL(it.FindLastSubString_NoCase(nullptr) == nullptr);
    WD_TEST_BOOL(it.FindLastSubString_NoCase("g") == nullptr);

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.FindLastSubString_NoCase("abcdef") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase("abc") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase("de") == sz + 3);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase("cd") == sz + 2);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase("") == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase(nullptr) == nullptr);
    WD_TEST_BOOL(it2.FindLastSubString_NoCase("g") == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.Compare("abcdef") == 0);
    WD_TEST_BOOL(it.Compare("abcde") > 0);
    WD_TEST_BOOL(it.Compare("abcdefg") < 0);

    wdStringView it2(sz + 2, sz + 5);
    it2.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it2.Compare("de") == 0);
    WD_TEST_BOOL(it2.Compare("def") < 0);
    WD_TEST_BOOL(it2.Compare("d") > 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Compare_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.Compare_NoCase("abcdef") == 0);
    WD_TEST_BOOL(it.Compare_NoCase("abcde") > 0);
    WD_TEST_BOOL(it.Compare_NoCase("abcdefg") < 0);

    wdStringView it2(sz + 2, sz + 5);
    it2.SetStartPosition(sz + 3);

    WD_TEST_BOOL(it2.Compare_NoCase("de") == 0);
    WD_TEST_BOOL(it2.Compare_NoCase("def") < 0);
    WD_TEST_BOOL(it2.Compare_NoCase("d") > 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareN")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.CompareN("abc", 3) == 0);
    WD_TEST_BOOL(it.CompareN("abcde", 6) > 0);
    WD_TEST_BOOL(it.CompareN("abcg", 3) == 0);

    wdStringView it2(sz + 2, sz + 5);

    WD_TEST_BOOL(it2.CompareN("cd", 2) == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "CompareN_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.CompareN_NoCase("abc", 3) == 0);
    WD_TEST_BOOL(it.CompareN_NoCase("abcde", 6) > 0);
    WD_TEST_BOOL(it.CompareN_NoCase("abcg", 3) == 0);

    wdStringView it2(sz + 2, sz + 5);

    WD_TEST_BOOL(it2.CompareN_NoCase("cd", 2) == 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.IsEqual("abcdef"));
    WD_TEST_BOOL(!it.IsEqual("abcde"));
    WD_TEST_BOOL(!it.IsEqual("abcdefg"));

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.IsEqual("cde"));
    WD_TEST_BOOL(!it2.IsEqual("bcde"));
    WD_TEST_BOOL(!it2.IsEqual("cdef"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.IsEqual_NoCase("abcdef"));
    WD_TEST_BOOL(!it.IsEqual_NoCase("abcde"));
    WD_TEST_BOOL(!it.IsEqual_NoCase("abcdefg"));

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.IsEqual_NoCase("cde"));
    WD_TEST_BOOL(!it2.IsEqual_NoCase("bcde"));
    WD_TEST_BOOL(!it2.IsEqual_NoCase("cdef"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualN")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.IsEqualN("abcGHI", 3));
    WD_TEST_BOOL(!it.IsEqualN("abcGHI", 4));

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.IsEqualN("cdeZX", 3));
    WD_TEST_BOOL(!it2.IsEqualN("cdeZX", 4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqualN_NoCase")
  {
    const char* sz = "ABCDEF";
    wdStringView it(sz);

    WD_TEST_BOOL(it.IsEqualN_NoCase("abcGHI", 3));
    WD_TEST_BOOL(!it.IsEqualN_NoCase("abcGHI", 4));

    wdStringView it2(sz + 1, sz + 5);
    it2.SetStartPosition(sz + 2);

    WD_TEST_BOOL(it2.IsEqualN_NoCase("cdeZX", 3));
    WD_TEST_BOOL(!it2.IsEqualN_NoCase("cdeZX", 4));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator==/!=")
  {
    const char* sz = "abcdef";
    const char* sz2 = "blabla";
    wdStringView it(sz);
    wdStringView it2(sz);
    wdStringView it3(sz2);

    WD_TEST_BOOL(it == sz);
    WD_TEST_BOOL(sz == it);
    WD_TEST_BOOL(it == "abcdef");
    WD_TEST_BOOL("abcdef" == it);
    WD_TEST_BOOL(it == it);
    WD_TEST_BOOL(it == it2);

    WD_TEST_BOOL(it != sz2);
    WD_TEST_BOOL(sz2 != it);
    WD_TEST_BOOL(it != "blabla");
    WD_TEST_BOOL("blabla" != it);
    WD_TEST_BOOL(it != it3);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "substring operator ==/!=/</>/<=/>=")
  {

    const char* sz1 = "aaabbbcccddd";
    const char* sz2 = "aaabbbdddeee";

    wdStringView it1(sz1 + 3, sz1 + 6);
    wdStringView it2(sz2 + 3, sz2 + 6);

    WD_TEST_BOOL(it1 == it1);
    WD_TEST_BOOL(it2 == it2);

    WD_TEST_BOOL(it1 == it2);
    WD_TEST_BOOL(!(it1 != it2));
    WD_TEST_BOOL(!(it1 < it2));
    WD_TEST_BOOL(!(it1 > it2));
    WD_TEST_BOOL(it1 <= it2);
    WD_TEST_BOOL(it1 >= it2);

    it1 = wdStringView(sz1 + 3, sz1 + 7);
    it2 = wdStringView(sz2 + 3, sz2 + 7);

    WD_TEST_BOOL(it1 == it1);
    WD_TEST_BOOL(it2 == it2);

    WD_TEST_BOOL(it1 != it2);
    WD_TEST_BOOL(!(it1 == it2));

    WD_TEST_BOOL(it1 < it2);
    WD_TEST_BOOL(!(it1 > it2));
    WD_TEST_BOOL(it1 <= it2);
    WD_TEST_BOOL(!(it1 >= it2));

    WD_TEST_BOOL(it2 > it1);
    WD_TEST_BOOL(!(it2 < it1));
    WD_TEST_BOOL(it2 >= it1);
    WD_TEST_BOOL(!(it2 <= it1));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator</>")
  {
    const char* sz = "abcdef";
    const char* sz2 = "abcdefg";
    wdStringView it(sz);
    wdStringView it2(sz2);

    WD_TEST_BOOL(it < sz2);
    WD_TEST_BOOL(sz < it2);
    WD_TEST_BOOL(it < it2);

    WD_TEST_BOOL(sz2 > it);
    WD_TEST_BOOL(it2 > sz);
    WD_TEST_BOOL(it2 > it);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator<=/>=")
  {
    {
      const char* sz = "abcdef";
      const char* sz2 = "abcdefg";
      wdStringView it(sz);
      wdStringView it2(sz2);

      WD_TEST_BOOL(it <= sz2);
      WD_TEST_BOOL(sz <= it2);
      WD_TEST_BOOL(it <= it2);

      WD_TEST_BOOL(sz2 >= it);
      WD_TEST_BOOL(it2 >= sz);
      WD_TEST_BOOL(it2 >= it);
    }

    {
      const char* sz = "abcdef";
      const char* sz2 = "abcdef";
      wdStringView it(sz);
      wdStringView it2(sz2);

      WD_TEST_BOOL(it <= sz2);
      WD_TEST_BOOL(sz <= it2);
      WD_TEST_BOOL(it <= it2);

      WD_TEST_BOOL(sz2 >= it);
      WD_TEST_BOOL(it2 >= sz);
      WD_TEST_BOOL(it2 >= it);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindWholeWord")
  {
    wdStringUtf8 s(L"abc def mompfhüßß ßßß öäü abcdef abc def abc def");
    wdStringView it(s.GetData() + 8, s.GetData() + s.GetElementCount() - 8);
    wdStringView it2(s.GetData() + 8, s.GetData() + s.GetElementCount());

    WD_TEST_BOOL(it.FindWholeWord("abc", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it.FindWholeWord("def", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[38]);
    WD_TEST_BOOL(
      it.FindWholeWord("mompfh", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[0]); // ü is not English (thus a delimiter)

    WD_TEST_BOOL(it.FindWholeWord("abc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 34) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it.FindWholeWord("abc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 35) == nullptr);

    WD_TEST_BOOL(it2.FindWholeWord("abc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 34) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it2.FindWholeWord("abc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 35) == &it.GetStartPointer()[42]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "FindWholeWord_NoCase")
  {
    wdStringUtf8 s(L"abc def mompfhüßß ßßß öäü abcdef abc def abc def");
    wdStringView it(s.GetData() + 8, s.GetData() + s.GetElementCount() - 8);
    wdStringView it2(s.GetData() + 8, s.GetData() + s.GetElementCount());

    WD_TEST_BOOL(it.FindWholeWord_NoCase("ABC", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it.FindWholeWord_NoCase("DEF", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[38]);
    WD_TEST_BOOL(it.FindWholeWord_NoCase("momPFH", wdStringUtils::IsWordDelimiter_English) == &it.GetStartPointer()[0]);

    WD_TEST_BOOL(it.FindWholeWord_NoCase("ABc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 34) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it.FindWholeWord_NoCase("ABc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 35) == nullptr);

    WD_TEST_BOOL(it2.FindWholeWord_NoCase("ABc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 34) == &it.GetStartPointer()[34]);
    WD_TEST_BOOL(it2.FindWholeWord_NoCase("ABc", wdStringUtils::IsWordDelimiter_English, it.GetStartPointer() + 35) == &it.GetStartPointer()[42]);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ComputeCharacterPosition")
  {
    const wchar_t* sz = L"mompfhüßß ßßß öäü abcdef abc def abc def";
    wdStringBuilder s(sz);

    WD_TEST_STRING(s.ComputeCharacterPosition(14), wdStringUtf8(L"öäü abcdef abc def abc def").GetData());
  }
}
