#pragma once

#include <Core/World/World.h>
#include <Foundation/Communication/Message.h>

/// \brief Base class for all messages that are sent as 'events'
struct WD_CORE_DLL wdEventMessage : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdEventMessage, wdMessage);

  wdGameObjectHandle m_hSenderObject;
  wdComponentHandle m_hSenderComponent;

  WD_ALWAYS_INLINE void FillFromSenderComponent(const wdComponent* pSenderComponent)
  {
    if (pSenderComponent != nullptr)
    {
      m_hSenderComponent = pSenderComponent->GetHandle();
      m_hSenderObject = pSenderComponent->GetOwner()->GetHandle();
    }
  }
};

namespace wdInternal
{
  struct WD_CORE_DLL EventMessageSenderHelper
  {
    static void SendEventMessage(wdMessage& ref_msg, wdComponent* pSenderComponent, wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers);
    static void SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers);
    static void PostEventMessage(const wdMessage& msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers, wdTime delay, wdObjectMsgQueueType::Enum queueType);
  };
} // namespace wdInternal

/// \brief A message sender that sends all messages to the next component derived from wdEventMessageHandlerComponent
///   up in the hierarchy starting with the given search object. If none is found the message is sent to
///   all components registered as global event message handler. The receiver is cached after the first send/post call.
template <typename EventMessageType>
class wdEventMessageSender : public wdMessageSenderBase<EventMessageType>
{
public:
  WD_ALWAYS_INLINE void SendEventMessage(EventMessageType& inout_msg, wdComponent* pSenderComponent, wdGameObject* pSearchObject)
  {
    if constexpr (WD_IS_DERIVED_FROM_STATIC(wdEventMessage, EventMessageType))
    {
      inout_msg.FillFromSenderComponent(pSenderComponent);
    }
    wdInternal::EventMessageSenderHelper::SendEventMessage(inout_msg, pSenderComponent, pSearchObject, m_CachedReceivers);
  }

  WD_ALWAYS_INLINE void SendEventMessage(EventMessageType& inout_msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject) const
  {
    if constexpr (WD_IS_DERIVED_FROM_STATIC(wdEventMessage, EventMessageType))
    {
      inout_msg.FillFromSenderComponent(pSenderComponent);
    }
    wdInternal::EventMessageSenderHelper::SendEventMessage(inout_msg, pSenderComponent, pSearchObject, m_CachedReceivers);
  }

  WD_ALWAYS_INLINE void PostEventMessage(EventMessageType& ref_msg, wdComponent* pSenderComponent, wdGameObject* pSearchObject,
    wdTime delay, wdObjectMsgQueueType::Enum queueType = wdObjectMsgQueueType::NextFrame)
  {
    if constexpr (WD_IS_DERIVED_FROM_STATIC(wdEventMessage, EventMessageType))
    {
      ref_msg.FillFromSenderComponent(pSenderComponent);
    }
    wdInternal::EventMessageSenderHelper::PostEventMessage(ref_msg, pSenderComponent, pSearchObject, m_CachedReceivers, delay, queueType);
  }

  WD_ALWAYS_INLINE void PostEventMessage(EventMessageType& ref_msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject,
    wdTime delay, wdObjectMsgQueueType::Enum queueType = wdObjectMsgQueueType::NextFrame) const
  {
    if constexpr (WD_IS_DERIVED_FROM_STATIC(wdEventMessage, EventMessageType))
    {
      ref_msg.FillFromSenderComponent(pSenderComponent);
    }
    wdInternal::EventMessageSenderHelper::PostEventMessage(ref_msg, pSenderComponent, pSearchObject, m_CachedReceivers, delay, queueType);
  }

  WD_ALWAYS_INLINE void Invalidate()
  {
    m_CachedReceivers.Clear();
    m_CachedReceivers.GetUserData<wdUInt32>() = 0;
  }

private:
  mutable wdSmallArray<wdComponentHandle, 1> m_CachedReceivers;
};
