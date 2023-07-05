#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/Strings/FormatString.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Time/Stopwatch.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Time/Timestamp.h>

#include <Foundation/Types/ScopeExit.h>
#include <stdarg.h>

void TestFormat(const wdFormatString& str, const char* szExpected)
{
  wdStringBuilder sb;
  const char* szText = str.GetText(sb);

  WD_TEST_STRING(szText, szExpected);
}

void TestFormatWChar(const wdFormatString& str, const wchar_t* pExpected)
{
  wdStringBuilder sb;
  const char* szText = str.GetText(sb);

  WD_TEST_WSTRING(wdStringWChar(szText), pExpected);
}

void CompareSnprintf(wdStringBuilder& ref_sLog, const wdFormatString& str, const char* szFormat, ...)
{
  va_list args;
  va_start(args, szFormat);

  char Temp1[256];
  char Temp2[256];

  // reusing args list crashes on GCC / Clang
  wdStringUtils::vsnprintf(Temp1, 256, szFormat, args);
  vsnprintf(Temp2, 256, szFormat, args);
  WD_TEST_STRING(Temp1, Temp2);

  wdTime t1, t2, t3;
  wdStopwatch sw;
  {
    sw.StopAndReset();

    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      wdStringUtils::vsnprintf(Temp1, 256, szFormat, args);
    }

    t1 = sw.Checkpoint();
  }

  {
    sw.StopAndReset();

    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      vsnprintf(Temp2, 256, szFormat, args);
    }

    t2 = sw.Checkpoint();
  }

  {
    wdStringBuilder sb;

    sw.StopAndReset();
    for (wdUInt32 i = 0; i < 10000; ++i)
    {
      const char* szText = str.GetText(sb);
    }

    t3 = sw.Checkpoint();
  }

  ref_sLog.AppendFormat("wd: {0} msec, std: {1} msec, wdFmt: {2} msec : {3} -> {4}\n", wdArgF(t1.GetMilliseconds(), 2), wdArgF(t2.GetMilliseconds(), 2),
    wdArgF(t3.GetMilliseconds(), 2), szFormat, Temp1);

  va_end(args);
}

