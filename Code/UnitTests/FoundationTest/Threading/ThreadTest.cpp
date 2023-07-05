#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Profiling/Profiling.h>
#include <Foundation/Threading/Mutex.h>
#include <Foundation/Threading/Thread.h>
#include <Foundation/Time/Time.h>

namespace
{
  volatile wdInt32 g_iCrossThreadVariable = 0;
  const wdUInt32 g_uiIncrementSteps = 160000;

  class TestThread3 : public wdThread
  {
  public:
    TestThread3()
      : wdThread("Test Thread")
    {
    }

    wdMutex* m_pWaitMutex = nullptr;
    wdMutex* m_pBlockedMutex = nullptr;

    virtual wdUInt32 Run()
    {
      // test TryLock on a locked mutex
      WD_TEST_BOOL(m_pBlockedMutex->TryLock().Failed());

      {
        // enter and leave the mutex once
        WD_LOCK(*m_pWaitMutex);
      }

      WD_PROFILE_SCOPE("Test Thread::Run");

      for (wdUInt32 i = 0; i < g_uiIncrementSteps; i++)
      {
        wdAtomicUtils::Increment(g_iCrossThreadVariable);

        wdTime::Now();
        wdThreadUtils::YieldTimeSlice();
        wdTime::Now();
      }

      return 0;
    }
  };
} // namespace

WD_CREATE_SIMPLE_TEST_GROUP(Threading);

WD_CREATE_SIMPLE_TEST(Threading, Thread)
{
  g_iCrossThreadVariable = 0;


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Thread")
  {
    TestThread3* pTestThread31 = nullptr;
    TestThread3* pTestThread32 = nullptr;

    /// the try-catch is necessary to quiet the static code analysis
    try
    {
      pTestThread31 = new TestThread3;
      pTestThread32 = new TestThread3;
    }
    catch (...)
    {
    }

    WD_TEST_BOOL(pTestThread31 != nullptr);
    WD_TEST_BOOL(pTestThread32 != nullptr);

    wdMutex waitMutex, blockedMutex;
    pTestThread31->m_pWaitMutex = &waitMutex;
    pTestThread32->m_pWaitMutex = &waitMutex;

    pTestThread31->m_pBlockedMutex = &blockedMutex;
    pTestThread32->m_pBlockedMutex = &blockedMutex;

    // no one holds these mutexes yet, must succeed
    WD_TEST_BOOL(blockedMutex.TryLock().Succeeded());
    WD_TEST_BOOL(waitMutex.TryLock().Succeeded());

    // Both thread will increment the global variable via atomic operations
    pTestThread31->Start();
    pTestThread32->Start();

    // give the threads a bit of time to start
    wdThreadUtils::Sleep(wdTime::Milliseconds(50));

    // allow the threads to run now
    waitMutex.Unlock();

    // Main thread will also increment the test variable
    wdAtomicUtils::Increment(g_iCrossThreadVariable);

    // Join with both threads
    pTestThread31->Join();
    pTestThread32->Join();

    // we are holding the mutex, another TryLock should work
    WD_TEST_BOOL(blockedMutex.TryLock().Succeeded());

    // The threads should have finished, no one holds the lock
    WD_TEST_BOOL(waitMutex.TryLock().Succeeded());

    // Test deletion
    delete pTestThread31;
    delete pTestThread32;

    WD_TEST_INT(g_iCrossThreadVariable, g_uiIncrementSteps * 2 + 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Thread Sleeping")
  {
    const wdTime start = wdTime::Now();

    wdTime sleepTime(wdTime::Seconds(0.3));

    wdThreadUtils::Sleep(sleepTime);

    const wdTime stop = wdTime::Now();

    const wdTime duration = stop - start;

    // We test for 0.25 - 0.35 since the threading functions are a bit varying in their precision
    WD_TEST_BOOL(duration.GetSeconds() > 0.25);
    WD_TEST_BOOL_MSG(duration.GetSeconds() < 1.0, "This test can fail when the machine is under too much load and blocks the process for too long.");
  }
}
