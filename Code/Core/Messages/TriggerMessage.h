#pragma once

#include <Core/Messages/EventMessage.h>
#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdTriggerState
{
  using StorageType = wdUInt8;

  enum Enum
  {
    Activated,   ///< The trigger was just activated (area entered, key pressed, etc.)
    Continuing,  ///< The trigger is active for more than one frame now.
    Deactivated, ///< The trigger was just deactivated (left area, key released, etc.)

    Default = Activated
  };
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdTriggerState);

/// \brief For internal use by components to trigger some known behavior. Usually components will post this message to themselves with a
/// delay, e.g. to trigger self destruction.
struct WD_CORE_DLL wdMsgComponentInternalTrigger : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgComponentInternalTrigger, wdMessage);

  /// Identifies what the message should trigger.
  wdHashedString m_sMessage;

  wdInt32 m_iPayload = 0;

private:
  const char* GetMessage() const { return m_sMessage; }
  void SetMessage(const char* szMessage) { m_sMessage.Assign(szMessage); }
};

/// \brief Sent when something enters or leaves a trigger
struct WD_CORE_DLL wdMsgTriggerTriggered : public wdEventMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgTriggerTriggered, wdEventMessage);

  /// Identifies what the message should trigger.
  wdHashedString m_sMessage;

  /// Messages are only sent for 'entered' ('Activated') and 'left' ('Deactivated')
  wdEnum<wdTriggerState> m_TriggerState;

  /// The object that entered the trigger volume.
  wdGameObjectHandle m_hTriggeringObject;

private:
  const char* GetMessage() const { return m_sMessage; }
  void SetMessage(const char* szMessage) { m_sMessage.Assign(szMessage); }
};
