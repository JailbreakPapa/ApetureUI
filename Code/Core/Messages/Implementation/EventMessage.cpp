#include <Core/CorePCH.h>

#include <Core/Messages/EventMessage.h>
#include <Core/World/EventMessageHandlerComponent.h>
#include <Core/World/World.h>

// clang-format off
WD_IMPLEMENT_MESSAGE_TYPE(wdEventMessage);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdEventMessage, 1, wdRTTIDefaultAllocator<wdEventMessage>)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

WD_CHECK_AT_COMPILETIME(sizeof(wdEventMessageSender<wdEventMessage>) == 16);

namespace wdInternal
{
  template <typename World, typename GameObject>
  static void UpdateCachedReceivers(const wdMessage& msg, World& ref_world, GameObject pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers)
  {
    if (inout_cachedReceivers.GetUserData<wdUInt32>() == 0)
    {
      using ComponentType = typename std::conditional<std::is_const<World>::value, const wdComponent*, wdComponent*>::type;

      wdHybridArray<ComponentType, 4> eventMsgHandlers;
      ref_world.FindEventMsgHandlers(msg, pSearchObject, eventMsgHandlers);

      for (auto pEventMsgHandler : eventMsgHandlers)
      {
        inout_cachedReceivers.PushBack(pEventMsgHandler->GetHandle());
      }

      inout_cachedReceivers.GetUserData<wdUInt32>() = 1;
    }
  }

  void EventMessageSenderHelper::SendEventMessage(wdMessage& ref_msg, wdComponent* pSenderComponent, wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers)
  {
    wdWorld* pWorld = pSenderComponent->GetWorld();
    UpdateCachedReceivers(ref_msg, *pWorld, pSearchObject, inout_cachedReceivers);

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    bool bHandlerFound = false;
#endif

    for (auto hReceiver : inout_cachedReceivers)
    {
      wdComponent* pReceiverComponent = nullptr;
      if (pWorld->TryGetComponent(hReceiver, pReceiverComponent))
      {
        pReceiverComponent->SendMessage(ref_msg);
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
        bHandlerFound = true;
#endif
      }
    }

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (!bHandlerFound && ref_msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdEventMessageSender::SendMessage: No event message handler found for message of type {0}.", ref_msg.GetId());
    }
#endif
  }

  void EventMessageSenderHelper::SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers)
  {
    const wdWorld* pWorld = pSenderComponent->GetWorld();
    UpdateCachedReceivers(ref_msg, *pWorld, pSearchObject, inout_cachedReceivers);

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    bool bHandlerFound = false;
#endif

    for (auto hReceiver : inout_cachedReceivers)
    {
      const wdComponent* pReceiverComponent = nullptr;
      if (pWorld->TryGetComponent(hReceiver, pReceiverComponent))
      {
        pReceiverComponent->SendMessage(ref_msg);
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
        bHandlerFound = true;
#endif
      }
    }

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (!bHandlerFound && ref_msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdEventMessageSender::SendMessage: No event message handler found for message of type {0}.", ref_msg.GetId());
    }
#endif
  }

  void EventMessageSenderHelper::PostEventMessage(const wdMessage& msg, const wdComponent* pSenderComponent, const wdGameObject* pSearchObject, wdSmallArray<wdComponentHandle, 1>& inout_cachedReceivers, wdTime delay, wdObjectMsgQueueType::Enum queueType)
  {
    const wdWorld* pWorld = pSenderComponent->GetWorld();
    UpdateCachedReceivers(msg, *pWorld, pSearchObject, inout_cachedReceivers);

    if (!inout_cachedReceivers.IsEmpty())
    {
      for (auto hReceiver : inout_cachedReceivers)
      {
        pWorld->PostMessage(hReceiver, msg, delay, queueType);
      }
    }
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    else if (msg.GetDebugMessageRouting())
    {
      wdLog::Warning("wdEventMessageSender::PostMessage: No event message handler found for message of type {0}.", msg.GetId());
    }
#endif
  }

} // namespace wdInternal

WD_STATICLINK_FILE(Core, Core_Messages_Implementation_EventMessage);
