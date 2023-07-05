#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/ThreadUtils.h>
#include <Foundation/Time/Clock.h>

class wdSimpleTimeStepSmoother : public wdTimeStepSmoothing
{
public:
  virtual wdTime GetSmoothedTimeStep(wdTime rawTimeStep, const wdClock* pClock) override { return wdTime::Seconds(0.42); }

  virtual void Reset(const wdClock* pClock) override {}
};

WD_CREATE_SIMPLE_TEST(Time, Clock)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constructor / Reset")
  {
    wdClock c("Test"); // calls 'Reset' internally

    WD_TEST_BOOL(c.GetTimeStepSmoothing() == nullptr); // after constructor

    WD_TEST_DOUBLE(c.GetAccumulatedTime().GetSeconds(), 0.0, 0.0);
    WD_TEST_DOUBLE(c.GetFixedTimeStep().GetSeconds(), 0.0, 0.0);
    WD_TEST_DOUBLE(c.GetSpeed(), 1.0, 0.0);
    WD_TEST_BOOL(c.GetPaused() == false);
    WD_TEST_DOUBLE(c.GetMinimumTimeStep().GetSeconds(), 0.001, 0.0); // to ensure the tests fail if somebody changes these constants
    WD_TEST_DOUBLE(c.GetMaximumTimeStep().GetSeconds(), 0.1, 0.0);   // to ensure the tests fail if somebody changes these constants
    WD_TEST_BOOL(c.GetTimeDiff() > wdTime::Seconds(0.0));

    wdSimpleTimeStepSmoother s;

    c.SetTimeStepSmoothing(&s);

    WD_TEST_BOOL(c.GetTimeStepSmoothing() == &s);

    c.Reset(false);

    // does NOT reset which time step smoother to use
    WD_TEST_BOOL(c.GetTimeStepSmoothing() == &s);

    c.Reset(true);
    WD_TEST_BOOL(c.GetTimeStepSmoothing() == nullptr); // after constructor
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetPaused / GetPaused")
  {
    wdClock c("Test");
    WD_TEST_BOOL(!c.GetPaused());

    c.SetPaused(true);
    WD_TEST_BOOL(c.GetPaused());

    c.SetPaused(false);
    WD_TEST_BOOL(!c.GetPaused());

    c.SetPaused(true);
    WD_TEST_BOOL(c.GetPaused());

    c.Reset(false);
    WD_TEST_BOOL(!c.GetPaused());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Updates while Paused / Unpaused")
  {
    wdClock c("Test");

    c.SetPaused(false);

    const wdTime t0 = c.GetAccumulatedTime();

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));
    c.Update();

    const wdTime t1 = c.GetAccumulatedTime();
    WD_TEST_BOOL(t0 < t1);

    c.SetPaused(true);

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));
    c.Update();

    const wdTime t2 = c.GetAccumulatedTime();
    WD_TEST_BOOL(t1 == t2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetFixedTimeStep / GetFixedTimeStep")
  {
    wdClock c("Test");

    WD_TEST_DOUBLE(c.GetFixedTimeStep().GetSeconds(), 0.0, 0.0);

    c.SetFixedTimeStep(wdTime::Seconds(1.0 / 60.0));

    WD_TEST_DOUBLE(c.GetFixedTimeStep().GetSeconds(), 1.0 / 60.0, 0.000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Updates with fixed time step")
  {
    wdClock c("Test");
    c.SetFixedTimeStep(wdTime::Seconds(1.0 / 60.0));
    c.Update();

    wdThreadUtils::Sleep(wdTime::Milliseconds(10));

    c.Update();
    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), 1.0 / 60.0, 0.000001);

    wdThreadUtils::Sleep(wdTime::Milliseconds(50));

    c.Update();
    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), 1.0 / 60.0, 0.000001);

    c.Update();
    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), 1.0 / 60.0, 0.000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetAccumulatedTime / GetAccumulatedTime")
  {
    wdClock c("Test");

    c.SetAccumulatedTime(wdTime::Seconds(23.42));

    WD_TEST_DOUBLE(c.GetAccumulatedTime().GetSeconds(), 23.42, 0.000001);

    c.Update(); // by default after a SetAccumulatedTime the time diff should always be > 0

    WD_TEST_BOOL(c.GetTimeDiff().GetSeconds() > 0.0);

    const wdTime t0 = c.GetAccumulatedTime();

    wdThreadUtils::Sleep(wdTime::Milliseconds(5));
    c.Update();

    const wdTime t1 = c.GetAccumulatedTime();

    WD_TEST_BOOL(t1 > t0);
    WD_TEST_BOOL(c.GetTimeDiff().GetSeconds() > 0.0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetSpeed / GetSpeed / GetTimeDiff")
  {
    wdClock c("Test");
    WD_TEST_DOUBLE(c.GetSpeed(), 1.0, 0.0);

    c.SetFixedTimeStep(wdTime::Seconds(0.01));

    c.SetSpeed(10.0);
    WD_TEST_DOUBLE(c.GetSpeed(), 10.0, 0.000001);

    c.Update();
    const wdTime t0 = c.GetTimeDiff();
    WD_TEST_DOUBLE(t0.GetSeconds(), 0.1, 0.00001);

    c.SetSpeed(0.1);

    c.Update();
    const wdTime t1 = c.GetTimeDiff();
    WD_TEST_DOUBLE(t1.GetSeconds(), 0.001, 0.00001);

    c.Reset(false);

    c.Update();
    const wdTime t2 = c.GetTimeDiff();
    WD_TEST_DOUBLE(t2.GetSeconds(), 0.01, 0.00001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetMinimumTimeStep / GetMinimumTimeStep")
  {
    wdClock c("Test");
    WD_TEST_DOUBLE(c.GetMinimumTimeStep().GetSeconds(), 0.001, 0.0); // to ensure the tests fail if somebody changes these constants

    c.Update();
    c.Update();

    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), c.GetMinimumTimeStep().GetSeconds(), 0.0000000001);

    c.SetMinimumTimeStep(wdTime::Seconds(0.1));
    c.SetMaximumTimeStep(wdTime::Seconds(1.0));

    WD_TEST_DOUBLE(c.GetMinimumTimeStep().GetSeconds(), 0.1, 0.0);

    c.Update();
    c.Update();

    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), c.GetMinimumTimeStep().GetSeconds(), 0.0000000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetMaximumTimeStep / GetMaximumTimeStep")
  {
    wdClock c("Test");
    WD_TEST_DOUBLE(c.GetMaximumTimeStep().GetSeconds(), 0.1, 0.0); // to ensure the tests fail if somebody changes these constants

    wdThreadUtils::Sleep(wdTime::Milliseconds(200));
    c.Update();

    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), c.GetMaximumTimeStep().GetSeconds(), 0.0000000001);

    c.SetMaximumTimeStep(wdTime::Seconds(0.2));

    WD_TEST_DOUBLE(c.GetMaximumTimeStep().GetSeconds(), 0.2, 0.0);

    wdThreadUtils::Sleep(wdTime::Milliseconds(400));
    c.Update();

    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), c.GetMaximumTimeStep().GetSeconds(), 0.0000000001);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "SetTimeStepSmoothing / GetTimeStepSmoothing")
  {
    wdClock c("Test");

    WD_TEST_BOOL(c.GetTimeStepSmoothing() == nullptr);

    wdSimpleTimeStepSmoother s;
    c.SetTimeStepSmoothing(&s);

    WD_TEST_BOOL(c.GetTimeStepSmoothing() == &s);

    c.SetMaximumTimeStep(wdTime::Seconds(10.0)); // this would limit the time step even after smoothing
    c.Update();

    WD_TEST_DOUBLE(c.GetTimeDiff().GetSeconds(), 0.42, 0.0);
  }
}
