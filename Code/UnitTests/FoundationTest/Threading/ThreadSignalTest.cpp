#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/Thread.h>
#include <Foundation/Threading/ThreadSignal.h>
#include <Foundation/Types/UniquePtr.h>

namespace
{
  class TestThread2 : public wdThread
  {
  public:
    TestThread2()
      : wdThread("Test Thread")
    {
    }

    wdThreadSignal* m_pSignalAuto = nullptr;
    wdThreadSignal* m_pSignalManual = nullptr;
    wdAtomicInteger32* m_pCounter = nullptr;
    bool m_bTimeout = false;

    virtual wdUInt32 Run()
    {
      m_pCounter->Decrement();

      m_pSignalAuto->WaitForSignal();

      m_pCounter->Increment();

      if (m_bTimeout)
      {
        m_pSignalManual->WaitForSignal(wdTime::Seconds(0.5));
      }
      else
      {
        m_pSignalManual->WaitForSignal();
      }

      m_pCounter->Increment();

      return 0;
    }
  };
} // namespace

WD_CREATE_SIMPLE_TEST(Threading, ThreadSignal)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Wait No Timeout")
  {
    constexpr wdUInt32 uiNumThreads = 32;

    wdUniquePtr<TestThread2> pTestThread2s[uiNumThreads];
    wdAtomicInteger32 iCounter = uiNumThreads;
    wdThreadSignal sigAuto(wdThreadSignal::Mode::AutoReset);
    wdThreadSignal sigManual(wdThreadSignal::Mode::ManualReset);

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThread2s[i] = WD_DEFAULT_NEW(TestThread2);
      pTestThread2s[i]->m_pCounter = &iCounter;
      pTestThread2s[i]->m_pSignalAuto = &sigAuto;
      pTestThread2s[i]->m_pSignalManual = &sigManual;
      pTestThread2s[i]->Start();
    }

    // wait until all threads are in waiting state
    while (iCounter > 0)
    {
      wdThreadUtils::YieldTimeSlice();
    }

    for (wdUInt32 t = 0; t < uiNumThreads; ++t)
    {
      const wdInt32 iExpected = t + 1;

      sigAuto.RaiseSignal();

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
      sigManual.RaiseSignal();

      for (wdUInt32 a = 0; a < 1000; ++a)
      {
        wdThreadUtils::Sleep(wdTime::Milliseconds(1));

        if (iCounter >= (wdInt32)uiNumThreads * 2)
          break;
      }

      // theoretically this could fail, if the OS doesn't wake up any other thread in time
      // but with 1000 tries that is very unlikely
      WD_TEST_INT(iCounter, (wdInt32)uiNumThreads * 2);
      WD_TEST_BOOL(iCounter <= (wdInt32)uiNumThreads * 2); // THIS test must never fail!
    }

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThread2s[i]->Join();
    }
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Wait With Timeout")
  {
    constexpr wdUInt32 uiNumThreads = 16;

    wdUniquePtr<TestThread2> pTestThread2s[uiNumThreads];
    wdAtomicInteger32 iCounter = uiNumThreads;
    wdThreadSignal sigAuto(wdThreadSignal::Mode::AutoReset);
    wdThreadSignal sigManual(wdThreadSignal::Mode::ManualReset);

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThread2s[i] = WD_DEFAULT_NEW(TestThread2);
      pTestThread2s[i]->m_pCounter = &iCounter;
      pTestThread2s[i]->m_pSignalAuto = &sigAuto;
      pTestThread2s[i]->m_pSignalManual = &sigManual;
      pTestThread2s[i]->m_bTimeout = true;
      pTestThread2s[i]->Start();
    }

    // wait until all threads are in waiting state
    while (iCounter > 0)
    {
      wdThreadUtils::YieldTimeSlice();
    }

    // raise the signal N times
    for (wdUInt32 t = 0; t < uiNumThreads; ++t)
    {
      sigAuto.RaiseSignal();

      for (wdUInt32 a = 0; a < 1000; ++a)
      {
        wdThreadUtils::Sleep(wdTime::Milliseconds(1));

        if (iCounter >= (wdInt32)t + 1)
          break;
      }
    }

    // due to the wait timeout in the thread, testing this exact value here would be unreliable
    // WD_TEST_INT(iCounter, (wdInt32)uiNumThreads);

    // just wait for the rest
    {
      for (wdUInt32 a = 0; a < 100; ++a)
      {
        wdThreadUtils::Sleep(wdTime::Milliseconds(50));

        if (iCounter >= (wdInt32)uiNumThreads * 2)
          break;
      }

      // theoretically this could fail, if the OS doesn't wake up any other thread in time
      // but with 1000 tries that is very unlikely
      WD_TEST_INT(iCounter, (wdInt32)uiNumThreads * 2);
      WD_TEST_BOOL(iCounter <= (wdInt32)uiNumThreads * 2); // THIS test must never fail!
    }

    for (wdUInt32 i = 0; i < uiNumThreads; ++i)
    {
      pTestThread2s[i]->Join();
    }
  }
}
