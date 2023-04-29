#include <Core/CorePCH.h>

#include <Core/Messages/DeleteObjectMessage.h>
#include <Core/World/World.h>
#include <Foundation/Types/VariantTypeRegistry.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdGameObjectHandle, wdNoBase, 1, wdRTTIDefaultAllocator<wdGameObjectHandle>)
WD_END_STATIC_REFLECTED_TYPE;
WD_DEFINE_CUSTOM_VARIANT_TYPE(wdGameObjectHandle);

WD_BEGIN_STATIC_REFLECTED_TYPE(wdComponentHandle, wdNoBase, 1, wdRTTIDefaultAllocator<wdComponentHandle>)
WD_END_STATIC_REFLECTED_TYPE;
WD_DEFINE_CUSTOM_VARIANT_TYPE(wdComponentHandle);

WD_BEGIN_STATIC_REFLECTED_ENUM(wdObjectMode, 1)
  WD_ENUM_CONSTANTS(wdObjectMode::Automatic, wdObjectMode::ForceDynamic)
WD_END_STATIC_REFLECTED_ENUM;

WD_BEGIN_STATIC_REFLECTED_ENUM(wdOnComponentFinishedAction, 1)
  WD_ENUM_CONSTANTS(wdOnComponentFinishedAction::None, wdOnComponentFinishedAction::DeleteComponent, wdOnComponentFinishedAction::DeleteGameObject)
WD_END_STATIC_REFLECTED_ENUM;

WD_BEGIN_STATIC_REFLECTED_ENUM(wdOnComponentFinishedAction2, 1)
  WD_ENUM_CONSTANTS(wdOnComponentFinishedAction2::None, wdOnComponentFinishedAction2::DeleteComponent, wdOnComponentFinishedAction2::DeleteGameObject, wdOnComponentFinishedAction2::Restart)
WD_END_STATIC_REFLECTED_ENUM;
// clang-format on

//////////////////////////////////////////////////////////////////////////

void operator<<(wdStreamWriter& inout_stream, const wdGameObjectHandle& hValue)
{
  WD_ASSERT_DEV(false, "This function should not be called. Use wdWorldWriter::WriteGameObjectHandle instead.");
}

void operator>>(wdStreamReader& inout_stream, wdGameObjectHandle& ref_hValue)
{
  WD_ASSERT_DEV(false, "This function should not be called. Use wdWorldReader::ReadGameObjectHandle instead.");
}

void operator<<(wdStreamWriter& inout_stream, const wdComponentHandle& hValue)
{
  WD_ASSERT_DEV(false, "This function should not be called. Use wdWorldWriter::WriteComponentHandle instead.");
}

void operator>>(wdStreamReader& inout_stream, wdComponentHandle& ref_hValue)
{
  WD_ASSERT_DEV(false, "This function should not be called. Use wdWorldReader::ReadComponentHandle instead.");
}

//////////////////////////////////////////////////////////////////////////

namespace
{
  template <typename T>
  void HandleFinishedActionImpl(wdComponent* pComponent, typename T::Enum action)
  {
    if (action == T::DeleteGameObject)
    {
      // Send a message to the owner object to check whether another component wants to delete this object later.
      // Can't use wdGameObject::SendMessage because the object would immediately delete itself and furthermore the sender component needs to be
      // filtered out here.
      wdMsgDeleteGameObject msg;

      for (wdComponent* pComp : pComponent->GetOwner()->GetComponents())
      {
        if (pComp == pComponent)
          continue;

        pComp->SendMessage(msg);
        if (msg.m_bCancel)
        {
          action = T::DeleteComponent;
          break;
        }
      }

      if (action == T::DeleteGameObject)
      {
        pComponent->GetWorld()->DeleteObjectDelayed(pComponent->GetOwner()->GetHandle());
        return;
      }
    }

    if (action == T::DeleteComponent)
    {
      pComponent->GetOwningManager()->DeleteComponent(pComponent->GetHandle());
    }
  }

  template <typename T>
  void HandleDeleteObjectMsgImpl(wdMsgDeleteGameObject& ref_msg, wdEnum<T>& ref_action)
  {
    if (ref_action == T::DeleteComponent)
    {
      ref_msg.m_bCancel = true;
      ref_action = T::DeleteGameObject;
    }
    else if (ref_action == T::DeleteGameObject)
    {
      ref_msg.m_bCancel = true;
    }
  }
} // namespace

//////////////////////////////////////////////////////////////////////////

void wdOnComponentFinishedAction::HandleFinishedAction(wdComponent* pComponent, wdOnComponentFinishedAction::Enum action)
{
  HandleFinishedActionImpl<wdOnComponentFinishedAction>(pComponent, action);
}

void wdOnComponentFinishedAction::HandleDeleteObjectMsg(wdMsgDeleteGameObject& ref_msg, wdEnum<wdOnComponentFinishedAction>& ref_action)
{
  HandleDeleteObjectMsgImpl(ref_msg, ref_action);
}

//////////////////////////////////////////////////////////////////////////

void wdOnComponentFinishedAction2::HandleFinishedAction(wdComponent* pComponent, wdOnComponentFinishedAction2::Enum action)
{
  HandleFinishedActionImpl<wdOnComponentFinishedAction2>(pComponent, action);
}

void wdOnComponentFinishedAction2::HandleDeleteObjectMsg(wdMsgDeleteGameObject& ref_msg, wdEnum<wdOnComponentFinishedAction2>& ref_action)
{
  HandleDeleteObjectMsgImpl(ref_msg, ref_action);
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_Declarations);
