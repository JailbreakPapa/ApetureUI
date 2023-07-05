#include <FoundationTest/FoundationTestPCH.h>

// NOTE: always save as Unicode UTF-8 with signature

#include <Foundation/Strings/String.h>

template <typename STRING>
void TestConstruction(const STRING& value, const char* szStart, const char* szEnd)
{
  wdStringUtf8 sUtf8(L"A単語F");
  WD_TEST_BOOL(value.IsEqual(sUtf8.GetData()));
  const bool bEqualForwardItTypes = wdConversionTest<typename STRING::iterator, typename STRING::const_iterator>::sameType == 1;
  WD_CHECK_AT_COMPILETIME_MSG(
    bEqualForwardItTypes, "As the string iterator is read-only, both const and non-const versions should be the same type.");
  const bool bEqualReverseItTypes = wdConversionTest<typename STRING::reverse_iterator, typename STRING::const_reverse_iterator>::sameType == 1;
  WD_CHECK_AT_COMPILETIME_MSG(
    bEqualReverseItTypes, "As the reverse string iterator is read-only, both const and non-const versions should be the same type.");

  typename STRING::iterator itInvalid;
  WD_TEST_BOOL(!itInvalid.IsValid());
  typename STRING::reverse_iterator itInvalidR;
  WD_TEST_BOOL(!itInvalidR.IsValid());

  // Begin
  const typename STRING::iterator itBegin = begin(value);
  WD_TEST_BOOL(itBegin == value.GetIteratorFront());
  WD_TEST_BOOL(itBegin.IsValid());
  WD_TEST_BOOL(itBegin == itBegin);
  WD_TEST_BOOL(itBegin.GetData() == szStart);
  WD_TEST_BOOL(itBegin.GetCharacter() == wdUnicodeUtils::ConvertUtf8ToUtf32("A"));
  WD_TEST_BOOL(*itBegin == wdUnicodeUtils::ConvertUtf8ToUtf32("A"));

  // End
  const typename STRING::iterator itEnd = end(value);
  WD_TEST_BOOL(!itEnd.IsValid());
  WD_TEST_BOOL(itEnd == itEnd);
  WD_TEST_BOOL(itBegin != itEnd);
  WD_TEST_BOOL(itEnd.GetData() == szEnd);
  WD_TEST_BOOL(itEnd.GetCharacter() == 0);
  WD_TEST_BOOL(*itEnd == 0);

  // RBegin
  const typename STRING::reverse_iterator itBeginR = rbegin(value);
  WD_TEST_BOOL(itBeginR == value.GetIteratorBack());
  WD_TEST_BOOL(itBeginR.IsValid());
  WD_TEST_BOOL(itBeginR == itBeginR);
  const char* szEndPrior = szEnd;
  wdUnicodeUtils::MoveToPriorUtf8(szEndPrior);
  WD_TEST_BOOL(itBeginR.GetData() == szEndPrior);
  WD_TEST_BOOL(itBeginR.GetCharacter() == wdUnicodeUtils::ConvertUtf8ToUtf32("F"));
  WD_TEST_BOOL(*itBeginR == wdUnicodeUtils::ConvertUtf8ToUtf32("F"));

  // REnd
  const typename STRING::reverse_iterator itEndR = rend(value);
  WD_TEST_BOOL(!itEndR.IsValid());
  WD_TEST_BOOL(itEndR == itEndR);
  WD_TEST_BOOL(itBeginR != itEndR);
  WD_TEST_BOOL(itEndR.GetData() == nullptr); // Position before first character is not a valid ptr, so it is set to nullptr.
  WD_TEST_BOOL(itEndR.GetCharacter() == 0);
  WD_TEST_BOOL(*itEndR == 0);
}

template <typename STRING, typename IT>
void TestIteratorBegin(const STRING& value, const IT& it)
{
  // It is safe to try to move beyond the iterator's range.
  IT itBegin = it;
  --itBegin;
  itBegin -= 4;
  WD_TEST_BOOL(itBegin == it);
  WD_TEST_BOOL(itBegin - 2 == it);

  // Prefix / Postfix
  WD_TEST_BOOL(itBegin + 2 != it);
  WD_TEST_BOOL(itBegin++ == it);
  WD_TEST_BOOL(itBegin-- != it);
  itBegin = it;
  WD_TEST_BOOL(++itBegin != it);
  WD_TEST_BOOL(--itBegin == it);

  // Misc
  itBegin = it;
  WD_TEST_BOOL(it + 2 == ++(++itBegin));
  itBegin -= 1;
  WD_TEST_BOOL(itBegin == it + 1);
  itBegin -= 0;
  WD_TEST_BOOL(itBegin == it + 1);
  itBegin += 0;
  WD_TEST_BOOL(itBegin == it + 1);
  itBegin += -1;
  WD_TEST_BOOL(itBegin == it);
}

