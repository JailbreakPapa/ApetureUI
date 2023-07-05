#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature or compile with /utf-8 on windows.

#include <Foundation/Containers/Deque.h>
#include <Foundation/Strings/String.h>

WD_CREATE_SIMPLE_TEST(Strings, StringView)
{
  wdStringBuilder tmp;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (simple)")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";

    wdStringView it(sz);

    WD_TEST_BOOL(it.GetStartPointer() == sz);
    WD_TEST_STRING(it.GetData(tmp), sz);
    WD_TEST_BOOL(it.GetEndPointer() == sz + 26);
    WD_TEST_INT(it.GetElementCount(), 26);

    wdStringView it2(sz + 15);

    WD_TEST_BOOL(it2.GetStartPointer() == &sz[15]);
    WD_TEST_STRING(it2.GetData(tmp), &sz[15]);
    WD_TEST_BOOL(it2.GetEndPointer() == sz + 26);
    WD_TEST_INT(it2.GetElementCount(), 11);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor (complex, YARLY!)")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";

    wdStringView it(sz + 3, sz + 17);
    it.SetStartPosition(sz + 5);

    WD_TEST_BOOL(it.GetStartPointer() == sz + 5);
    WD_TEST_STRING(it.GetData(tmp), "fghijklmnopq");
    WD_TEST_BOOL(it.GetEndPointer() == sz + 17);
    WD_TEST_INT(it.GetElementCount(), 12);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor constexpr")
  {
    constexpr wdStringView b = wdStringView("Hello World", 10);
    WD_TEST_INT(b.GetElementCount(), 10);
    WD_TEST_STRING(b.GetData(tmp), "Hello Worl");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "String literal")
  {
    constexpr wdStringView a = "Hello World"_wdsv;
    WD_TEST_INT(a.GetElementCount(), 11);
    WD_TEST_STRING(a.GetData(tmp), "Hello World");

    wdStringView b = "Hello Worl"_wdsv;
    WD_TEST_INT(b.GetElementCount(), 10);
    WD_TEST_STRING(b.GetData(tmp), "Hello Worl");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator++")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";
    wdStringView it(sz);

    for (wdInt32 i = 0; i < 26; ++i)
    {
      WD_TEST_INT(it.GetCharacter(), sz[i]);
      WD_TEST_BOOL(it.IsValid());
      it.Shrink(1, 0);
    }

    WD_TEST_BOOL(!it.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator== / operator!=")
  {
    wdString s1(L"abcdefghiäöüß€");
    wdString s2(L"ghiäöüß€abdef");

    wdStringView it1 = s1.GetSubString(8, 4);
    wdStringView it2 = s2.GetSubString(2, 4);
    wdStringView it3 = s2.GetSubString(2, 5);

    WD_TEST_BOOL(it1 == it2);
    WD_TEST_BOOL(it1 != it3);

    WD_TEST_BOOL(it1 == wdString(L"iäöü").GetData());
    WD_TEST_BOOL(it2 == wdString(L"iäöü").GetData());
    WD_TEST_BOOL(it3 == wdString(L"iäöüß").GetData());

    s1 = "abcdefghijkl";
    s2 = "oghijklm";

    it1 = s1.GetSubString(6, 4);
    it2 = s2.GetSubString(1, 4);
    it3 = s2.GetSubString(1, 5);

    WD_TEST_BOOL(it1 == it2);
    WD_TEST_BOOL(it1 != it3);

    WD_TEST_BOOL(it1 == "ghij");
    WD_TEST_BOOL(it1 != "ghijk");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsEqual")
  {
    const char* sz = "abcdef";
    wdStringView it(sz);

    WD_TEST_BOOL(it.IsEqual(wdStringView("abcdef")));
    WD_TEST_BOOL(!it.IsEqual(wdStringView("abcde")));
    WD_TEST_BOOL(!it.IsEqual(wdStringView("abcdefg")));

    wdStringView it2(sz + 2, sz + 5);

    const char* szRhs = "Abcdef";
    wdStringView it3(szRhs + 2, szRhs + 5);
    WD_TEST_BOOL(it2.IsEqual(it3));
    it3 = wdStringView(szRhs + 1, szRhs + 5);
    WD_TEST_BOOL(!it2.IsEqual(it3));
    it3 = wdStringView(szRhs + 2, szRhs + 6);
    WD_TEST_BOOL(!it2.IsEqual(it3));
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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator+=")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";
    wdStringView it(sz);

    for (wdInt32 i = 0; i < 26; i += 2)
    {
      WD_TEST_INT(it.GetCharacter(), sz[i]);
      WD_TEST_BOOL(it.IsValid());
      it.Shrink(2, 0);
    }

    WD_TEST_BOOL(!it.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetCharacter")
  {
    wdStringUtf8 s(L"abcäöü€");
    wdStringView it = wdStringView(s.GetData());

    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[0]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[1]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[2]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[3]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[5]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[7]));
    it.Shrink(1, 0);
    WD_TEST_INT(it.GetCharacter(), wdUnicodeUtils::ConvertUtf8ToUtf32(&s.GetData()[9]));
    it.Shrink(1, 0);
    WD_TEST_BOOL(!it.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetElementCount")
  {
    wdStringUtf8 s(L"abcäöü€");
    wdStringView it = wdStringView(s.GetData());

    WD_TEST_INT(it.GetElementCount(), 12);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 11);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 10);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 9);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 7);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 5);
    it.Shrink(1, 0);
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 3);
    it.Shrink(1, 0);
    WD_TEST_BOOL(!it.IsValid());
    WD_TEST_INT(it.GetElementCount(), 0);
    it.Shrink(1, 0);
    WD_TEST_BOOL(!it.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetStartPosition")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";
    wdStringView it(sz);

    for (wdInt32 i = 0; i < 26; ++i)
    {
      it.SetStartPosition(sz + i);
      WD_TEST_BOOL(it.IsValid());
      WD_TEST_BOOL(it.StartsWith(&sz[i]));
    }

    WD_TEST_BOOL(it.IsValid());
    it.Shrink(1, 0);
    WD_TEST_BOOL(!it.IsValid());

    it = wdStringView(sz);
    for (wdInt32 i = 0; i < 26; ++i)
    {
      it.SetStartPosition(sz + i);
      WD_TEST_BOOL(it.IsValid());
      WD_TEST_BOOL(it.StartsWith(&sz[i]));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetStartPosition / GetEndPosition / GetData")
  {
    const char* sz = "abcdefghijklmnopqrstuvwxyz";
    wdStringView it(sz + 7, sz + 19);

    WD_TEST_BOOL(it.GetStartPointer() == sz + 7);
    WD_TEST_BOOL(it.GetEndPointer() == sz + 19);
    WD_TEST_STRING(it.GetData(tmp), "hijklmnopqrs");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Shrink")
  {
    wdStringUtf8 s(L"abcäöü€def");
    wdStringView it(s.GetData());

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[0]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[15]);
    WD_TEST_STRING(it.GetData(tmp), &s.GetData()[0]);
    WD_TEST_BOOL(it.IsValid());

    it.Shrink(1, 0);

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[1]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[15]);
    WD_TEST_STRING(it.GetData(tmp), &s.GetData()[1]);
    WD_TEST_BOOL(it.IsValid());

    it.Shrink(3, 0);

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[5]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[15]);
    WD_TEST_STRING(it.GetData(tmp), &s.GetData()[5]);
    WD_TEST_BOOL(it.IsValid());

    it.Shrink(0, 4);

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[5]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[9]);
    WD_TEST_STRING(it.GetData(tmp), u8"öü");
    WD_TEST_BOOL(it.IsValid());

    it.Shrink(1, 1);

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[7]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[7]);
    WD_TEST_STRING(it.GetData(tmp), "");
    WD_TEST_BOOL(!it.IsValid());

    it.Shrink(10, 10);

    WD_TEST_BOOL(it.GetStartPointer() == &s.GetData()[7]);
    WD_TEST_BOOL(it.GetEndPointer() == &s.GetData()[7]);
    WD_TEST_STRING(it.GetData(tmp), "");
    WD_TEST_BOOL(!it.IsValid());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChopAwayFirstCharacterUtf8")
  {
    wdStringUtf8 utf8(L"О, Господи!");
    wdStringView s(utf8.GetData());

    const char* szOrgStart = s.GetStartPointer();
    const char* szOrgEnd = s.GetEndPointer();

    while (!s.IsEmpty())
    {
      const wdUInt32 uiNumCharsBefore = wdStringUtils::GetCharacterCount(s.GetStartPointer(), s.GetEndPointer());
      s.ChopAwayFirstCharacterUtf8();
      const wdUInt32 uiNumCharsAfter = wdStringUtils::GetCharacterCount(s.GetStartPointer(), s.GetEndPointer());

      WD_TEST_INT(uiNumCharsBefore, uiNumCharsAfter + 1);
    }

    // this needs to be true, some code relies on the fact that the start pointer always moves forwards
    WD_TEST_BOOL(s.GetStartPointer() == szOrgEnd);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ChopAwayFirstCharacterAscii")
  {
    wdStringUtf8 utf8(L"Wosn Schmarrn");
    wdStringView s("");

    const char* szOrgStart = s.GetStartPointer();
    const char* szOrgEnd = s.GetEndPointer();

    while (!s.IsEmpty())
    {
      const wdUInt32 uiNumCharsBefore = s.GetElementCount();
      s.ChopAwayFirstCharacterAscii();
      const wdUInt32 uiNumCharsAfter = s.GetElementCount();

      WD_TEST_INT(uiNumCharsBefore, uiNumCharsAfter + 1);
    }

    // this needs to be true, some code relies on the fact that the start pointer always moves forwards
    WD_TEST_BOOL(s.GetStartPointer() == szOrgEnd);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Trim")
  {
    // Empty input
    wdStringUtf8 utf8(L"");
    wdStringView view(utf8.GetData());
    view.Trim(" \t");
    WD_TEST_BOOL(view.IsEqual(wdStringUtf8(L"").GetData()));
    view.Trim(nullptr, " \t");
    WD_TEST_BOOL(view.IsEqual(wdStringUtf8(L"").GetData()));
    view.Trim(" \t", nullptr);
    WD_TEST_BOOL(view.IsEqual(wdStringUtf8(L"").GetData()));

    // Clear all from one side
    wdStringUtf8 sUnicode(L"私はクリストハさんです");
    view = sUnicode.GetData();
    view.Trim(nullptr, sUnicode.GetData());
    WD_TEST_BOOL(view.IsEqual(""));
    view = sUnicode.GetData();
    view.Trim(sUnicode.GetData(), nullptr);
    WD_TEST_BOOL(view.IsEqual(""));

    // Clear partial side
    sUnicode = L"ですですですAにぱにぱにぱ";
    view = sUnicode.GetData();
    view.Trim(nullptr, wdStringUtf8(L"にぱ").GetData());
    sUnicode = L"ですですですA";
    WD_TEST_BOOL(view.IsEqual(sUnicode.GetData()));
    view.Trim(wdStringUtf8(L"です").GetData(), nullptr);
    WD_TEST_BOOL(view.IsEqual(wdStringUtf8(L"A").GetData()));

    sUnicode = L"ですですですAにぱにぱにぱ";
    view = sUnicode.GetData();
    view.Trim(wdStringUtf8(L"ですにぱ").GetData());
    WD_TEST_BOOL(view.IsEqual(wdStringUtf8(L"A").GetData()));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TrimWordStart")
  {
    wdStringView sb;

    {
      sb = "<test>abc<test>";
      WD_TEST_BOOL(sb.TrimWordStart("<test>"));
      WD_TEST_BOOL(sb == "abc<test>");
      WD_TEST_BOOL(sb.TrimWordStart("<test>") == false);
      WD_TEST_BOOL(sb == "abc<test>");
    }

    {
      sb = "<test><tut><test><test><tut>abc<tut><test>";
      WD_TEST_BOOL(sb.TrimWordStart("<tut>", "<test>"));
      WD_TEST_BOOL(sb == "abc<tut><test>");
      WD_TEST_BOOL(sb.TrimWordStart("<tut>", "<test>") == false);
      WD_TEST_BOOL(sb == "abc<tut><test>");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>abc";
      WD_TEST_BOOL(sb.TrimWordStart("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_BOOL(sb == "abc");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordStart("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_BOOL(sb == "");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TrimWordEnd")
  {
    wdStringView sb;

    {
      sb = "<test>abc<test>";
      WD_TEST_BOOL(sb.TrimWordEnd("<test>"));
      WD_TEST_BOOL(sb == "<test>abc");
      WD_TEST_BOOL(sb.TrimWordEnd("<test>") == false);
      WD_TEST_BOOL(sb == "<test>abc");
    }

    {
      sb = "<tut><test>abc<test><tut><test><test><tut>";
      WD_TEST_BOOL(sb.TrimWordEnd("<tut>", "<test>"));
      WD_TEST_BOOL(sb == "<tut><test>abc");
      WD_TEST_BOOL(sb.TrimWordEnd("<tut>", "<test>") == false);
      WD_TEST_BOOL(sb == "<tut><test>abc");
    }

    {
      sb = "abc<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordEnd("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_BOOL(sb == "abc");
    }

    {
      sb = "<a><b><c><d><e><a><b><c><d><e>";
      WD_TEST_BOOL(sb.TrimWordEnd("<a>", "<b>", "<c>", "<d>", "<e>"));
      WD_TEST_BOOL(sb == "");
    }
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Split")
  {
    wdStringView s = "|abc,def<>ghi|,<>jkl|mno,pqr|stu";

    wdDeque<wdStringView> SubStrings;

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

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasAnyExtension")
  {
    wdStringView p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.HasAnyExtension());

    p = "This/Is\\My//Path.dot\\file_no_extension";
    WD_TEST_BOOL(!p.HasAnyExtension());
    WD_TEST_BOOL(!p.HasAnyExtension());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "HasExtension")
  {
    wdStringView p;

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
    wdStringView p;

    p = "This/Is\\My//Path.dot\\file.extension";
    WD_TEST_BOOL(p.GetFileExtension() == "extension");

    p = "This/Is\\My//Path.dot\\file";
    WD_TEST_BOOL(p.GetFileExtension() == "");

    p = "";
    WD_TEST_BOOL(p.GetFileExtension() == "");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFileNameAndExtension")
  {
    wdStringView p;

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
    wdStringView p;

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
    wdStringView p;

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
    wdStringView p;

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
    wdStringView p;

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
}
