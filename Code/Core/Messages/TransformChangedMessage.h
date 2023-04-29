#pragma once

#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdMsgTransformChanged : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgTransformChanged, wdMessage);

  wdTransform m_OldGlobalTransform;
  wdTransform m_NewGlobalTransform;
};
