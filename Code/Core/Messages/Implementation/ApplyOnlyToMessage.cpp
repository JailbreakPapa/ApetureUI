#include <Core/CorePCH.h>

#include <Core/Messages/ApplyOnlyToMessage.h>

// clang-format off
WD_IMPLEMENT_MESSAGE_TYPE(wdMsgOnlyApplyToObject);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgOnlyApplyToObject, 1, wdRTTIDefaultAllocator<wdMsgOnlyApplyToObject>)
{
  ///\todo enable this once we have object reference properties
  /*WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Object", m_hObject),
  }
  WD_END_PROPERTIES;*/
  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on


WD_STATICLINK_FILE(Core, Core_Messages_Implementation_ApplyOnlyToMessage);