template <typename STRING, typename IT>
void TestIteratorEnd(const STRING& value, const IT& it)
{
  // It is safe to try to move beyond the iterator's range.
  IT itEnd = it;
  ++itEnd;
  itEnd += 4;
  WD_TEST_BOOL(itEnd == it);
  WD_TEST_BOOL(itEnd + 2 == it);

  // Prefix / Postfix
  WD_TEST_BOOL(itEnd - 2 != it);
  WD_TEST_BOOL(itEnd-- == it);
  WD_TEST_BOOL(itEnd++ != it);
  itEnd = it;
  WD_TEST_BOOL(--itEnd != it);
  WD_TEST_BOOL(++itEnd == it);

  // Misc
  itEnd = it;
  WD_TEST_BOOL(it - 2 == --(--itEnd));
  itEnd += 1;
  WD_TEST_BOOL(itEnd == it - 1);
  itEnd += 0;
  WD_TEST_BOOL(itEnd == it - 1);
  itEnd -= 0;
  WD_TEST_BOOL(itEnd == it - 1);
  itEnd -= -1;
  WD_TEST_BOOL(itEnd == it);
}

template <typename STRING>
void TestOperators(const STRING& value, const char* szStart, const char* szEnd)
{
  wdStringUtf8 sUtf8(L"A単語F");
  WD_TEST_BOOL(value.IsEqual(sUtf8.GetData()));

  // Begin
  typename STRING::iterator itBegin = begin(value);
  TestIteratorBegin(value, itBegin);

  // End
  typename STRING::iterator itEnd = end(value);
  TestIteratorEnd(value, itEnd);

  // RBegin
  typename STRING::reverse_iterator itBeginR = rbegin(value);
  TestIteratorBegin(value, itBeginR);

  // REnd
  typename STRING::reverse_iterator itEndR = rend(value);
  TestIteratorEnd(value, itEndR);
}

template <typename STRING>
void TestLoops(const STRING& value, const char* szStart, const char* szEnd)
{
  wdStringUtf8 sUtf8(L"A単語F");
  wdUInt32 characters[] = {wdUnicodeUtils::ConvertUtf8ToUtf32(wdStringUtf8(L"A").GetData()),
    wdUnicodeUtils::ConvertUtf8ToUtf32(wdStringUtf8(L"単").GetData()), wdUnicodeUtils::ConvertUtf8ToUtf32(wdStringUtf8(L"語").GetData()),
    wdUnicodeUtils::ConvertUtf8ToUtf32(wdStringUtf8(L"F").GetData())};

  // Forward
  wdInt32 iIndex = 0;
  for (wdUInt32 character : value)
  {
    WD_TEST_INT(characters[iIndex], character);
    ++iIndex;
  }
  WD_TEST_INT(iIndex, 4);

  typename STRING::iterator itBegin = begin(value);
  typename STRING::iterator itEnd = end(value);
  iIndex = 0;
  for (auto it = itBegin; it != itEnd; ++it)
  {
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(characters[iIndex], it.GetCharacter());
    WD_TEST_INT(characters[iIndex], *it);
    WD_TEST_BOOL(it.GetData() >= szStart);
    WD_TEST_BOOL(it.GetData() < szEnd);
    ++iIndex;
  }
  WD_TEST_INT(iIndex, 4);

  // Reverse
  typename STRING::reverse_iterator itBeginR = rbegin(value);
  typename STRING::reverse_iterator itEndR = rend(value);
  iIndex = 3;
  for (auto it = itBeginR; it != itEndR; ++it)
  {
    WD_TEST_BOOL(it.IsValid());
    WD_TEST_INT(characters[iIndex], it.GetCharacter());
    WD_TEST_INT(characters[iIndex], *it);
    WD_TEST_BOOL(it.GetData() >= szStart);
    WD_TEST_BOOL(it.GetData() < szEnd);
    --iIndex;
  }
  WD_TEST_INT(iIndex, -1);
}

WD_CREATE_SIMPLE_TEST(Strings, StringIterator)
{
  wdStringUtf8 sUtf8(L"_A単語F_");
  wdStringBuilder sTestStringBuilder = sUtf8.GetData();
  sTestStringBuilder.Shrink(1, 1);
  wdString sTextString = sTestStringBuilder.GetData();

  wdStringView view(sUtf8.GetData());
  view.Shrink(1, 1);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Construction")
  {
    TestConstruction<wdString>(sTextString, sTextString.GetData(), sTextString.GetData() + sTextString.GetElementCount());
    TestConstruction<wdStringBuilder>(
      sTestStringBuilder, sTestStringBuilder.GetData(), sTestStringBuilder.GetData() + sTestStringBuilder.GetElementCount());
    TestConstruction<wdStringView>(view, view.GetStartPointer(), view.GetEndPointer());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    TestOperators<wdString>(sTextString, sTextString.GetData(), sTextString.GetData() + sTextString.GetElementCount());
    TestOperators<wdStringBuilder>(
      sTestStringBuilder, sTestStringBuilder.GetData(), sTestStringBuilder.GetData() + sTestStringBuilder.GetElementCount());
    TestOperators<wdStringView>(view, view.GetStartPointer(), view.GetEndPointer());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Loops")
  {
    TestLoops<wdString>(sTextString, sTextString.GetData(), sTextString.GetData() + sTextString.GetElementCount());
    TestLoops<wdStringBuilder>(sTestStringBuilder, sTestStringBuilder.GetData(), sTestStringBuilder.GetData() + sTestStringBuilder.GetElementCount());
    TestLoops<wdStringView>(view, view.GetStartPointer(), view.GetEndPointer());
  }
}
