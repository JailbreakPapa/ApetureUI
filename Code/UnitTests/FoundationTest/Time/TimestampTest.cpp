#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/ThreadUtils.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Time/Timestamp.h>

WD_CREATE_SIMPLE_TEST(Time, Timestamp)
{
  const wdInt64 iFirstContactUnixTimeInSeconds = 2942956800LL;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructors / Valid Check")
  {
    wdTimestamp invalidTimestamp;
    WD_TEST_BOOL(!invalidTimestamp.IsValid());

    wdTimestamp validTimestamp(0, wdSIUnitOfTime::Second);
    WD_TEST_BOOL(validTimestamp.IsValid());
    validTimestamp.Invalidate();
    WD_TEST_BOOL(!validTimestamp.IsValid());

    wdTimestamp currentTimestamp = wdTimestamp::CurrentTimestamp();
    // Kind of hard to hit a moving target, let's just test if it is in a probable range.
    WD_TEST_BOOL(currentTimestamp.IsValid());
    WD_TEST_BOOL_MSG(currentTimestamp.GetInt64(wdSIUnitOfTime::Second) > 1384597970LL, "The current time is before this test was written!");
    WD_TEST_BOOL_MSG(currentTimestamp.GetInt64(wdSIUnitOfTime::Second) < 32531209845LL,
      "This current time is after the year 3000! If this is actually the case, please fix this test.");

    // Sleep for 10 milliseconds
    wdThreadUtils::Sleep(wdTime::Milliseconds(10));
    WD_TEST_BOOL_MSG(currentTimestamp.GetInt64(wdSIUnitOfTime::Microsecond) < wdTimestamp::CurrentTimestamp().GetInt64(wdSIUnitOfTime::Microsecond),
      "Sleeping for 10 ms should cause the timestamp to change!");
    WD_TEST_BOOL_MSG(!currentTimestamp.Compare(wdTimestamp::CurrentTimestamp(), wdTimestamp::CompareMode::Identical),
      "Sleeping for 10 ms should cause the timestamp to change!");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Public Accessors")
  {
    const wdTimestamp epoch(0, wdSIUnitOfTime::Second);
    const wdTimestamp firstContact(iFirstContactUnixTimeInSeconds, wdSIUnitOfTime::Second);
    WD_TEST_BOOL(epoch.IsValid());
    WD_TEST_BOOL(firstContact.IsValid());

    // GetInt64 / SetInt64
    wdTimestamp firstContactTest(iFirstContactUnixTimeInSeconds, wdSIUnitOfTime::Second);
    WD_TEST_INT(firstContactTest.GetInt64(wdSIUnitOfTime::Second), iFirstContactUnixTimeInSeconds);
    WD_TEST_INT(firstContactTest.GetInt64(wdSIUnitOfTime::Millisecond), iFirstContactUnixTimeInSeconds * 1000LL);
    WD_TEST_INT(firstContactTest.GetInt64(wdSIUnitOfTime::Microsecond), iFirstContactUnixTimeInSeconds * 1000000LL);
    WD_TEST_INT(firstContactTest.GetInt64(wdSIUnitOfTime::Nanosecond), iFirstContactUnixTimeInSeconds * 1000000000LL);

    firstContactTest.SetInt64(firstContactTest.GetInt64(wdSIUnitOfTime::Second), wdSIUnitOfTime::Second);
    WD_TEST_BOOL(firstContactTest.Compare(firstContact, wdTimestamp::CompareMode::Identical));
    firstContactTest.SetInt64(firstContactTest.GetInt64(wdSIUnitOfTime::Millisecond), wdSIUnitOfTime::Millisecond);
    WD_TEST_BOOL(firstContactTest.Compare(firstContact, wdTimestamp::CompareMode::Identical));
    firstContactTest.SetInt64(firstContactTest.GetInt64(wdSIUnitOfTime::Microsecond), wdSIUnitOfTime::Microsecond);
    WD_TEST_BOOL(firstContactTest.Compare(firstContact, wdTimestamp::CompareMode::Identical));
    firstContactTest.SetInt64(firstContactTest.GetInt64(wdSIUnitOfTime::Nanosecond), wdSIUnitOfTime::Nanosecond);
    WD_TEST_BOOL(firstContactTest.Compare(firstContact, wdTimestamp::CompareMode::Identical));

    // IsEqual
    const wdTimestamp firstContactPlusAFewMicroseconds(firstContact.GetInt64(wdSIUnitOfTime::Microsecond) + 42, wdSIUnitOfTime::Microsecond);
    WD_TEST_BOOL(firstContact.Compare(firstContactPlusAFewMicroseconds, wdTimestamp::CompareMode::FileTimeEqual));
    WD_TEST_BOOL(!firstContact.Compare(firstContactPlusAFewMicroseconds, wdTimestamp::CompareMode::Identical));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operators")
  {
    const wdTimestamp firstContact(iFirstContactUnixTimeInSeconds, wdSIUnitOfTime::Second);

    // Time span arithmetics
    const wdTime timeSpan1000s = wdTime::Seconds(1000);
    WD_TEST_BOOL(timeSpan1000s.GetMicroseconds() == 1000000000LL);

    // operator +
    const wdTimestamp firstContactPlus1000s = firstContact + timeSpan1000s;
    wdInt64 iSpanDiff = firstContactPlus1000s.GetInt64(wdSIUnitOfTime::Microsecond) - firstContact.GetInt64(wdSIUnitOfTime::Microsecond);
    WD_TEST_BOOL(iSpanDiff == 1000000000LL);
    // You can only subtract points in time
    WD_TEST_BOOL(firstContactPlus1000s - firstContact == timeSpan1000s);

    const wdTimestamp T1000sPlusFirstContact = timeSpan1000s + firstContact;
    iSpanDiff = T1000sPlusFirstContact.GetInt64(wdSIUnitOfTime::Microsecond) - firstContact.GetInt64(wdSIUnitOfTime::Microsecond);
    WD_TEST_BOOL(iSpanDiff == 1000000000LL);
    // You can only subtract points in time
    WD_TEST_BOOL(T1000sPlusFirstContact - firstContact == timeSpan1000s);

    // operator -
    const wdTimestamp firstContactMinus1000s = firstContact - timeSpan1000s;
    iSpanDiff = firstContactMinus1000s.GetInt64(wdSIUnitOfTime::Microsecond) - firstContact.GetInt64(wdSIUnitOfTime::Microsecond);
    WD_TEST_BOOL(iSpanDiff == -1000000000LL);
    // You can only subtract points in time
    WD_TEST_BOOL(firstContact - firstContactMinus1000s == timeSpan1000s);


    // operator += / -=
    wdTimestamp testTimestamp = firstContact;
    testTimestamp += timeSpan1000s;
    WD_TEST_BOOL(testTimestamp.Compare(firstContactPlus1000s, wdTimestamp::CompareMode::Identical));
    testTimestamp -= timeSpan1000s;
    WD_TEST_BOOL(testTimestamp.Compare(firstContact, wdTimestamp::CompareMode::Identical));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdDateTime conversion")
  {
    // Constructor
    wdDateTime invalidDateTime;
    WD_TEST_BOOL(!invalidDateTime.GetTimestamp().IsValid());

    const wdTimestamp firstContact(iFirstContactUnixTimeInSeconds, wdSIUnitOfTime::Second);
    wdDateTime firstContactDataTime(firstContact);

    // Getter
    WD_TEST_INT(firstContactDataTime.GetYear(), 2063);
    WD_TEST_INT(firstContactDataTime.GetMonth(), 4);
    WD_TEST_INT(firstContactDataTime.GetDay(), 5);
    WD_TEST_BOOL(firstContactDataTime.GetDayOfWeek() == 4 ||
                 firstContactDataTime.GetDayOfWeek() == 255); // not supported on all platforms, should output 255 then
    WD_TEST_INT(firstContactDataTime.GetHour(), 0);
    WD_TEST_INT(firstContactDataTime.GetMinute(), 0);
    WD_TEST_INT(firstContactDataTime.GetSecond(), 0);
    WD_TEST_INT(firstContactDataTime.GetMicroseconds(), 0);

    // SetTimestamp / GetTimestamp
    wdTimestamp currentTimestamp = wdTimestamp::CurrentTimestamp();
    wdDateTime currentDateTime;
    currentDateTime.SetTimestamp(currentTimestamp);
    wdTimestamp currentTimestamp2 = currentDateTime.GetTimestamp();
    // OS date time functions should be accurate within one second.
    wdInt64 iDiff = wdMath::Abs(currentTimestamp.GetInt64(wdSIUnitOfTime::Microsecond) - currentTimestamp2.GetInt64(wdSIUnitOfTime::Microsecond));
    WD_TEST_BOOL(iDiff <= 1000000);

    // Setter
    wdDateTime oneSmallStep;
    oneSmallStep.SetYear(1969);
    oneSmallStep.SetMonth(7);
    oneSmallStep.SetDay(21);
    oneSmallStep.SetDayOfWeek(1);
    oneSmallStep.SetHour(2);
    oneSmallStep.SetMinute(56);
    oneSmallStep.SetSecond(0);
    oneSmallStep.SetMicroseconds(0);

    wdTimestamp oneSmallStepTimestamp = oneSmallStep.GetTimestamp();
    WD_TEST_BOOL(oneSmallStepTimestamp.IsValid());
    WD_TEST_INT(oneSmallStepTimestamp.GetInt64(wdSIUnitOfTime::Second), -14159040LL);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdDateTime formatting")
  {
    wdDateTime dateTime;

    dateTime.SetYear(2019);
    dateTime.SetMonth(8);
    dateTime.SetDay(16);
    dateTime.SetDayOfWeek(5);
    dateTime.SetHour(13);
    dateTime.SetMinute(40);
    dateTime.SetSecond(30);
    dateTime.SetMicroseconds(345678);

    char szTimestampFormatted[256] = "";

    // no names, no UTC, no milliseconds
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::Default));
    WD_TEST_STRING("2019-08-16 - 13:40:30", szTimestampFormatted);
    // no names, no UTC, with milliseconds
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::Default | wdArgDateTime::ShowMilliseconds));
    WD_TEST_STRING("2019-08-16 - 13:40:30.345", szTimestampFormatted);
    // no names, with UTC, no milliseconds
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::Default | wdArgDateTime::ShowTimeZone));
    WD_TEST_STRING("2019-08-16 - 13:40:30 (UTC)", szTimestampFormatted);
    // no names, with UTC, with milliseconds
    BuildString(
      szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::ShowDate | wdArgDateTime::ShowMilliseconds | wdArgDateTime::ShowTimeZone));
    WD_TEST_STRING("2019-08-16 - 13:40:30.345 (UTC)", szTimestampFormatted);
    // with names, no UTC, no milliseconds
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::DefaultTextual | wdArgDateTime::ShowWeekday));
    WD_TEST_STRING("2019 Aug 16 (Fri) - 13:40:30", szTimestampFormatted);
    // no names, no UTC, with milliseconds
    BuildString(szTimestampFormatted, 256,
      wdArgDateTime(dateTime, wdArgDateTime::DefaultTextual | wdArgDateTime::ShowWeekday | wdArgDateTime::ShowMilliseconds));
    WD_TEST_STRING("2019 Aug 16 (Fri) - 13:40:30.345", szTimestampFormatted);
    // no names, with UTC, no milliseconds
    BuildString(
      szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::DefaultTextual | wdArgDateTime::ShowWeekday | wdArgDateTime::ShowTimeZone));
    WD_TEST_STRING("2019 Aug 16 (Fri) - 13:40:30 (UTC)", szTimestampFormatted);
    // no names, with UTC, with milliseconds
    BuildString(szTimestampFormatted, 256,
      wdArgDateTime(
        dateTime, wdArgDateTime::DefaultTextual | wdArgDateTime::ShowWeekday | wdArgDateTime::ShowMilliseconds | wdArgDateTime::ShowTimeZone));
    WD_TEST_STRING("2019 Aug 16 (Fri) - 13:40:30.345 (UTC)", szTimestampFormatted);

    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::ShowDate));
    WD_TEST_STRING("2019-08-16", szTimestampFormatted);
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::TextualDate));
    WD_TEST_STRING("2019 Aug 16", szTimestampFormatted);
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::ShowTime));
    WD_TEST_STRING("13:40", szTimestampFormatted);
    BuildString(szTimestampFormatted, 256, wdArgDateTime(dateTime, wdArgDateTime::ShowWeekday | wdArgDateTime::ShowMilliseconds));
    WD_TEST_STRING("(Fri) - 13:40:30.345", szTimestampFormatted);
  }
}
