#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Communication/Event.h>

WD_CREATE_SIMPLE_TEST_GROUP(Communication);

namespace
{
  struct Test
  {
    void DoStuff(wdInt32* pEventData) { *pEventData += m_iData; }

    wdInt32 m_iData;
  };

  struct TestRecursion
  {
    TestRecursion() { m_uiRecursionCount = 0; }
    void DoStuff(wdUInt32 uiRecursions)
    {
      if (m_uiRecursionCount < uiRecursions)
      {
        m_uiRecursionCount++;
        m_Event.Broadcast(uiRecursions, 10);
      }
    }

    typedef wdEvent<wdUInt32> Event;
    Event m_Event;
    wdUInt32 m_uiRecursionCount;
  };
} // namespace

WD_CREATE_SIMPLE_TEST(Communication, Event)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basics")
  {
    typedef wdEvent<wdInt32*> TestEvent;
    TestEvent e;

    Test test1;
    test1.m_iData = 3;

    Test test2;
    test2.m_iData = 5;

    wdInt32 iResult = 0;

    e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test1));
    WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));

    iResult = 0;
    e.Broadcast(&iResult);

    WD_TEST_INT(iResult, 3);

    e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test2));
    WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));

    iResult = 0;
    e.Broadcast(&iResult);

    WD_TEST_INT(iResult, 8);

    e.RemoveEventHandler(TestEvent::Handler(&Test::DoStuff, &test1));
    WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));

    iResult = 0;
    e.Broadcast(&iResult);

    WD_TEST_INT(iResult, 5);

    e.RemoveEventHandler(TestEvent::Handler(&Test::DoStuff, &test2));
    WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));

    iResult = 0;
    e.Broadcast(&iResult);

    WD_TEST_INT(iResult, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Unsubscribing via ID")
  {
    typedef wdEvent<wdInt32*> TestEvent;
    TestEvent e;

    Test test1;
    Test test2;

    auto subId1 = e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test1));
    WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));

    auto subId2 = e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test2));
    WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));

    e.RemoveEventHandler(subId1);
    WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));

    e.RemoveEventHandler(subId2);
    WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Unsubscribing via Unsubscriber")
  {
    typedef wdEvent<wdInt32*> TestEvent;
    TestEvent e;

    Test test1;
    Test test2;

    {
      TestEvent::Unsubscriber unsub1;

      {
        TestEvent::Unsubscriber unsub2;

        e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test1), unsub1);
        WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));

        e.AddEventHandler(TestEvent::Handler(&Test::DoStuff, &test2), unsub2);
        WD_TEST_BOOL(e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));
      }

      WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test2)));
    }

    WD_TEST_BOOL(!e.HasEventHandler(TestEvent::Handler(&Test::DoStuff, &test1)));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Recursion")
  {
    for (wdUInt32 i = 0; i < 10; i++)
    {
      TestRecursion test;
      test.m_Event.AddEventHandler(TestRecursion::Event::Handler(&TestRecursion::DoStuff, &test));
      test.m_Event.Broadcast(i, 10);
      WD_TEST_INT(test.m_uiRecursionCount, i);
      test.m_Event.RemoveEventHandler(TestRecursion::Event::Handler(&TestRecursion::DoStuff, &test));
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Remove while iterate")
  {
    typedef wdEvent<int, wdMutex, wdDefaultAllocatorWrapper, wdEventType::CopyOnBroadcast> TestEvent;
    TestEvent e;

    wdUInt32 callMap = 0;

    wdEventSubscriptionID subscriptions[4] = {};

    subscriptions[0] = e.AddEventHandler(TestEvent::Handler([&](int i) { callMap |= WD_BIT(0); }));

    subscriptions[1] = e.AddEventHandler(TestEvent::Handler([&](int i) {
      callMap |= WD_BIT(1);
      e.RemoveEventHandler(subscriptions[1]);
    }));

    subscriptions[2] = e.AddEventHandler(TestEvent::Handler([&](int i) {
      callMap |= WD_BIT(2);
      e.RemoveEventHandler(subscriptions[2]);
      e.RemoveEventHandler(subscriptions[3]);
    }));

    subscriptions[3] = e.AddEventHandler(TestEvent::Handler([&](int i) { callMap |= WD_BIT(3); }));

    e.Broadcast(0);

    WD_TEST_BOOL(callMap == (WD_BIT(0) | WD_BIT(1) | WD_BIT(2) | WD_BIT(3)));

    callMap = 0;
    e.Broadcast(0);
    WD_TEST_BOOL(callMap == WD_BIT(0));

    e.RemoveEventHandler(subscriptions[0]);
  }
}
