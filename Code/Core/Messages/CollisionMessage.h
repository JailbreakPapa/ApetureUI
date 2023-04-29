#pragma once

#include <Core/World/Declarations.h>
#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdMsgCollision : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgCollision, wdMessage);

  wdGameObjectHandle m_hObjectA;
  wdGameObjectHandle m_hObjectB;

  wdComponentHandle m_hComponentA;
  wdComponentHandle m_hComponentB;

  wdVec3 m_vPosition; ///< The collision position in world space.
  wdVec3 m_vNormal;   ///< The collision normal on the surface of object B.
  wdVec3 m_vImpulse;  ///< The collision impulse applied from object A to object B.
};
