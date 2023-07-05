#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Strings/String.h>

static wdString GetString(const char* szSz)
{
  wdString s;
  s = szSz;
  return s;
}

static wdStringBuilder GetStringBuilder(const char* szSz)
{
  wdStringBuilder s;

  for (wdUInt32 i = 0; i < 10; ++i)
    s.Append(szSz);

  return s;
}

WD_CREATE_SIMPLE_TEST(Strings, String)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor")
  {
    wdString s1;
    WD_TEST_BOOL(s1 == "");

    wdString s2("abc");
    WD_TEST_BOOL(s2 == "abc");

    wdString s3(s2);
    WD_TEST_BOOL(s2 == s3);
    WD_TEST_BOOL(s3 == "abc");

    wdString s4(L"abc");
    WD_TEST_BOOL(s4 == "abc");

    wdStringView it = s4.GetFirst(2);
    wdString s5(it);
    WD_TEST_BOOL(s5 == "ab");

    wdStringBuilder strB("wobwob");
    wdString s6(strB);
    WD_TEST_BOOL(s6 == "wobwob");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator=")
  {
    wdString s2;
    s2 = "abc";
    WD_TEST_BOOL(s2 == "abc");

    wdString s3;
    s3 = s2;
    WD_TEST_BOOL(s2 == s3);
    WD_TEST_BOOL(s3 == "abc");

    wdString s4;
    s4 = L"abc";
    WD_TEST_BOOL(s4 == "abc");

    wdString s5(L"abcdefghijklm");
    wdStringView it(s5.GetData() + 2, s5.GetData() + 10);
    wdString s5b = it;
    WD_TEST_STRING(s5b, "cdefghij");

    wdString s6(L"aölsdföasld");
    wdStringBuilder strB("wobwob");
    s6 = strB;
    WD_TEST_BOOL(s6 == "wobwob");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "convert to wdStringView")
  {
    wdString s(L"aölsdföasld");
    wdStringBuilder tmp;

    wdStringView sv = s;

    WD_TEST_STRING(sv.GetData(tmp), wdStringUtf8(L"aölsdföasld").GetData());
    WD_TEST_BOOL(sv == wdStringUtf8(L"aölsdföasld").GetData());

    s = "abcdef";

    WD_TEST_STRING(sv.GetStartPointer(), "abcdef");
    WD_TEST_BOOL(sv == "abcdef");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move constructor / operator")
  {
    wdString s1(GetString("move me"));
    WD_TEST_STRING(s1.GetData(), "move me");

    s1 = GetString("move move move move move move move move ");
    WD_TEST_STRING(s1.GetData(), "move move move move move move move move ");

    wdString s2(GetString("move move move move move move move move "));
    WD_TEST_STRING(s2.GetData(), "move move move move move move move move ");

    s2 = GetString("move me");
    WD_TEST_STRING(s2.GetData(), "move me");

    s1 = s2;
    WD_TEST_STRING(s1.GetData(), "move me");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Move constructor / operator (StringBuilder)")
  {
    const wdString s1(GetStringBuilder("move me"));
    const wdString s2(GetStringBuilder("move move move move move move move move "));

    wdString s3(GetStringBuilder("move me"));
    WD_TEST_BOOL(s3 == s1);

    s3 = GetStringBuilder("move move move move move move move move ");
    WD_TEST_BOOL(s3 == s2);

    wdString s4(GetStringBuilder("move move move move move move move move "));
    WD_TEST_BOOL(s4 == s2);

    s4 = GetStringBuilder("move me");
    WD_TEST_BOOL(s4 == s1);

    s3 = s4;
    WD_TEST_BOOL(s3 == s1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Clear")
  {
    wdString s("abcdef");
    WD_TEST_BOOL(s == "abcdef");

    s.Clear();
    WD_TEST_BOOL(s.IsEmpty());
    WD_TEST_BOOL(s == "");
    WD_TEST_BOOL(s == nullptr);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetData")
  {
    const char* sz = "abcdef";

    wdString s(sz);
    WD_TEST_BOOL(s.GetData() != sz); // it should NOT be the exact same string
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetElementCount / GetCharacterCount")
  {
    wdString s(L"abcäöü€");

    WD_TEST_INT(s.GetElementCount(), 12);
    WD_TEST_INT(s.GetCharacterCount(), 7);

    s = "testtest";
    WD_TEST_INT(s.GetElementCount(), 8);
    WD_TEST_INT(s.GetCharacterCount(), 8);

    s.Clear();

    WD_TEST_INT(s.GetElementCount(), 0);
    WD_TEST_INT(s.GetCharacterCount(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Convert to wdStringView")
  {
    wdString s(L"abcäöü€def");

    wdStringView view = s;
    WD_TEST_BOOL(view.StartsWith("abc"));
    WD_TEST_BOOL(view.EndsWith("def"));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetSubString")
  {
    wdString s(L"abcäöü€def");
    wdStringUtf8 s8(L"äöü€");

    wdStringView it = s.GetSubString(3, 4);
    WD_TEST_BOOL(it == s8.GetData());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetFirst")
  {
    wdString s(L"abcäöü€def");

    WD_TEST_BOOL(s.GetFirst(3) == "abc");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetLast")
  {
    wdString s(L"abcäöü€def");

    WD_TEST_BOOL(s.GetLast(3) == "def");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "ReadAll")
  {
    wdDefaultMemoryStreamStorage StreamStorage;

    wdMemoryStreamWriter MemoryWriter(&StreamStorage);
    wdMemoryStreamReader MemoryReader(&StreamStorage);

    const char* szText =
      "l;kjasdflkjdfasjlk asflkj asfljwe oiweq2390432 4 @#$ otrjk3l;2rlkhitoqhrn324:R l324h32kjr hnasfhsakfh234fas1440687873242321245";

    MemoryWriter.WriteBytes(szText, wdStringUtils::GetStringElementCount(szText)).IgnoreResult();

    wdString s;
    s.ReadAll(MemoryReader);

    WD_TEST_BOOL(s == szText);
  }
}
