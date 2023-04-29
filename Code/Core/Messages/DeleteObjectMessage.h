#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdMsgDeleteGameObject : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgDeleteGameObject, wdMessage);

  /// \brief If set to true, any parent/ancestor that has no other children or components will also be deleted.
  bool m_bDeleteEmptyParents = true;

  /// \brief This is used by wdOnComponentFinishedAction to orchestrate when an object shall really be deleted.
  bool m_bCancel = false;
};
