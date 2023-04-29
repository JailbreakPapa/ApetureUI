#pragma once

#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdMsgParentChanged : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgParentChanged, wdMessage);

  enum class Type
  {
    ParentLinked,
    ParentUnlinked,
  };

  Type m_Type;
  wdGameObjectHandle m_hParent; // previous or new parent, depending on m_Type
};

struct WD_CORE_DLL wdMsgChildrenChanged : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgChildrenChanged, wdMessage);

  enum class Type
  {
    ChildAdded,
    ChildRemoved
  };

  Type m_Type;
  wdGameObjectHandle m_hParent;
  wdGameObjectHandle m_hChild;
};

struct WD_CORE_DLL wdMsgComponentsChanged : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgComponentsChanged, wdMessage);

  enum class Type
  {
    ComponentAdded,
    ComponentRemoved
  };

  Type m_Type;
  wdGameObjectHandle m_hOwner;
  wdComponentHandle m_hComponent;
};
