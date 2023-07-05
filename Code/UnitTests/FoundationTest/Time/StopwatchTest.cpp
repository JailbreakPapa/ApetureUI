#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/ThreadUtils.h>
#include <Foundation/Time/Stopwatch.h>

WD_CREATE_SIMPLE_TEST(Time, Stopwatch)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "General Functionality")
  {
    wdStopwatch sw;

    wdThreadUtils::Sleep(wdTime::Milliseconds(50));

    sw.StopAndReset();
    sw.Resume();

    const wdTime t0 = sw.Checkpoint();

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));

    const wdTime t1 = sw.Checkpoint();

    wdThreadUtils::Sleep(wdTime::Milliseconds(20));

    const wdTime t2 = sw.Checkpoint();

    wdThreadUtils::Sleep(wdTime::Milliseconds(30));

    const wdTime t3 = sw.Checkpoint();

    const wdTime tTotal1 = sw.GetRunningTotal();

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));

    sw.Pause(); // frewde the current running total

    const wdTime tTotal2 = sw.GetRunningTotal();

    wdThreadUtils::Sleep(wdTime::Milliseconds(10)); // should not affect the running total anymore

    const wdTime tTotal3 = sw.GetRunningTotal();


    // these tests are deliberately written such that they cannot fail,
    // even when the OS is under heavy load

    WD_TEST_BOOL(t0 > wdTime::Milliseconds(5));
    WD_TEST_BOOL(t1 > wdTime::Milliseconds(5));
    WD_TEST_BOOL(t2 > wdTime::Milliseconds(5));
    WD_TEST_BOOL(t3 > wdTime::Milliseconds(5));


    WD_TEST_BOOL(t1 + t2 + t3 <= tTotal1);
    WD_TEST_BOOL(t0 + t1 + t2 + t3 > tTotal1);

    WD_TEST_BOOL(tTotal1 < tTotal2);
    WD_TEST_BOOL(tTotal1 < tTotal3);
    WD_TEST_BOOL(tTotal2 == tTotal3);
  }
}
