#include <Core/CorePCH.h>

#include <Core/Messages/CollisionMessage.h>
#include <Core/Messages/CommonMessages.h>
#include <Core/Messages/DeleteObjectMessage.h>
#include <Core/Messages/HierarchyChangedMessages.h>
#include <Core/Messages/TransformChangedMessage.h>
#include <Core/Messages/TriggerMessage.h>
#include <Core/Messages/UpdateLocalBoundsMessage.h>

// clang-format off
WD_IMPLEMENT_MESSAGE_TYPE(wdMsgCollision);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgCollision, 1, wdRTTIDefaultAllocator<wdMsgCollision>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_BEGIN_STATIC_REFLECTED_ENUM(wdTriggerState, 1)
  WD_ENUM_CONSTANTS(wdTriggerState::Activated, wdTriggerState::Continuing, wdTriggerState::Deactivated)
WD_END_STATIC_REFLECTED_ENUM;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgDeleteGameObject);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgDeleteGameObject, 1, wdRTTIDefaultAllocator<wdMsgDeleteGameObject>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgComponentInternalTrigger);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgComponentInternalTrigger, 1, wdRTTIDefaultAllocator<wdMsgComponentInternalTrigger>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Message", GetMessage, SetMessage),
    WD_MEMBER_PROPERTY("Payload", m_iPayload),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender(),
    new wdAutoGenVisScriptMsgHandler()
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgUpdateLocalBounds);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgUpdateLocalBounds, 1, wdRTTIDefaultAllocator<wdMsgUpdateLocalBounds>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgSetPlaying);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgSetPlaying, 1, wdRTTIDefaultAllocator<wdMsgSetPlaying>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Play", m_bPlay)->AddAttributes(new wdDefaultValueAttribute(true)),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender(),
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgParentChanged);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgParentChanged, 1, wdRTTIDefaultAllocator<wdMsgParentChanged>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgChildrenChanged);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgChildrenChanged, 1, wdRTTIDefaultAllocator<wdMsgChildrenChanged>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgComponentsChanged);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgComponentsChanged, 1, wdRTTIDefaultAllocator<wdMsgComponentsChanged>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgTransformChanged);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgTransformChanged, 1, wdRTTIDefaultAllocator<wdMsgTransformChanged>)
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgSetFloatParameter);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgSetFloatParameter, 1, wdRTTIDefaultAllocator<wdMsgSetFloatParameter>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Name", m_sParameterName),
    WD_MEMBER_PROPERTY("Value", m_fValue),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender(),
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgGenericEvent);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgGenericEvent, 1, wdRTTIDefaultAllocator<wdMsgGenericEvent>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Message", GetMessage, SetMessage),
    WD_MEMBER_PROPERTY("Value", m_Value)->AddAttributes(new wdDefaultValueAttribute(0))
  }
  WD_END_PROPERTIES;

  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender(),
    new wdAutoGenVisScriptMsgHandler()
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgAnimationReachedEnd);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgAnimationReachedEnd, 1, wdRTTIDefaultAllocator<wdMsgAnimationReachedEnd>)
{
  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgHandler(),
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgTriggerTriggered)
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgTriggerTriggered, 1, wdRTTIDefaultAllocator<wdMsgTriggerTriggered>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Message", GetMessage, SetMessage),
    WD_ENUM_MEMBER_PROPERTY("TriggerState", wdTriggerState, m_TriggerState),
    //WD_MEMBER_PROPERTY("GameObject", m_hTriggeringObject),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

// clang-format on

WD_STATICLINK_FILE(Core, Core_Messages_Implementation_Messages);
