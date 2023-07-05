#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Time/Time.h>

WD_CREATE_SIMPLE_TEST_GROUP(Time);

WD_CREATE_SIMPLE_TEST(Time, Timer)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basics")
  {
    wdTime TestTime = wdTime::Now();

    WD_TEST_BOOL(TestTime.GetMicroseconds() > 0.0);

    volatile wdUInt32 testValue = 0;
    for (wdUInt32 i = 0; i < 42000; ++i)
    {
      testValue += 23;
    }

    wdTime TestTime2 = wdTime::Now();

    WD_TEST_BOOL(TestTime2.GetMicroseconds() > 0.0);

    TestTime2 -= TestTime;

    WD_TEST_BOOL(TestTime2.GetMicroseconds() > 0.0);
  }
}
