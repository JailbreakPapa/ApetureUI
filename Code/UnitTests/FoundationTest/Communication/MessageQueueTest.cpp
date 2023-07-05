#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Communication/MessageQueue.h>

namespace
{
  struct wdMsgTest : public wdMessage
  {
    WD_DECLARE_MESSAGE_TYPE(wdMsgTest, wdMessage);
  };

  WD_IMPLEMENT_MESSAGE_TYPE(wdMsgTest);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgTest, 1, wdRTTIDefaultAllocator<wdMsgTest>)
  WD_END_DYNAMIC_REFLECTED_TYPE;

  struct TestMessage : public wdMsgTest
  {
    WD_DECLARE_MESSAGE_TYPE(TestMessage, wdMsgTest);

    int x;
    int y;
  };

  struct MetaData
  {
    int receiver;
  };

  typedef wdMessageQueue<MetaData> TestMessageQueue;

  WD_IMPLEMENT_MESSAGE_TYPE(TestMessage);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(TestMessage, 1, wdRTTIDefaultAllocator<TestMessage>)
  WD_END_DYNAMIC_REFLECTED_TYPE;
} // namespace

WD_CREATE_SIMPLE_TEST(Communication, MessageQueue)
{
  {
    TestMessage msg;
    WD_TEST_INT(msg.GetSize(), sizeof(TestMessage));
  }

  TestMessageQueue q;

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Enqueue")
  {
    for (wdUInt32 i = 0; i < 100; ++i)
    {
      TestMessage* pMsg = WD_DEFAULT_NEW(TestMessage);
      pMsg->x = rand();
      pMsg->y = rand();

      MetaData md;
      md.receiver = rand() % 10;

      q.Enqueue(pMsg, md);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Sorting")
  {
    struct MessageComparer
    {
      bool Less(const TestMessageQueue::Entry& a, const TestMessageQueue::Entry& b) const
      {
        if (a.m_MetaData.receiver != b.m_MetaData.receiver)
          return a.m_MetaData.receiver < b.m_MetaData.receiver;

        return a.m_pMessage->GetHash() < b.m_pMessage->GetHash();
      }
    };

    q.Sort(MessageComparer());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "operator[]")
  {
    WD_LOCK(q);

    wdMessage* pLastMsg = q[0].m_pMessage;
    MetaData lastMd = q[0].m_MetaData;

    for (wdUInt32 i = 1; i < q.GetCount(); ++i)
    {
      wdMessage* pMsg = q[i].m_pMessage;
      MetaData md = q[i].m_MetaData;

      if (md.receiver == lastMd.receiver)
      {
        WD_TEST_BOOL(pMsg->GetHash() >= pLastMsg->GetHash());
      }
      else
      {
        WD_TEST_BOOL(md.receiver >= lastMd.receiver);
      }
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Dequeue")
  {
    wdMessage* pMsg = nullptr;
    MetaData md;

    while (q.TryDequeue(pMsg, md))
    {
      WD_DEFAULT_DELETE(pMsg);
    }
  }
}
