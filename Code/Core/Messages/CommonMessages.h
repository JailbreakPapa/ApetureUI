#pragma once

#include <Core/Messages/EventMessage.h>
#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

/// \brief Common message for components that can be toggled between playing and paused states
struct WD_CORE_DLL wdMsgSetPlaying : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgSetPlaying, wdMessage);

  bool m_bPlay = true;
};

/// \brief Basic message to set some generic parameter to a float value.
struct WD_CORE_DLL wdMsgSetFloatParameter : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgSetFloatParameter, wdMessage);

  wdString m_sParameterName;
  float m_fValue = 0;
};

/// \brief For use in scripts to signal a custom event that some game event has occurred.
///
/// This is a simple message for simple use cases. Create custom messages for more elaborate cases where a string is not sufficient
/// information.
struct WD_CORE_DLL wdMsgGenericEvent : public wdEventMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgGenericEvent, wdEventMessage);

  /// A custom string to identify the intent.
  wdHashedString m_sMessage;
  wdVariant m_Value;

private:
  const char* GetMessage() const { return m_sMessage; }
  void SetMessage(const char* szMessage) { m_sMessage.Assign(szMessage); }
};

/// \brief Sent when an animation reached its end (either forwards or backwards playing)
///
/// This is sent regardless of whether the animation is played once, looped or back and forth,
/// ie. it should be sent at each 'end' point, even when it then starts another cycle.
struct WD_CORE_DLL wdMsgAnimationReachedEnd : public wdEventMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgAnimationReachedEnd, wdEventMessage);
};
