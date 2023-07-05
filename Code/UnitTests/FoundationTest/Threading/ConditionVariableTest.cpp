#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/ConditionVariable.h>

#include <Foundation/Threading/Mutex.h>
#include <Foundation/Threading/Thread.h>
#include <Foundation/Types/UniquePtr.h>

namespace
{
  class TestThread : public wdThread
  {
  public:
    TestThread()
      : wdThread("Test Thread")
    {
    }

    wdConditionVariable* m_pCV = nullptr;
    wdAtomicInteger32* m_pCounter = nullptr;

    virtual wdUInt32 Run()
    {
      WD_LOCK(*m_pCV);

      m_pCounter->Decrement();

      m_pCV->UnlockWaitForSignalAndLock();

      m_pCounter->Increment();
      return 0;
    }
  };

  class TestThreadTimeout : public wdThread
  {
  public:
    TestThreadTimeout()
      : wdThread("Test Thread Timeout")
    {
    }

    wdConditionVariable* m_pCV = nullptr;
    wdConditionVariable* m_pCVTimeout = nullptr;
    wdAtomicInteger32* m_pCounter = nullptr;

    virtual wdUInt32 Run()
    {
      // make sure all threads are put to sleep first
      {
        WD_LOCK(*m_pCV);
        m_pCounter->Decrement();
        m_pCV->UnlockWaitForSignalAndLock();
      }

      // this condition will never be met during the test
      // it should always run into the timeout
      WD_LOCK(*m_pCVTimeout);
      m_pCVTimeout->UnlockWaitForSignalAndLock(wdTime::Seconds(0.5));

      m_pCounter->Increment();
      return 0;
    }
  };
} // namespace

WD_CREATE_SIMPLE_TEST(Threading, ConditionalVariable)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Wait No Timeout")
  {
    constexpr wdUInt32 uiNumThreads = 32;

    wdUniquePtr<TestThread> pTestThreads[uiNumThreads];
    wdAtomicInteger32 iCounter = uiNumThreads;
    wdConditionVariable cv;

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThreads[i] = WD_DEFAULT_NEW(TestThread);
      pTestThreads[i]->m_pCounter = &iCounter;
      pTestThreads[i]->m_pCV = &cv;
      pTestThreads[i]->Start();
    }

    // wait until all threads are in waiting state
    while (true)
    {
      // We need to lock here as otherwise we could signal
      // while a thread hasn't reached the wait yet.
      WD_LOCK(cv);
      if (iCounter == 0)
        break;

      wdThreadUtils::YieldTimeSlice();
    }

    for (wdUInt32 t = 0; t < uiNumThreads / 2; ++t)
    {
      const wdInt32 iExpected = t + 1;

      cv.SignalOne();

      for (wdUInt32 a = 0; a < 1000; ++a)
      {
        wdThreadUtils::Sleep(wdTime::Milliseconds(1));

        if (iCounter >= iExpected)
          break;
      }

      // theoretically this could fail, if the OS doesn't wake up any other thread in time
      // but with 1000 tries that is very unlikely
      WD_TEST_INT(iCounter, iExpected);
      WD_TEST_BOOL(iCounter <= iExpected); // THIS test must never fail!
    }

    // wake up the rest
    {
      cv.SignalAll();

      for (wdUInt32 a = 0; a < 1000; ++a)
      {
        wdThreadUtils::Sleep(wdTime::Milliseconds(1));

        if (iCounter >= (wdInt32)uiNumThreads)
          break;
      }

      // theoretically this could fail, if the OS doesn't wake up any other thread in time
      // but with 1000 tries that is very unlikely
      WD_TEST_INT(iCounter, (wdInt32)uiNumThreads);
      WD_TEST_BOOL(iCounter <= (wdInt32)uiNumThreads); // THIS test must never fail!
    }

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThreads[i]->Join();
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Wait With timeout")
  {
    constexpr wdUInt32 uiNumThreads = 16;

    wdUniquePtr<TestThreadTimeout> pTestThreads[uiNumThreads];
    wdAtomicInteger32 iCounter = uiNumThreads;
    wdConditionVariable cv;
    wdConditionVariable cvt;

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThreads[i] = WD_DEFAULT_NEW(TestThreadTimeout);
      pTestThreads[i]->m_pCounter = &iCounter;
      pTestThreads[i]->m_pCV = &cv;
      pTestThreads[i]->m_pCVTimeout = &cvt;
      pTestThreads[i]->Start();
    }

    // wait until all threads are in waiting state
    while (true)
    {
      // We need to lock here as otherwise we could signal
      // while a thread hasn't reached the wait yet.
      WD_LOCK(cv);
      if (iCounter == 0)
        break;

      wdThreadUtils::YieldTimeSlice();
    }

    // open the flood gates
    cv.SignalAll();

    // all threads should run into their timeout now
    for (wdUInt32 a = 0; a < 100; ++a)
    {
      wdThreadUtils::Sleep(wdTime::Milliseconds(50));

      if (iCounter >= (wdInt32)uiNumThreads)
        break;
    }

    // theoretically this could fail, if the OS doesn't wake up any other thread in time
    // but with 100 tries that is very unlikely
    WD_TEST_INT(iCounter, (wdInt32)uiNumThreads);
    WD_TEST_BOOL(iCounter <= (wdInt32)uiNumThreads); // THIS test must never fail!

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThreads[i]->Join();
    }
  }
}
