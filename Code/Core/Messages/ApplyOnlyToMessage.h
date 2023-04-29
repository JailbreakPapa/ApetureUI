#pragma once

#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdMsgOnlyApplyToObject : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgOnlyApplyToObject, wdMessage);

  wdGameObjectHandle m_hObject;
};
