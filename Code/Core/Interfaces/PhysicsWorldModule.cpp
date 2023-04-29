#include <Core/CorePCH.h>

#include <Core/Interfaces/PhysicsWorldModule.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdPhysicsWorldModuleInterface, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_BEGIN_STATIC_REFLECTED_BITFLAGS(wdPhysicsShapeType, 1)
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Static),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Dynamic),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Query),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Trigger),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Character),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Ragdoll),
  WD_BITFLAGS_CONSTANT(wdPhysicsShapeType::Rope),
WD_END_STATIC_REFLECTED_BITFLAGS;

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgPhysicsAddImpulse);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgPhysicsAddImpulse, 1, wdRTTIDefaultAllocator<wdMsgPhysicsAddImpulse>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("GlobalPosition", m_vGlobalPosition),
    WD_MEMBER_PROPERTY("Impulse", m_vImpulse),
    WD_MEMBER_PROPERTY("ObjectFilterID", m_uiObjectFilterID),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgPhysicsAddForce);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgPhysicsAddForce, 1, wdRTTIDefaultAllocator<wdMsgPhysicsAddForce>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("GlobalPosition", m_vGlobalPosition),
    WD_MEMBER_PROPERTY("Force", m_vForce),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgPhysicsJointBroke);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgPhysicsJointBroke, 1, wdRTTIDefaultAllocator<wdMsgPhysicsJointBroke>)
//{
  //WD_BEGIN_PROPERTIES
  //{
  //  WD_MEMBER_PROPERTY("JointObject", m_hJointObject)
  //}
  //WD_END_PROPERTIES;
//}
WD_END_DYNAMIC_REFLECTED_TYPE

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgObjectGrabbed);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgObjectGrabbed, 1, wdRTTIDefaultAllocator<wdMsgObjectGrabbed>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("GrabbedBy", m_hGrabbedBy),
    WD_MEMBER_PROPERTY("GotGrabbed", m_bGotGrabbed),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgReleaseObjectGrab);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgReleaseObjectGrab, 1, wdRTTIDefaultAllocator<wdMsgReleaseObjectGrab>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("GrabbedObjectToRelease", m_hGrabbedObjectToRelease),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgBuildStaticMesh);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgBuildStaticMesh, 1, wdRTTIDefaultAllocator<wdMsgBuildStaticMesh>)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on


WD_STATICLINK_FILE(Core, Core_Interfaces_PhysicsWorldModule);
