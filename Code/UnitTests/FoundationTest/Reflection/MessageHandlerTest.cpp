#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Communication/Message.h>
#include <Foundation/Reflection/Reflection.h>

#ifdef GetMessage
#  undef GetMessage
#endif

namespace
{

  struct wdMsgTest : public wdMessage
  {
    WD_DECLARE_MESSAGE_TYPE(wdMsgTest, wdMessage);
  };

  WD_IMPLEMENT_MESSAGE_TYPE(wdMsgTest);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgTest, 1, wdRTTIDefaultAllocator<wdMsgTest>)
  WD_END_DYNAMIC_REFLECTED_TYPE;

  struct AddMessage : public wdMsgTest
  {
    WD_DECLARE_MESSAGE_TYPE(AddMessage, wdMsgTest);

    wdInt32 m_iValue;
  };
  WD_IMPLEMENT_MESSAGE_TYPE(AddMessage);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(AddMessage, 1, wdRTTIDefaultAllocator<AddMessage>)
  WD_END_DYNAMIC_REFLECTED_TYPE;

  struct SubMessage : public wdMsgTest
  {
    WD_DECLARE_MESSAGE_TYPE(SubMessage, wdMsgTest);

    wdInt32 m_iValue;
  };
  WD_IMPLEMENT_MESSAGE_TYPE(SubMessage);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(SubMessage, 1, wdRTTIDefaultAllocator<SubMessage>)
  WD_END_DYNAMIC_REFLECTED_TYPE;

  struct MulMessage : public wdMsgTest
  {
    WD_DECLARE_MESSAGE_TYPE(MulMessage, wdMsgTest);

    wdInt32 m_iValue;
  };
  WD_IMPLEMENT_MESSAGE_TYPE(MulMessage);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(MulMessage, 1, wdRTTIDefaultAllocator<MulMessage>)
  WD_END_DYNAMIC_REFLECTED_TYPE;

  struct GetMessage : public wdMsgTest
  {
    WD_DECLARE_MESSAGE_TYPE(GetMessage, wdMsgTest);

    wdInt32 m_iValue;
  };
  WD_IMPLEMENT_MESSAGE_TYPE(GetMessage);
  WD_BEGIN_DYNAMIC_REFLECTED_TYPE(GetMessage, 1, wdRTTIDefaultAllocator<GetMessage>)
  WD_END_DYNAMIC_REFLECTED_TYPE;
} // namespace

class BaseHandler : public wdReflectedClass
{
  WD_ADD_DYNAMIC_REFLECTION(BaseHandler, wdReflectedClass);

public:
  BaseHandler()
    : m_iValue(0)
  {
  }

  void OnAddMessage(AddMessage& ref_msg) { m_iValue += ref_msg.m_iValue; }

  void OnMulMessage(MulMessage& ref_msg) { m_iValue *= ref_msg.m_iValue; }

  void OnGetMessage(GetMessage& ref_msg) const { ref_msg.m_iValue = m_iValue; }

  wdInt32 m_iValue;
};

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(BaseHandler, 1, wdRTTINoAllocator)
{
  WD_BEGIN_MESSAGEHANDLERS{
      WD_MESSAGE_HANDLER(AddMessage, OnAddMessage),
      WD_MESSAGE_HANDLER(MulMessage, OnMulMessage),
      WD_MESSAGE_HANDLER(GetMessage, OnGetMessage),
  } WD_END_MESSAGEHANDLERS;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

class DerivedHandler : public BaseHandler
{
  WD_ADD_DYNAMIC_REFLECTION(DerivedHandler, BaseHandler);

public:
  void OnAddMessage(AddMessage& ref_msg) { m_iValue += ref_msg.m_iValue * 2; }

  void OnSubMessage(SubMessage& ref_msg) { m_iValue -= ref_msg.m_iValue; }
};

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(DerivedHandler, 1, wdRTTINoAllocator)
{
  WD_BEGIN_MESSAGEHANDLERS
  {
    WD_MESSAGE_HANDLER(AddMessage, OnAddMessage),
    WD_MESSAGE_HANDLER(SubMessage, OnSubMessage),
  }
  WD_END_MESSAGEHANDLERS;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

WD_CREATE_SIMPLE_TEST(Reflection, MessageHandler)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple Dispatch")
  {
    BaseHandler test;
    const wdRTTI* pRTTI = test.GetStaticRTTI();

    WD_TEST_BOOL(pRTTI->CanHandleMessage<AddMessage>());
    WD_TEST_BOOL(!pRTTI->CanHandleMessage<SubMessage>());
    WD_TEST_BOOL(pRTTI->CanHandleMessage<MulMessage>());
    WD_TEST_BOOL(pRTTI->CanHandleMessage<GetMessage>());

    AddMessage addMsg;
    addMsg.m_iValue = 4;
    bool handled = pRTTI->DispatchMessage(&test, addMsg);
    WD_TEST_BOOL(handled);

    WD_TEST_INT(test.m_iValue, 4);

    SubMessage subMsg;
    subMsg.m_iValue = 4;
    handled = pRTTI->DispatchMessage(&test, subMsg); // should do nothing
    WD_TEST_BOOL(!handled);

    WD_TEST_INT(test.m_iValue, 4);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Simple Dispatch const")
  {
    const BaseHandler test;
    const wdRTTI* pRTTI = test.GetStaticRTTI();

    AddMessage addMsg;
    addMsg.m_iValue = 4;
    bool handled = pRTTI->DispatchMessage(&test, addMsg);
    WD_TEST_BOOL(!handled); // should do nothing since object is const and the add message handler is non-const

    WD_TEST_INT(test.m_iValue, 0);

    GetMessage getMsg;
    getMsg.m_iValue = 12;
    handled = pRTTI->DispatchMessage(&test, getMsg);
    WD_TEST_BOOL(handled);
    WD_TEST_INT(getMsg.m_iValue, 0);

    WD_TEST_INT(test.m_iValue, 0); // object must not be modified
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Dispatch with inheritance")
  {
    DerivedHandler test;
    const wdRTTI* pRTTI = test.GetStaticRTTI();

    WD_TEST_BOOL(pRTTI->CanHandleMessage<AddMessage>());
    WD_TEST_BOOL(pRTTI->CanHandleMessage<SubMessage>());
    WD_TEST_BOOL(pRTTI->CanHandleMessage<MulMessage>());

    // message handler overridden by derived class
    AddMessage addMsg;
    addMsg.m_iValue = 4;
    bool handled = pRTTI->DispatchMessage(&test, addMsg);
    WD_TEST_BOOL(handled);

    WD_TEST_INT(test.m_iValue, 8);

    SubMessage subMsg;
    subMsg.m_iValue = 4;
    handled = pRTTI->DispatchMessage(&test, subMsg);
    WD_TEST_BOOL(handled);

    WD_TEST_INT(test.m_iValue, 4);

    // message handled by base class
    MulMessage mulMsg;
    mulMsg.m_iValue = 4;
    handled = pRTTI->DispatchMessage(&test, mulMsg);
    WD_TEST_BOOL(handled);

    WD_TEST_INT(test.m_iValue, 16);
  }
}