WD_CREATE_SIMPLE_TEST(Strings, FormatString)
{
  wdStringBuilder perfLog;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basics")
  {
    const char* tmp = "stringviewstuff";

    const char* sz = "sz";
    wdString string = "string";
    wdStringBuilder sb = "builder";
    wdStringView sv(tmp + 6, tmp + 10);

    TestFormat(wdFmt("{0}, {1}, {2}, {3}", wdInt8(-1), wdInt16(-2), wdInt32(-3), wdInt64(-4)), "-1, -2, -3, -4");
    TestFormat(wdFmt("{0}, {1}, {2}, {3}", wdUInt8(1), wdUInt16(2), wdUInt32(3), wdUInt64(4)), "1, 2, 3, 4");

    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(0ll), wdArgHumanReadable(1ll)), "0, 1");
    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(-0ll), wdArgHumanReadable(-1ll)), "0, -1");
    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(999ll), wdArgHumanReadable(1000ll)), "999, 1.00K");
    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(-999ll), wdArgHumanReadable(-1000ll)), "-999, -1.00K");
    // 999.999 gets rounded up for precision 2, so result is 1000.00K not 999.99K
    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(999'999ll), wdArgHumanReadable(1'000'000ll)), "1000.00K, 1.00M");
    TestFormat(wdFmt("{0}, {1}", wdArgHumanReadable(-999'999ll), wdArgHumanReadable(-1'000'000ll)), "-1000.00K, -1.00M");

    TestFormat(wdFmt("{0}, {1}", wdArgFileSize(0u), wdArgFileSize(1u)), "0B, 1B");
    TestFormat(wdFmt("{0}, {1}", wdArgFileSize(1023u), wdArgFileSize(1024u)), "1023B, 1.00KB");
    // 1023.999 gets rounded up for precision 2, so result is 1024.00KB not 1023.99KB
    TestFormat(wdFmt("{0}, {1}", wdArgFileSize(1024u * 1024u - 1u), wdArgFileSize(1024u * 1024u)), "1024.00KB, 1.00MB");

    const char* const suffixes[] = {" Foo", " Bar", " Foobar"};
    const wdUInt32 suffixCount = WD_ARRAY_SIZE(suffixes);
    TestFormat(wdFmt("{0}", wdArgHumanReadable(0ll, 25u, suffixes, suffixCount)), "0 Foo");
    TestFormat(wdFmt("{0}", wdArgHumanReadable(25ll, 25u, suffixes, suffixCount)), "1.00 Bar");
    TestFormat(wdFmt("{0}", wdArgHumanReadable(25ll * 25ll * 2ll, 25u, suffixes, suffixCount)), "2.00 Foobar");

    TestFormat(wdFmt("{0}", wdArgHumanReadable(-0ll, 25u, suffixes, suffixCount)), "0 Foo");
    TestFormat(wdFmt("{0}", wdArgHumanReadable(-25ll, 25u, suffixes, suffixCount)), "-1.00 Bar");
    TestFormat(wdFmt("{0}", wdArgHumanReadable(-25ll * 25ll * 2ll, 25u, suffixes, suffixCount)), "-2.00 Foobar");

    TestFormat(wdFmt("'{0}, {1}'", "inl", sz), "'inl, sz'");
    TestFormat(wdFmt("'{0}'", string), "'string'");
    TestFormat(wdFmt("'{0}'", sb), "'builder'");
    TestFormat(wdFmt("'{0}'", sv), "'view'");

    TestFormat(wdFmt("{3}, {1}, {0}, {2}", wdArgF(23.12345f, 1), wdArgI(42), 17, 12.34f), "12.34, 42, 23.1, 17");

    const wchar_t* wsz = L"wsz";
    TestFormatWChar(wdFmt("'{0}, {1}'", "inl", wsz), L"'inl, wsz'");
    TestFormatWChar(wdFmt("'{0}, {1}'", L"inl", wsz), L"'inl, wsz'");
    // Temp buffer limit is 63 byte (64 including trailing zero). Each character in UTF-8 can potentially use 4 byte.
    // All input characters are 1 byte, so the 60th character is the last with 4 bytes left in the buffer.
    // Thus we end up with truncation after 60 characters.
    const wchar_t* wszTooLong = L"123456789.123456789.123456789.123456789.123456789.123456789.WAAAAAAAAAAAAAAH";
    const wchar_t* wszTooLongExpected = L"123456789.123456789.123456789.123456789.123456789.123456789.";
    const wchar_t* wszTooLongExpected2 =
      L"'123456789.123456789.123456789.123456789.123456789.123456789., 123456789.123456789.123456789.123456789.123456789.123456789.'";
    TestFormatWChar(wdFmt("{0}", wszTooLong), wszTooLongExpected);
    TestFormatWChar(wdFmt("'{0}, {1}'", wszTooLong, wszTooLong), wszTooLongExpected2);
  }

  WD_TEST_BLOCK(wdTestBlock::DisabledNoWarning, "Compare Performance")
  {
    CompareSnprintf(perfLog, wdFmt("Hello {0}, i = {1}, f = {2}", "World", 42, wdArgF(3.141f, 2)), "Hello %s, i = %i, f = %.2f", "World", 42, 3.141f);
    CompareSnprintf(perfLog, wdFmt("No formatting at all"), "No formatting at all");
    CompareSnprintf(perfLog, wdFmt("{0}, {1}, {2}, {3}, {4}", "AAAAAA", "BBBBBBB", "CCCCCC", "DDDDDDDDDDDDD", "EE"), "%s, %s, %s, %s, %s", "AAAAAA",
      "BBBBBBB", "CCCCCC", "DDDDDDDDDDDDD", "EE");
    CompareSnprintf(perfLog, wdFmt("{0}", 23), "%i", 23);
    CompareSnprintf(perfLog, wdFmt("{0}", 23.123456789), "%f", 23.123456789);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgF(23.123456789, 2)), "%.2f", 23.123456789);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgI(123456789, 20, true)), "%020i", 123456789);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgI(123456789, 20, true, 16)), "%020X", 123456789);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgU(1234567890987ll, 30, false, 16)), "%30llx", 1234567890987ll);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgU(1234567890987ll, 30, false, 16, true)), "%30llX", 1234567890987ll);
    CompareSnprintf(perfLog, wdFmt("{0}, {1}, {2}, {3}, {4}", 0, 1, 2, 3, 4), "%i, %i, %i, %i, %i", 0, 1, 2, 3, 4);
    CompareSnprintf(perfLog, wdFmt("{0}, {1}, {2}, {3}, {4}", 0.1, 1.1, 2.1, 3.1, 4.1), "%.1f, %.1f, %.1f, %.1f, %.1f", 0.1, 1.1, 2.1, 3.1, 4.1);
    CompareSnprintf(perfLog, wdFmt("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9),
      "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    CompareSnprintf(perfLog, wdFmt("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}", 0.1, 1.1, 2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1, 9.1),
      "%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f", 0.1, 1.1, 2.1, 3.1, 4.1, 5.1, 6.1, 7.1, 8.1, 9.1);
    CompareSnprintf(perfLog, wdFmt("{0}", wdArgC('z')), "%c", 'z');

    CompareSnprintf(perfLog, wdFmt("{}, {}, {}, {}, {}, {}, {}, {}, {}, {}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9), "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    // FILE* file = fopen("D:\\snprintf_perf.txt", "wb");
    // if (file)
    //{
    //  fwrite(perfLog.GetData(), 1, perfLog.GetElementCount(), file);
    //  fclose(file);
    //}
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Auto Increment")
  {
    TestFormat(wdFmt("{}, {}, {}, {}", wdInt8(-1), wdInt16(-2), wdInt32(-3), wdInt64(-4)), "-1, -2, -3, -4");
    TestFormat(wdFmt("{}, {}, {}, {}", wdUInt8(1), wdUInt16(2), wdUInt32(3), wdUInt64(4)), "1, 2, 3, 4");

    TestFormat(wdFmt("{0}, {}, {}, {}", wdUInt8(1), wdUInt16(2), wdUInt32(3), wdUInt64(4)), "1, 2, 3, 4");

    TestFormat(wdFmt("{1}, {}, {}, {}", wdUInt8(1), wdUInt16(2), wdUInt32(3), wdUInt64(4), wdUInt64(5)), "2, 3, 4, 5");

    TestFormat(wdFmt("{2}, {}, {1}, {}", wdUInt8(1), wdUInt16(2), wdUInt32(3), wdUInt64(4), wdUInt64(5)), "3, 4, 2, 3");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdTime")
  {
    TestFormat(wdFmt("{}", wdTime()), "0ns");
    TestFormat(wdFmt("{}", wdTime::Nanoseconds(999)), "999ns");
    TestFormat(wdFmt("{}", wdTime::Nanoseconds(999.1)), "999.1ns");
    TestFormat(wdFmt("{}", wdTime::Microseconds(999)), u8"999\u00B5s");     // Utf-8 encoding for the microsecond sign
    TestFormat(wdFmt("{}", wdTime::Microseconds(999.2)), u8"999.2\u00B5s"); // Utf-8 encoding for the microsecond sign
    TestFormat(wdFmt("{}", wdTime::Milliseconds(-999)), "-999ms");
    TestFormat(wdFmt("{}", wdTime::Milliseconds(-999.3)), "-999.3ms");
    TestFormat(wdFmt("{}", wdTime::Seconds(59)), "59sec");
    TestFormat(wdFmt("{}", wdTime::Seconds(-59.9)), "-59.9sec");
    TestFormat(wdFmt("{}", wdTime::Seconds(75)), "1min 15sec");
    TestFormat(wdFmt("{}", wdTime::Seconds(-75.4)), "-1min 15sec");
    TestFormat(wdFmt("{}", wdTime::Minutes(59)), "59min 0sec");
    TestFormat(wdFmt("{}", wdTime::Minutes(-1)), "-1min 0sec");
    TestFormat(wdFmt("{}", wdTime::Minutes(90)), "1h 30min 0sec");
    TestFormat(wdFmt("{}", wdTime::Minutes(-90.5)), "-1h 30min 30sec");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdDateTime")
  {
    {
      wdDateTime dt;
      dt.SetYear(2019);
      dt.SetMonth(6);
      dt.SetDay(12);
      dt.SetHour(13);
      dt.SetMinute(26);
      dt.SetSecond(51);
      dt.SetMicroseconds(7000);

      TestFormat(wdFmt("{}", dt), "2019-06-12_13-26-51-007");
    }

    {
      wdDateTime dt;
      dt.SetYear(0);
      dt.SetMonth(1);
      dt.SetDay(1);
      dt.SetHour(0);
      dt.SetMinute(0);
      dt.SetSecond(0);
      dt.SetMicroseconds(0);

      TestFormat(wdFmt("{}", dt), "0000-01-01_00-00-00-000");
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sensitive Info")
  {
    auto prev = wdArgSensitive::s_BuildStringCB;
    WD_SCOPE_EXIT(wdArgSensitive::s_BuildStringCB = prev);

    wdArgSensitive::s_BuildStringCB = wdArgSensitive::BuildString_SensitiveUserData_Hash;

    wdStringBuilder fmt;

    fmt.Format("Password: {}", wdArgSensitive("hunter2", "pwd"));
    WD_TEST_STRING(fmt, "Password: sud:pwd#96d66ce6($7)");

    fmt.Format("Password: {}", wdArgSensitive("hunter2"));
    WD_TEST_STRING(fmt, "Password: sud:#96d66ce6($7)");
  }
}
