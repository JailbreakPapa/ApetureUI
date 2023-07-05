#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Memory/CommonAllocators.h>
#include <Foundation/Strings/String.h>

// this file takes ages to compile in a Release build
// since we don't care for runtime performance, just disable all optimizations
#pragma optimize("", off)

WD_CREATE_SIMPLE_TEST(Strings, StringBuilder)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(empty)")
  {
    wdStringBuilder s;

    WD_TEST_BOOL(s.IsEmpty());
    WD_TEST_INT(s.GetCharacterCount(), 0);
    WD_TEST_INT(s.GetElementCount(), 0);
    WD_TEST_BOOL(s.IsPureASCII());
    WD_TEST_BOOL(s == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(Utf8)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s(sUtf8.GetData());

    WD_TEST_BOOL(s.GetData() != sUtf8.GetData());
    WD_TEST_BOOL(s == sUtf8.GetData());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s.IsPureASCII());

    wdStringBuilder s2("test test");

    WD_TEST_BOOL(s2 == "test test");
    WD_TEST_INT(s2.GetElementCount(), 9);
    WD_TEST_INT(s2.GetCharacterCount(), 9);
    WD_TEST_BOOL(s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(wchar_t)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s(L"abc äöü € def");

    WD_TEST_BOOL(s == sUtf8.GetData());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s.IsPureASCII());

    wdStringBuilder s2(L"test test");

    WD_TEST_BOOL(s2 == "test test");
    WD_TEST_INT(s2.GetElementCount(), 9);
    WD_TEST_INT(s2.GetCharacterCount(), 9);
    WD_TEST_BOOL(s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(copy)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s(L"abc äöü € def");
    wdStringBuilder s2(s);

    WD_TEST_BOOL(s2 == sUtf8.GetData());
    WD_TEST_INT(s2.GetElementCount(), 18);
    WD_TEST_INT(s2.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(StringView)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");

    wdStringView it(sUtf8.GetData() + 2, sUtf8.GetData() + 8);

    wdStringBuilder s(it);

    WD_TEST_INT(s.GetElementCount(), 6);
    WD_TEST_INT(s.GetCharacterCount(), 4);
    WD_TEST_BOOL(!s.IsPureASCII());
    WD_TEST_BOOL(s == wdStringUtf8(L"c äö").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor(multiple)")
  {
    wdStringUtf8 sUtf8(L"⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺");
    wdStringUtf8 sUtf2(L"⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺");

    wdStringBuilder sb(sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData());

    WD_TEST_STRING(sb.GetData(), sUtf2.GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=(Utf8)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s("bla");
    s = sUtf8.GetData();

    WD_TEST_BOOL(s.GetData() != sUtf8.GetData());
    WD_TEST_BOOL(s == sUtf8.GetData());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s.IsPureASCII());

    wdStringBuilder s2("bla");
    s2 = "test test";

    WD_TEST_BOOL(s2 == "test test");
    WD_TEST_INT(s2.GetElementCount(), 9);
    WD_TEST_INT(s2.GetCharacterCount(), 9);
    WD_TEST_BOOL(s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=(wchar_t)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s("bla");
    s = L"abc äöü € def";

    WD_TEST_BOOL(s == sUtf8.GetData());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s.IsPureASCII());

    wdStringBuilder s2("bla");
    s2 = L"test test";

    WD_TEST_BOOL(s2 == "test test");
    WD_TEST_INT(s2.GetElementCount(), 9);
    WD_TEST_INT(s2.GetCharacterCount(), 9);
    WD_TEST_BOOL(s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=(copy)")
  {
    wdStringUtf8 sUtf8(L"abc äöü € def");
    wdStringBuilder s(L"abc äöü € def");
    wdStringBuilder s2;
    s2 = s;

    WD_TEST_BOOL(s2 == sUtf8.GetData());
    WD_TEST_INT(s2.GetElementCount(), 18);
    WD_TEST_INT(s2.GetCharacterCount(), 13);
    WD_TEST_BOOL(!s2.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=(StringView)")
  {
    wdStringBuilder s("abcdefghi");
    wdStringView it(s.GetData() + 2, s.GetData() + 8);
    it.SetStartPosition(s.GetData() + 3);

    s = it;

    WD_TEST_BOOL(s == "defgh");
    WD_TEST_INT(s.GetElementCount(), 5);
    WD_TEST_INT(s.GetCharacterCount(), 5);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "convert to wdStringView")
  {
    wdStringBuilder s(L"aölsdföasld");
    wdStringBuilder tmp;

    wdStringView sv = s;

    WD_TEST_STRING(sv.GetData(tmp), wdStringUtf8(L"aölsdföasld").GetData());
    WD_TEST_BOOL(sv == wdStringUtf8(L"aölsdföasld").GetData());

    s = "abcdef";

    WD_TEST_STRING(sv.GetStartPointer(), "abcdef");
    WD_TEST_BOOL(sv == "abcdef");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    wdStringBuilder s(L"abc äöü € def");

    WD_TEST_BOOL(!s.IsEmpty());
    WD_TEST_BOOL(!s.IsPureASCII());

    s.Clear();
    WD_TEST_BOOL(s.IsEmpty());
    WD_TEST_INT(s.GetElementCount(), 0);
    WD_TEST_INT(s.GetCharacterCount(), 0);
    WD_TEST_BOOL(s.IsPureASCII());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetElementCount / GetCharacterCount / IsPureASCII")
  {
    wdStringBuilder s(L"abc äöü € def");

    WD_TEST_BOOL(!s.IsPureASCII());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 13);

    s = "abc";

    WD_TEST_BOOL(s.IsPureASCII());
    WD_TEST_INT(s.GetElementCount(), 3);
    WD_TEST_INT(s.GetCharacterCount(), 3);

    s = L"Hällo! I love €";

    WD_TEST_BOOL(!s.IsPureASCII());
    WD_TEST_INT(s.GetElementCount(), 18);
    WD_TEST_INT(s.GetCharacterCount(), 15);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Append(single unicode char)")
  {
    wdStringUtf32 u32(L"äöüß");

    wdStringBuilder s("abc");
    WD_TEST_INT(s.GetCharacterCount(), 3);
    s.Append(u32.GetData()[0]);
    WD_TEST_INT(s.GetCharacterCount(), 4);

    WD_TEST_BOOL(s == wdStringUtf8(L"abcä").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Prepend(single unicode char)")
  {
    wdStringUtf32 u32(L"äöüß");

    wdStringBuilder s("abc");
    WD_TEST_INT(s.GetCharacterCount(), 3);
    s.Prepend(u32.GetData()[0]);
    WD_TEST_INT(s.GetCharacterCount(), 4);

    WD_TEST_BOOL(s == wdStringUtf8(L"äabc").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Append(char)")
  {
    wdStringBuilder s("abc");
    WD_TEST_INT(s.GetCharacterCount(), 3);
    s.Append("de", "fg", "hi", wdStringUtf8(L"öä").GetData(), "jk", wdStringUtf8(L"ü€").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 15);

    WD_TEST_BOOL(s == wdStringUtf8(L"abcdefghiöäjkü€").GetData());

    s = "pups";
    s.Append(nullptr, "b", nullptr, "d", nullptr, wdStringUtf8(L"ü€").GetData());
    WD_TEST_BOOL(s == wdStringUtf8(L"pupsbdü€").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Append(wchar_t)")
  {
    wdStringBuilder s("abc");
    WD_TEST_INT(s.GetCharacterCount(), 3);
    s.Append(L"de", L"fg", L"hi", L"öä", L"jk", L"ü€");
    WD_TEST_INT(s.GetCharacterCount(), 15);

    WD_TEST_BOOL(s == wdStringUtf8(L"abcdefghiöäjkü€").GetData());

    s = "pups";
    s.Append(nullptr, L"b", nullptr, L"d", nullptr, L"ü€");
    WD_TEST_BOOL(s == wdStringUtf8(L"pupsbdü€").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Append(multiple)")
  {
    wdStringUtf8 sUtf8(L"⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺");
    wdStringUtf8 sUtf2(L"Test⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺Test2");

    wdStringBuilder sb("Test");
    sb.Append(sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), "Test2");

    WD_TEST_STRING(sb.GetData(), sUtf2.GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Set(multiple)")
  {
    wdStringUtf8 sUtf8(L"⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺");
    wdStringUtf8 sUtf2(L"⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺⺅⻩⽇⿕〄㈷㑧䆴ظؼݻ༺Test2");

    wdStringBuilder sb("Test");
    sb.Set(sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), sUtf8.GetData(), "Test2");

    WD_TEST_STRING(sb.GetData(), sUtf2.GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "AppendFormat")
  {
    wdStringBuilder s("abc");
    s.AppendFormat("Test{0}{1}{2}", 42, "foo", wdStringUtf8(L"bär").GetData());

    WD_TEST_BOOL(s == wdStringUtf8(L"abcTest42foobär").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Prepend(char)")
  {
    wdStringBuilder s("abc");
    s.Prepend("de", "fg", "hi", wdStringUtf8(L"öä").GetData(), "jk", wdStringUtf8(L"ü€").GetData());

    WD_TEST_BOOL(s == wdStringUtf8(L"defghiöäjkü€abc").GetData());

    s = "pups";
    s.Prepend(nullptr, "b", nullptr, "d", nullptr, wdStringUtf8(L"ü€").GetData());
    WD_TEST_BOOL(s == wdStringUtf8(L"bdü€pups").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Prepend(wchar_t)")
  {
    wdStringBuilder s("abc");
    s.Prepend(L"de", L"fg", L"hi", L"öä", L"jk", L"ü€");

    WD_TEST_BOOL(s == wdStringUtf8(L"defghiöäjkü€abc").GetData());

    s = "pups";
    s.Prepend(nullptr, L"b", nullptr, L"d", nullptr, L"ü€");
    WD_TEST_BOOL(s == wdStringUtf8(L"bdü€pups").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PrependFormat")
  {
    wdStringBuilder s("abc");
    s.PrependFormat("Test{0}{1}{2}", 42, "foo", wdStringUtf8(L"bär").GetData());

    WD_TEST_BOOL(s == wdStringUtf8(L"Test42foobärabc").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Printf")
  {
    wdStringBuilder s("abc");
    s.Printf("Test%i%s%s", 42, "foo", wdStringUtf8(L"bär").GetData());

    WD_TEST_BOOL(s == wdStringUtf8(L"Test42foobär").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Format")
  {
    wdStringBuilder s("abc");
    s.Format("Test{0}{1}{2}", 42, "foo", wdStringUtf8(L"bär").GetData());

    WD_TEST_BOOL(s == wdStringUtf8(L"Test42foobär").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToUpper")
  {
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    s.ToUpper();
    WD_TEST_BOOL(s == wdStringUtf8(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ€ß").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ToLower")
  {
    wdStringBuilder s(L"ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ€ß");
    s.ToLower();
    WD_TEST_BOOL(s == wdStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Shrink")
  {
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    s.Shrink(5, 3);

    WD_TEST_BOOL(s == wdStringUtf8(L"fghijklmnopqrstuvwxyzäö").GetData());

    s.Shrink(9, 7);
    WD_TEST_BOOL(s == wdStringUtf8(L"opqrstu").GetData());

    s.Shrink(3, 2);
    WD_TEST_BOOL(s == wdStringUtf8(L"rs").GetData());

    s.Shrink(1, 0);
    WD_TEST_BOOL(s == wdStringUtf8(L"s").GetData());

    s.Shrink(0, 0);
    WD_TEST_BOOL(s == wdStringUtf8(L"s").GetData());

    s.Shrink(0, 1);
    WD_TEST_BOOL(s == wdStringUtf8(L"").GetData());

    s.Shrink(10, 0);
    WD_TEST_BOOL(s == wdStringUtf8(L"").GetData());

    s.Shrink(0, 10);
    WD_TEST_BOOL(s == wdStringUtf8(L"").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Reserve")
  {
    wdHeapAllocator allocator("reserve test allocator");
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß", &allocator);
    wdUInt32 characterCountBefore = s.GetCharacterCount();

    s.Reserve(2048);

    WD_TEST_BOOL(s.GetCharacterCount() == characterCountBefore);

    wdUInt64 iNumAllocs = allocator.GetStats().m_uiNumAllocations;
    s.Append("blablablablablablablablablablablablablablablablablablablablablablablablablablablablablabla");
    WD_TEST_BOOL(iNumAllocs == allocator.GetStats().m_uiNumAllocations);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Convert to StringView")
  {
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");
    wdStringView it = s;

    WD_TEST_BOOL(it.StartsWith(wdStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));
    WD_TEST_BOOL(it.EndsWith(wdStringUtf8(L"abcdefghijklmnopqrstuvwxyzäöü€ß").GetData()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeCharacter")
  {
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");

    wdStringUtf8 upr(L"ÄÖÜ€ßABCDEFGHIJKLMNOPQRSTUVWXYZ");
    wdStringView view(upr.GetData());

    for (auto it = begin(s); it.IsValid(); ++it, view.Shrink(1, 0))
    {
      s.ChangeCharacter(it, view.GetCharacter());

      WD_TEST_BOOL(it.GetCharacter() == view.GetCharacter()); // iterator reflects the changes
    }

    WD_TEST_BOOL(s == upr.GetData());
    WD_TEST_INT(s.GetCharacterCount(), 31);
    WD_TEST_INT(s.GetElementCount(), 37);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceSubString")
  {
    wdStringBuilder s(L"abcdefghijklmnopqrstuvwxyzäöü€ß");

    s.ReplaceSubString(s.GetData() + 3, s.GetData() + 7, "DEFG"); // equal length, equal num characters
    WD_TEST_BOOL(s == wdStringUtf8(L"abcDEFGhijklmnopqrstuvwxyzäöü€ß").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 31);
    WD_TEST_INT(s.GetElementCount(), 37);

    s.ReplaceSubString(s.GetData() + 7, s.GetData() + 15, ""); // remove
    WD_TEST_BOOL(s == wdStringUtf8(L"abcDEFGpqrstuvwxyzäöü€ß").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 23);
    WD_TEST_INT(s.GetElementCount(), 29);

    s.ReplaceSubString(s.GetData() + 17, s.GetData() + 22, "blablub"); // make longer
    WD_TEST_BOOL(s == wdStringUtf8(L"abcDEFGpqrstuvwxyblablubü€ß").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 27);
    WD_TEST_INT(s.GetElementCount(), 31);

    s.ReplaceSubString(s.GetData() + 22, s.GetData() + 22, wdStringUtf8(L"määh!").GetData()); // insert
    WD_TEST_BOOL(s == wdStringUtf8(L"abcDEFGpqrstuvwxyblablmääh!ubü€ß").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 32);
    WD_TEST_INT(s.GetElementCount(), 38);

    s.ReplaceSubString(s.GetData(), s.GetData() + 10, nullptr); // remove at front
    WD_TEST_BOOL(s == wdStringUtf8(L"stuvwxyblablmääh!ubü€ß").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 22);
    WD_TEST_INT(s.GetElementCount(), 28);

    s.ReplaceSubString(s.GetData() + 18, s.GetData() + 28, nullptr); // remove at back
    WD_TEST_BOOL(s == wdStringUtf8(L"stuvwxyblablmääh").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 16);
    WD_TEST_INT(s.GetElementCount(), 18);

    s.ReplaceSubString(s.GetData(), s.GetData() + 18, nullptr); // clear
    WD_TEST_BOOL(s == wdStringUtf8(L"").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 0);
    WD_TEST_INT(s.GetElementCount(), 0);

    const char* szInsert = "abc def ghi";

    s.ReplaceSubString(s.GetData(), s.GetData(), wdStringView(szInsert, szInsert + 7)); // partial insert into empty
    WD_TEST_BOOL(s == wdStringUtf8(L"abc def").GetData());
    WD_TEST_INT(s.GetCharacterCount(), 7);
    WD_TEST_INT(s.GetElementCount(), 7);

    // insert very large block
    s = wdStringBuilder("a"); // hard reset to keep buffer small
    wdString insertString("omfg this string is so long it possibly won't never ever ever ever fit into the current buffer - this will "
                          "hopefully lead to a buffer resize :)"
                          "................................................................................................................"
                          "........................................................"
                          "................................................................................................................"
                          "........................................................"
                          "................................................................................................................"
                          "........................................................"
                          "................................................................................................................"
                          "........................................................"
                          "................................................................................................................"
                          "........................................................"
                          "................................................................................................................"
                          "........................................................");
    s.ReplaceSubString(s.GetData(), s.GetData() + s.GetElementCount(), insertString.GetData());
    WD_TEST_BOOL(s == insertString.GetData());
    WD_TEST_INT(s.GetCharacterCount(), insertString.GetCharacterCount());
    WD_TEST_INT(s.GetElementCount(), insertString.GetElementCount());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Insert")
  {
    wdStringBuilder s;

    s.Insert(s.GetData(), "test");
    WD_TEST_BOOL(s == "test");

    s.Insert(s.GetData() + 2, "TUT");
    WD_TEST_BOOL(s == "teTUTst");

    s.Insert(s.GetData(), "MOEP");
    WD_TEST_BOOL(s == "MOEPteTUTst");

    s.Insert(s.GetData() + s.GetElementCount(), "hompf");
    WD_TEST_BOOL(s == "MOEPteTUTsthompf");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove")
  {
    wdStringBuilder s("MOEPteTUTsthompf");

    s.Remove(s.GetData() + 11, s.GetData() + s.GetElementCount());
    WD_TEST_BOOL(s == "MOEPteTUTst");

    s.Remove(s.GetData(), s.GetData() + 4);
    WD_TEST_BOOL(s == "teTUTst");

    s.Remove(s.GetData() + 2, s.GetData() + 5);
    WD_TEST_BOOL(s == "test");

    s.Remove(s.GetData(), s.GetData() + s.GetElementCount());
    WD_TEST_BOOL(s == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceFirst")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceFirst("def", "BLOED");
    WD_TEST_BOOL(s == "abc BLOED abc def ghi abc ghi");

    s.ReplaceFirst("abc", "BLOED");
    WD_TEST_BOOL(s == "BLOED BLOED abc def ghi abc ghi");

    s.ReplaceFirst("abc", "BLOED", s.GetData() + 15);
    WD_TEST_BOOL(s == "BLOED BLOED abc def ghi BLOED ghi");

    s.ReplaceFirst("ghi", "LAANGWEILIG");
    WD_TEST_BOOL(s == "BLOED BLOED abc def LAANGWEILIG BLOED ghi");

    s.ReplaceFirst("ghi", "LAANGWEILIG");
    WD_TEST_BOOL(s == "BLOED BLOED abc def LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("def", "OEDE");
    WD_TEST_BOOL(s == "BLOED BLOED abc OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("abc", "BLOEDE");
    WD_TEST_BOOL(s == "BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst("BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG", "weg");
    WD_TEST_BOOL(s == "weg");

    s.ReplaceFirst("weg", nullptr);
    WD_TEST_BOOL(s == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceLast")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceLast("abc", "ABC");
    WD_TEST_BOOL(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast("abc", "ABC");
    WD_TEST_BOOL(s == "abc def ABC def ghi ABC ghi");

    s.ReplaceLast("abc", "ABC");
    WD_TEST_BOOL(s == "ABC def ABC def ghi ABC ghi");

    s.ReplaceLast("ghi", "GHI", s.GetData() + 24);
    WD_TEST_BOOL(s == "ABC def ABC def GHI ABC ghi");

    s.ReplaceLast("i", "I");
    WD_TEST_BOOL(s == "ABC def ABC def GHI ABC ghI");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceAll")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceAll("abc", "TEST");
    WD_TEST_BOOL(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll("def", "def");
    WD_TEST_BOOL(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll("def", "defdef");
    WD_TEST_BOOL(s == "TEST defdef TEST defdef ghi TEST ghi");

    s.ReplaceAll("def", "defdef");
    WD_TEST_BOOL(s == "TEST defdefdefdef TEST defdefdefdef ghi TEST ghi");

    s.ReplaceAll("def", " ");
    WD_TEST_BOOL(s == "TEST      TEST      ghi TEST ghi");

    s.ReplaceAll(" ", "");
    WD_TEST_BOOL(s == "TESTTESTghiTESTghi");

    s.ReplaceAll("TEST", "a");
    WD_TEST_BOOL(s == "aaghiaghi");

    s.ReplaceAll("hi", "hihi");
    WD_TEST_BOOL(s == "aaghihiaghihi");

    s.ReplaceAll("ag", " ");
    WD_TEST_BOOL(s == "a hihi hihi");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceFirst_NoCase")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceFirst_NoCase("DEF", "BLOED");
    WD_TEST_BOOL(s == "abc BLOED abc def ghi abc ghi");

    s.ReplaceFirst_NoCase("ABC", "BLOED");
    WD_TEST_BOOL(s == "BLOED BLOED abc def ghi abc ghi");

    s.ReplaceFirst_NoCase("ABC", "BLOED", s.GetData() + 15);
    WD_TEST_BOOL(s == "BLOED BLOED abc def ghi BLOED ghi");

    s.ReplaceFirst_NoCase("GHI", "LAANGWEILIG");
    WD_TEST_BOOL(s == "BLOED BLOED abc def LAANGWEILIG BLOED ghi");

    s.ReplaceFirst_NoCase("GHI", "LAANGWEILIG");
    WD_TEST_BOOL(s == "BLOED BLOED abc def LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("DEF", "OEDE");
    WD_TEST_BOOL(s == "BLOED BLOED abc OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("ABC", "BLOEDE");
    WD_TEST_BOOL(s == "BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG");

    s.ReplaceFirst_NoCase("BLOED BLOED BLOEDE OEDE LAANGWEILIG BLOED LAANGWEILIG", "weg");
    WD_TEST_BOOL(s == "weg");

    s.ReplaceFirst_NoCase("WEG", nullptr);
    WD_TEST_BOOL(s == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceLast_NoCase")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceLast_NoCase("abc", "ABC");
    WD_TEST_BOOL(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("aBc", "ABC");
    WD_TEST_BOOL(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("ABC", "ABC");
    WD_TEST_BOOL(s == "abc def abc def ghi ABC ghi");

    s.ReplaceLast_NoCase("GHI", "GHI", s.GetData() + 24);
    WD_TEST_BOOL(s == "abc def abc def GHI ABC ghi");

    s.ReplaceLast_NoCase("I", "I");
    WD_TEST_BOOL(s == "abc def abc def GHI ABC ghI");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceAll_NoCase")
  {
    wdStringBuilder s = "abc def abc def ghi abc ghi";

    s.ReplaceAll_NoCase("ABC", "TEST");
    WD_TEST_BOOL(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "def");
    WD_TEST_BOOL(s == "TEST def TEST def ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "defdef");
    WD_TEST_BOOL(s == "TEST defdef TEST defdef ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", "defdef");
    WD_TEST_BOOL(s == "TEST defdefdefdef TEST defdefdefdef ghi TEST ghi");

    s.ReplaceAll_NoCase("DEF", " ");
    WD_TEST_BOOL(s == "TEST      TEST      ghi TEST ghi");

    s.ReplaceAll_NoCase(" ", "");
    WD_TEST_BOOL(s == "TESTTESTghiTESTghi");

    s.ReplaceAll_NoCase("teST", "a");
    WD_TEST_BOOL(s == "aaghiaghi");

    s.ReplaceAll_NoCase("hI", "hihi");
    WD_TEST_BOOL(s == "aaghihiaghihi");

    s.ReplaceAll_NoCase("Ag", " ");
    WD_TEST_BOOL(s == "a hihi hihi");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceWholeWord")
  {
    wdStringBuilder s = "abcd abc abcd abc dabc abc";

    WD_TEST_BOOL(s.ReplaceWholeWord("abc", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd abc dabc abc");

    WD_TEST_BOOL(s.ReplaceWholeWord("abc", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc abc");

    WD_TEST_BOOL(s.ReplaceWholeWord("abc", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord("abc", "def", wdStringUtils::IsWordDelimiter_English) == nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord("abcd", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "def def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord("abcd", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "def def def def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord("abcd", "def", wdStringUtils::IsWordDelimiter_English) == nullptr);
    WD_TEST_BOOL(s == "def def def def dabc def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceWholeWord_NoCase")
  {
    wdStringBuilder s = "abcd abc abcd abc dabc abc";

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd abc dabc abc");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc abc");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English) == nullptr);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABCd", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "def def abcd def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("aBCD", "def", wdStringUtils::IsWordDelimiter_English) != nullptr);
    WD_TEST_BOOL(s == "def def def def dabc def");

    WD_TEST_BOOL(s.ReplaceWholeWord_NoCase("ABcd", "def", wdStringUtils::IsWordDelimiter_English) == nullptr);
    WD_TEST_BOOL(s == "def def def def dabc def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceWholeWordAll")
  {
    wdStringBuilder s = "abcd abc abcd abc dabc abc";

    WD_TEST_INT(s.ReplaceWholeWordAll("abc", "def", wdStringUtils::IsWordDelimiter_English), 3);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll("abc", "def", wdStringUtils::IsWordDelimiter_English), 0);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll("abcd", "def", wdStringUtils::IsWordDelimiter_English), 2);
    WD_TEST_BOOL(s == "def def def def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll("abcd", "def", wdStringUtils::IsWordDelimiter_English), 0);
    WD_TEST_BOOL(s == "def def def def dabc def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReplaceWholeWordAll_NoCase")
  {
    wdStringBuilder s = "abcd abc abcd abc dabc abc";

    WD_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English), 3);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABC", "def", wdStringUtils::IsWordDelimiter_English), 0);
    WD_TEST_BOOL(s == "abcd def abcd def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABCd", "def", wdStringUtils::IsWordDelimiter_English), 2);
    WD_TEST_BOOL(s == "def def def def dabc def");

    WD_TEST_INT(s.ReplaceWholeWordAll_NoCase("ABCd", "def", wdStringUtils::IsWordDelimiter_English), 0);
    WD_TEST_BOOL(s == "def def def def dabc def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "teset")
  {
    const char* sz = "abc def";
    wdStringView it(sz);

    wdStringBuilder s = it;
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Split")
  {
    wdStringBuilder s = "|abc,def<>ghi|,<>jkl|mno,pqr|stu";

    wdHybridArray<wdStringView, 32> SubStrings;

    s.Split(false, SubStrings, ",", "|", "<>");

    WD_TEST_INT(SubStrings.GetCount(), 7);
    WD_TEST_BOOL(SubStrings[0] == "abc");
    WD_TEST_BOOL(SubStrings[1] == "def");
    WD_TEST_BOOL(SubStrings[2] == "ghi");
    WD_TEST_BOOL(SubStrings[3] == "jkl");
    WD_TEST_BOOL(SubStrings[4] == "mno");
    WD_TEST_BOOL(SubStrings[5] == "pqr");
    WD_TEST_BOOL(SubStrings[6] == "stu");

    s.Split(true, SubStrings, ",", "|", "<>");

    WD_TEST_INT(SubStrings.GetCount(), 10);
    WD_TEST_BOOL(SubStrings[0] == "");
    WD_TEST_BOOL(SubStrings[1] == "abc");
    WD_TEST_BOOL(SubStrings[2] == "def");
    WD_TEST_BOOL(SubStrings[3] == "ghi");
    WD_TEST_BOOL(SubStrings[4] == "");
    WD_TEST_BOOL(SubStrings[5] == "");
    WD_TEST_BOOL(SubStrings[6] == "jkl");
    WD_TEST_BOOL(SubStrings[7] == "mno");
    WD_TEST_BOOL(SubStrings[8] == "pqr");
    WD_TEST_BOOL(SubStrings[9] == "stu");
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeCleanPath")
  {
    wdStringBuilder p;

    p = "C:\\temp/temp//tut";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "C:/temp/temp/tut");

    p = "\\temp/temp//tut\\\\";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "/temp/temp/tut/");

    p = "\\";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "/");

    p = "file";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "file");

    p = "C:\\temp/..//tut";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "C:/tut");

    p = "C:\\temp/..";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "C:/temp/..");

    p = "C:\\temp/..\\";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "C:/");

    p = "\\//temp/../bla\\\\blub///..\\temp//tut/tat/..\\\\..\\//ploep";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "//bla/temp/ploep");

    p = "a/b/c/../../../../e/f";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "../e/f");

    p = "/../../a/../../e/f";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "../../e/f");

    p = "/../../a/../../e/f/../";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "../../e/");

    p = "/../../a/../../e/f/..";
    p.MakeCleanPath();
    WD_TEST_BOOL(p == "../../e/f/..");

    p = "\\//temp/./bla\\\\blub///.\\temp//tut/tat/..\\.\\.\\//ploep";
    p.MakeCleanPath();
    WD_TEST_STRING(p.GetData(), "//temp/bla/blub/temp/tut/ploep");

    p = "./";
    p.MakeCleanPath();
    WD_TEST_STRING(p.GetData(), "");

    p = "/./././";
    p.MakeCleanPath();
    WD_TEST_STRING(p.GetData(), "/");

    p = "./.././";
    p.MakeCleanPath();
    WD_TEST_STRING(p.GetData(), "../");

    // more than two dots are invalid, so the should be kept as is
    p = "./..././abc/...\\def";
    p.MakeCleanPath();
    WD_TEST_STRING(p.GetData(), ".../abc/.../def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "PathParentDirectory")
  {
    wdStringBuilder p;

    p = "C:\\temp/temp//tut";
    p.PathParentDirectory();
    WD_TEST_BOOL(p == "C:/temp/temp/");

    p = "C:\\temp/temp//tut\\\\";
    p.PathParentDirectory();
    WD_TEST_BOOL(p == "C:/temp/temp/");

    p = "file";
    p.PathParentDirectory();
    WD_TEST_BOOL(p == "");

    p = "/file";
    p.PathParentDirectory();
    WD_TEST_BOOL(p == "/");

    p = "C:\\temp/..//tut";
    p.PathParentDirectory();
    WD_TEST_BOOL(p == "C:/");

    p = "file";
    p.PathParentDirectory(3);
    WD_TEST_BOOL(p == "../../");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "AppendPath")
  {
    wdStringBuilder p;

    p = "this/is\\my//path";
    p.AppendPath("orly/nowai");
    WD_TEST_BOOL(p == "this/is\\my//path/orly/nowai");

    p = "this/is\\my//path///";
    p.AppendPath("orly/nowai");
    WD_TEST_BOOL(p == "this/is\\my//path///orly/nowai");

    p = "";
    p.AppendPath("orly/nowai");
    WD_TEST_BOOL(p == "orly/nowai");

    // It should be valid to append an absolute path to an empty string.
    {
#if WD_ENABLED(WD_PLATFORM_WINDOWS)
      const char* szAbsPath = "C:\\folder";
      const char* szAbsPathAppendResult = "C:\\folder/File.ext";
#elif WD_ENABLED(WD_PLATFORM_OSX) || WD_ENABLED(WD_PLATFORM_LINUX) || WD_ENABLED(WD_PLATFORM_ANDROID)
      const char* szAbsPath = "/folder";
      const char* szAbsPathAppendResult = "/folder/File.ext";
#else
#  error "An absolute path example must be defined for the 'AppendPath' test for each platform!"
#endif

      p = "";
      p.AppendPath(szAbsPath, "File.ext");
      WD_TEST_BOOL(p == szAbsPathAppendResult);
    }

    p = "bla";
    p.AppendPath("");
    WD_TEST_BOOL(p == "bla");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeFileName")
  {
    wdStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileName("bla");
    WD_TEST_BOOL(p == "C:/test/test/bla.ext");

    p = "test/test/tut/troet.toeff";
    p.ChangeFileName("toeff");
    WD_TEST_BOOL(p == "test/test/tut/toeff.toeff");

    p = "test/test/tut/murpf";
    p.ChangeFileName("toeff");
    WD_TEST_BOOL(p == "test/test/tut/toeff");

    p = "test/test/tut/murpf/";
    p.ChangeFileName("toeff");
    WD_TEST_BOOL(p == "test/test/tut/murpf/toeff"); // filename is EMPTY -> thus ADDS it

    p = "test/test/tut/murpf/.extension"; // folders that start with a dot must be considered to be empty filenames with an extension
    p.ChangeFileName("toeff");
    WD_TEST_BOOL(p == "test/test/tut/murpf/toeff.extension");

    p = "test/test/tut/murpf/.extension/"; // folders that start with a dot ARE considered as folders, if the path ends with a slash
    p.ChangeFileName("toeff");
    WD_TEST_BOOL(p == "test/test/tut/murpf/.extension/toeff");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeFileNameAndExtension")
  {
    wdStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileNameAndExtension("bla.pups");
    WD_TEST_BOOL(p == "C:/test/test/bla.pups");

    p = "test/test/tut/troet.toeff";
    p.ChangeFileNameAndExtension("toeff");
    WD_TEST_BOOL(p == "test/test/tut/toeff");

    p = "test/test/tut/murpf";
    p.ChangeFileNameAndExtension("toeff.tut");
    WD_TEST_BOOL(p == "test/test/tut/toeff.tut");

    p = "test/test/tut/murpf/";
    p.ChangeFileNameAndExtension("toeff.blo");
    WD_TEST_BOOL(p == "test/test/tut/murpf/toeff.blo"); // filename is EMPTY -> thus ADDS it

    p = "test/test/tut/murpf/.extension"; // folders that start with a dot must be considered to be empty filenames with an extension
    p.ChangeFileNameAndExtension("toeff.ext");
    WD_TEST_BOOL(p == "test/test/tut/murpf/toeff.ext");

    p = "test/test/tut/murpf/.extension/"; // folders that start with a dot ARE considered as folders, if the path ends with a slash
    p.ChangeFileNameAndExtension("toeff");
    WD_TEST_BOOL(p == "test/test/tut/murpf/.extension/toeff");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChangeFileExtension")
  {
    wdStringBuilder p;

    p = "C:/test/test/tut.ext";
    p.ChangeFileExtension("pups");
    WD_TEST_BOOL(p == "C:/test/test/tut.pups");

    p = "C:/test/test/tut";
    p.ChangeFileExtension("pups");
    WD_TEST_BOOL(p == "C:/test/test/tut.pups");

    p = "C:/test/test/tut.ext";
    p.ChangeFileExtension("");
    WD_TEST_BOOL(p == "C:/test/test/tut.");

    p = "C:/test/test/tut";
    p.ChangeFileExtension("");
    WD_TEST_BOOL(p == "C:/test/test/tut.");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasAnyExtension")
  {
    wdStringBuilder p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.HasAnyExtension());

    p = "This/Is\\My//Path.dot\\file_no_extension";
    WD_TEST_BOOL(!p.HasAnyExtension());
    WD_TEST_BOOL(!p.HasAnyExtension());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasExtension")
  {
    wdStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.HasExtension(".Extension"));

    p = "This/Is\\My//Path.dot\\file.ext";
    WD_TEST_BOOL(p.HasExtension("EXT"));

    p = "This/Is\\My//Path.dot\\file.ext";
    WD_TEST_BOOL(!p.HasExtension("NEXT"));

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(!p.HasExtension(".Ext"));

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(!p.HasExtension("sion"));

    p = "";
    WD_TEST_BOOL(!p.HasExtension("ext"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileExtension")
  {
    wdStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.GetFileExtension() == "extension");

    p = "This/Is\\My//Path.dot\\file";
    WD_TEST_BOOL(p.GetFileExtension() == "");

    p = "";
    WD_TEST_BOOL(p.GetFileExtension() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileNameAndExtension")
  {
    wdStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "file.extension");

    p = "This/Is\\My//Path.dot\\.extension";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == ".extension");

    p = "This/Is\\My//Path.dot\\file";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "file");

    p = "\\file";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "file");

    p = "";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "");

    p = "/";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "");

    p = "This/Is\\My//Path.dot\\";
    WD_TEST_BOOL(p.GetFileNameAndExtension() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileName")
  {
    wdStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.GetFileName() == "file");

    p = "This/Is\\My//Path.dot\\file";
    WD_TEST_BOOL(p.GetFileName() == "file");

    p = "\\file";
    WD_TEST_BOOL(p.GetFileName() == "file");

    p = "";
    WD_TEST_BOOL(p.GetFileName() == "");

    p = "/";
    WD_TEST_BOOL(p.GetFileName() == "");

    p = "This/Is\\My//Path.dot\\";
    WD_TEST_BOOL(p.GetFileName() == "");

    // so far we treat file and folders whose names start with a '.' as extensions
    p = "This/Is\\My//Path.dot\\.stupidfile";
    WD_TEST_BOOL(p.GetFileName() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileDirectory")
  {
    wdStringBuilder p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This/Is\\My//Path.dot\\.extension";
    WD_TEST_BOOL(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This/Is\\My//Path.dot\\file";
    WD_TEST_BOOL(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "\\file";
    WD_TEST_BOOL(p.GetFileDirectory() == "\\");

    p = "";
    WD_TEST_BOOL(p.GetFileDirectory() == "");

    p = "/";
    WD_TEST_BOOL(p.GetFileDirectory() == "/");

    p = "This/Is\\My//Path.dot\\";
    WD_TEST_BOOL(p.GetFileDirectory() == "This/Is\\My//Path.dot\\");

    p = "This";
    WD_TEST_BOOL(p.GetFileDirectory() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsAbsolutePath / IsRelativePath / IsRootedPath")
  {
    wdStringBuilder p;

    p = "";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
    p = "C:\\temp.stuff";
    WD_TEST_BOOL(p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "C:/temp.stuff";
    WD_TEST_BOOL(p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "\\\\myserver\\temp.stuff";
    WD_TEST_BOOL(p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "\\myserver\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath()); // neither absolute nor relativ, just stupid
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "/temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath()); // bloed
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath()); // bloed
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "..\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = ".\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = ":MyDataDir\bla";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(p.IsRootedPath());

    p = ":\\MyDataDir\bla";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(p.IsRootedPath());

    p = ":/MyDataDir/bla";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(p.IsRootedPath());

#elif WD_ENABLED(WD_PLATFORM_OSX) || WD_ENABLED(WD_PLATFORM_LINUX) || WD_ENABLED(WD_PLATFORM_ANDROID)

    p = "C:\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "/temp.stuff";
    WD_TEST_BOOL(p.IsAbsolutePath());
    WD_TEST_BOOL(!p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = "..\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

    p = ".\\temp.stuff";
    WD_TEST_BOOL(!p.IsAbsolutePath());
    WD_TEST_BOOL(p.IsRelativePath());
    WD_TEST_BOOL(!p.IsRootedPath());

#else
#  error "Unknown platform."
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetRootedPathRootName")
  {
    wdStringBuilder p;

    p = ":root\\bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "root");

    p = ":root/bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "root");

    p = "://root/bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "root");

    p = ":/\\/root\\/bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "root");

    p = "://\\root";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "root");

    p = ":";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "");

    p = "";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "");

    p = "noroot\\bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "");

    p = "C:\\noroot/bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "");

    p = "/noroot/bla";
    WD_TEST_BOOL(p.GetRootedPathRootName() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsPathBelowFolder")
  {
    wdStringBuilder p;

    p = "a/b\\c//d\\\\e/f";
    WD_TEST_BOOL(!p.IsPathBelowFolder("/a/b\\c"));
    WD_TEST_BOOL(p.IsPathBelowFolder("a/b\\c"));
    WD_TEST_BOOL(p.IsPathBelowFolder("a/b\\c//"));
    WD_TEST_BOOL(p.IsPathBelowFolder("a/b\\c//d/\\e\\f")); // equal paths are considered 'below'
    WD_TEST_BOOL(!p.IsPathBelowFolder("a/b\\c//d/\\e\\f/g"));
    WD_TEST_BOOL(p.IsPathBelowFolder("a"));
    WD_TEST_BOOL(!p.IsPathBelowFolder("b"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakeRelativeTo")
  {
    wdStringBuilder p;

    p = u8"ä/b\\c/d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c").Succeeded());
    WD_TEST_BOOL(p == "d/e/f/g");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c").Failed());
    WD_TEST_BOOL(p == "d/e/f/g");

    p = u8"ä/b\\c//d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c").Succeeded());
    WD_TEST_BOOL(p == "d/e/f/g");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c").Failed());
    WD_TEST_BOOL(p == "d/e/f/g");

    p = u8"ä/b\\c/d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c/").Succeeded());
    WD_TEST_BOOL(p == "d/e/f/g");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c/").Failed());
    WD_TEST_BOOL(p == "d/e/f/g");

    p = u8"ä/b\\c//d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c/").Succeeded());
    WD_TEST_BOOL(p == "d/e/f/g");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c/").Failed());
    WD_TEST_BOOL(p == "d/e/f/g");

    p = u8"ä/b\\c//d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d/\\e\\f/g").Succeeded());
    WD_TEST_BOOL(p == "");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d/\\e\\f/g").Failed());
    WD_TEST_BOOL(p == "");

    p = u8"ä/b\\c//d\\\\e/f/g/";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d//e\\f/g\\h/i").Succeeded());
    WD_TEST_BOOL(p == "../../");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d//e\\f/g\\h/i").Failed());
    WD_TEST_BOOL(p == "../../");

    p = u8"ä/b\\c//d\\\\e/f/g/j/k";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d//e\\f/g\\h/i").Succeeded());
    WD_TEST_BOOL(p == "../../j/k");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c\\/d//e\\f/g\\h/i").Failed());
    WD_TEST_BOOL(p == "../../j/k");

    p = u8"ä/b\\c//d\\\\e/f/ge";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d/\\e\\f/g\\h/i").Succeeded());
    WD_TEST_BOOL(p == "../../../ge");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d/\\e\\f/g\\h/i").Failed());
    WD_TEST_BOOL(p == "../../../ge");

    p = u8"ä/b\\c//d\\\\e/f/g.txt";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d//e\\f/g\\h/i").Succeeded());
    WD_TEST_BOOL(p == "../../../g.txt");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d//e\\f/g\\h/i").Failed());
    WD_TEST_BOOL(p == "../../../g.txt");

    p = u8"ä/b\\c//d\\\\e/f/g";
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d//e\\f/g\\h/i").Succeeded());
    WD_TEST_BOOL(p == "../../");
    WD_TEST_BOOL(p.MakeRelativeTo(u8"ä\\b/c//d//e\\f/g\\h/i").Failed());
    WD_TEST_BOOL(p == "../../");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "MakePathSeparatorsNative")
  {
    wdStringBuilder p;
    p = "This/is\\a/temp\\\\path//to/my///file";

    p.MakePathSeparatorsNative();

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
    WD_TEST_STRING(p.GetData(), "This\\is\\a\\temp\\path\\to\\my\\file");
#else
    WD_TEST_STRING(p.GetData(), "This/is/a/temp/path/to/my/file");
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadAll")
  {
    wdDefaultMemoryStreamStorage StreamStorage;

    wdMemoryStreamWriter MemoryWriter(&StreamStorage);
    wdMemoryStreamReader MemoryReader(&StreamStorage);

    const char* szText =
      "l;kjasdflkjdfasjlk asflkj asfljwe oiweq2390432 4 @#$ otrjk3l;2rlkhitoqhrn324:R l324h32kjr hnasfhsakfh234fas1440687873242321245";

    MemoryWriter.WriteBytes(szText, wdStringUtils::GetStringElementCount(szText)).IgnoreResult();

    wdStringBuilder s;
    s.ReadAll(MemoryReader);

    WD_TEST_BOOL(s == szText);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSubString_FromTo")
  {
    wdStringBuilder sb = "basf";

    const char* sz = "abcdefghijklmnopqrstuvwxyz";

    sb.SetSubString_FromTo(sz + 5, sz + 13);
    WD_TEST_BOOL(sb == "fghijklm");

    sb.SetSubString_FromTo(sz + 17, sz + 30);
    WD_TEST_BOOL(sb == "rstuvwxyz");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSubString_ElementCount")
  {
    wdStringBuilder sb = "basf";

    wdStringUtf8 sz(L"aäbcödefügh");

    sb.SetSubString_ElementCount(sz.GetData() + 5, 5);
    WD_TEST_BOOL(sb == wdStringUtf8(L"ödef").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSubString_CharacterCount")
  {
    wdStringBuilder sb = "basf";

    wdStringUtf8 sz(L"aäbcödefgh");

    sb.SetSubString_CharacterCount(sz.GetData() + 5, 5);
    WD_TEST_BOOL(sb == wdStringUtf8(L"ödefg").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RemoveFileExtension")
  {
    wdStringBuilder sb = L"⺅⻩⽇⿕.〄㈷㑧䆴.ؼݻ༺.";

    sb.RemoveFileExtension();
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"⺅⻩⽇⿕.〄㈷㑧䆴.ؼݻ༺").GetData());

    sb.RemoveFileExtension();
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"⺅⻩⽇⿕.〄㈷㑧䆴").GetData());

    sb.RemoveFileExtension();
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"⺅⻩⽇⿕").GetData());

    sb.RemoveFileExtension();
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"⺅⻩⽇⿕").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Trim")
  {
    // Empty input
    wdStringBuilder sb = L"";
    sb.Trim(" \t");
    WD_TEST_STRING(sb.GetData(), wdStringUtf8(L"").GetData());
    sb.Trim(nullptr, " \t");
    WD_TEST_STRING(sb.GetData(), wdStringUtf8(L"").GetData());
    sb.Trim(" \t", nullptr);
    WD_TEST_STRING(sb.GetData(), wdStringUtf8(L"").GetData());

    // Clear all from one side
    auto sUnicode = L"私はクリストハさんです";
    sb = sUnicode;
    sb.Trim(nullptr, wdStringUtf8(sUnicode).GetData());
    WD_TEST_STRING(sb.GetData(), "");
    sb = sUnicode;
    sb.Trim(wdStringUtf8(sUnicode).GetData(), nullptr);
    WD_TEST_STRING(sb.GetData(), "");

    // Clear partial side
    sb = L"ですですですAにぱにぱにぱ";
    sb.Trim(nullptr, wdStringUtf8(L"にぱ").GetData());
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"ですですですA").GetData());
    sb.Trim(wdStringUtf8(L"です").GetData(), nullptr);
    WD_TEST_STRING_UNICODE(sb.GetData(), wdStringUtf8(L"A").GetData());

    sb = L"ですですですAにぱにぱにぱ";
    sb.Trim(wdStringUtf8(L"ですにぱ").GetData());
    WD_TEST_STRING(sb.GetData(), wdStringUtf8(L"A").GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TrimWordStart")
  {
    wdStringBuilder sb;

    {
      sb = "<test>abc<test>";
      WD_TEST_BOOL(sb.TrimWordStart("<test>"));
      WD_TEST_STRING(sb, "abc<test>");
      WD_TEST_BOOL(sb.TrimWordStart("<test>") == false);
      WD_TEST_STRING(sb, "abc<test>");
    }

    {
      sb = "<test><tut><test><test><tut>abc<tut><test>";
      WD_TEST_BOOL(sb.TrimWordStart("<tut>", "<test>"));
      WD_TEST_STRING(sb, "abc<tut><test>");
      WD_TEST_BOOL(sb.TrimWordStart("<tut>", "<test>") == false);
      WD_TEST_STRING(sb, "abc<tut><test>");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>abc";
      WD_TEST_BOOL(sb.TrimWordStart("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_STRING(sb, "abc");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordStart("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_STRING(sb, "");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TrimWordEnd")
  {
    wdStringBuilder sb;

    {
      sb = "<test>abc<test>";
      WD_TEST_BOOL(sb.TrimWordEnd("<test>"));
      WD_TEST_STRING(sb, "<test>abc");
      WD_TEST_BOOL(sb.TrimWordEnd("<test>") == false);
      WD_TEST_STRING(sb, "<test>abc");
    }

    {
      sb = "<tut><test>abc<test><tut><test><test><tut>";
      WD_TEST_BOOL(sb.TrimWordEnd("<tut>", "<test>"));
      WD_TEST_STRING(sb, "<tut><test>abc");
      WD_TEST_BOOL(sb.TrimWordEnd("<tut>", "<test>") == false);
      WD_TEST_STRING(sb, "<tut><test>abc");
    }

    {
      sb = "abc<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordEnd("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_STRING(sb, "abc");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordEnd("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_STRING(sb, "");
    }
  }
}

#pragma optimize("", on)
