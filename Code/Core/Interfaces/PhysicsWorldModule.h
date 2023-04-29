#pragma once

#include <Core/Messages/EventMessage.h>
#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/World/WorldModule.h>
#include <Foundation/Communication/Message.h>

struct wdGameObjectHandle;
struct wdSkeletonResourceDescriptor;

using wdSurfaceResourceHandle = wdTypedResourceHandle<class wdSurfaceResource>;

/// \brief Classifies the facing of an individual raycast hit
enum class wdPhysicsHitType : int8_t
{
  Undefined = -1,        ///< Returned if the respective physics binding does not provide this information
  TriangleFrontFace = 0, ///< The raycast hit the front face of a triangle
  TriangleBackFace = 1,  ///< The raycast hit the back face of a triangle
};

/// \brief Used for raycast and seep tests
struct wdPhysicsCastResult
{
  wdVec3 m_vPosition;
  wdVec3 m_vNormal;
  float m_fDistance;

  wdGameObjectHandle m_hShapeObject;                        ///< The game object to which the hit physics shape is attached.
  wdGameObjectHandle m_hActorObject;                        ///< The game object to which the parent actor of the hit physics shape is attached.
  wdSurfaceResourceHandle m_hSurface;                       ///< The type of surface that was hit (if available)
  wdUInt32 m_uiObjectFilterID = wdInvalidIndex;             ///< An ID either per object (rigid-body / ragdoll) or per shape (implementation specific) that can be used to ignore this object during raycasts and shape queries.
  wdPhysicsHitType m_hitType = wdPhysicsHitType::Undefined; ///< Classification of the triangle face, see wdPhysicsHitType

  // Physics-engine specific information, may be available or not.
  void* m_pInternalPhysicsShape = nullptr;
  void* m_pInternalPhysicsActor = nullptr;
};

struct wdPhysicsCastResultArray
{
  wdHybridArray<wdPhysicsCastResult, 16> m_Results;
};

/// \brief Used to report overlap query results
struct wdPhysicsOverlapResult
{
  WD_DECLARE_POD_TYPE();

  wdGameObjectHandle m_hShapeObject;            ///< The game object to which the hit physics shape is attached.
  wdGameObjectHandle m_hActorObject;            ///< The game object to which the parent actor of the hit physics shape is attached.
  wdUInt32 m_uiObjectFilterID = wdInvalidIndex; ///< The shape id of the hit physics shape
};

struct wdPhysicsOverlapResultArray
{
  wdHybridArray<wdPhysicsOverlapResult, 16> m_Results;
};

/// \brief Flags for selecting which types of physics shapes should be included in things like overlap queries and raycasts.
///
/// This is mainly for optimization purposes. It is up to the physics integration to support some or all of these flags.
///
/// Note: If this is modified, 'Physics.ts' also has to be updated.
WD_DECLARE_FLAGS_WITH_DEFAULT(wdUInt32, wdPhysicsShapeType, 0xFFFFFFFF,
  Static,    ///< Static geometry
  Dynamic,   ///< Dynamic and kinematic objects
  Query,     ///< Query shapes are kinematic bodies that don't participate in the simulation and are only used for raycasts and other queries.
  Trigger,   ///< Trigger shapes
  Character, ///< Shapes associated with character controllers.
  Ragdoll,   ///< All shapes belonging to ragdolls.
  Rope       ///< All shapes belonging to ropes.
);

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdPhysicsShapeType);

struct wdPhysicsQueryParameters
{
  wdPhysicsQueryParameters() = default;
  explicit wdPhysicsQueryParameters(wdUInt32 uiCollisionLayer,
    wdBitflags<wdPhysicsShapeType> shapeTypes = wdPhysicsShapeType::Default, wdUInt32 uiIgnoreObjectFilterID = wdInvalidIndex)
    : m_uiCollisionLayer(uiCollisionLayer)
    , m_ShapeTypes(shapeTypes)
    , m_uiIgnoreObjectFilterID(uiIgnoreObjectFilterID)
  {
  }

  wdUInt32 m_uiCollisionLayer = 0;
  wdBitflags<wdPhysicsShapeType> m_ShapeTypes = wdPhysicsShapeType::Default;
  wdUInt32 m_uiIgnoreObjectFilterID = wdInvalidIndex;
  bool m_bIgnoreInitialOverlap = false;
};

enum class wdPhysicsHitCollection
{
  Closest,
  Any
};

class WD_CORE_DLL wdPhysicsWorldModuleInterface : public wdWorldModule
{
  WD_ADD_DYNAMIC_REFLECTION(wdPhysicsWorldModuleInterface, wdWorldModule);

protected:
  wdPhysicsWorldModuleInterface(wdWorld* pWorld)
    : wdWorldModule(pWorld)
  {
  }

public:
  virtual bool Raycast(wdPhysicsCastResult& out_result, const wdVec3& vStart, const wdVec3& vDir, float fDistance, const wdPhysicsQueryParameters& params, wdPhysicsHitCollection collection = wdPhysicsHitCollection::Closest) const = 0;

  virtual bool RaycastAll(wdPhysicsCastResultArray& out_results, const wdVec3& vStart, const wdVec3& vDir, float fDistance, const wdPhysicsQueryParameters& params) const = 0;

  virtual bool SweepTestSphere(wdPhysicsCastResult& out_result, float fSphereRadius, const wdVec3& vStart, const wdVec3& vDir, float fDistance, const wdPhysicsQueryParameters& params, wdPhysicsHitCollection collection = wdPhysicsHitCollection::Closest) const = 0;

  virtual bool SweepTestBox(wdPhysicsCastResult& out_result, wdVec3 vBoxExtends, const wdTransform& transform, const wdVec3& vDir, float fDistance, const wdPhysicsQueryParameters& params, wdPhysicsHitCollection collection = wdPhysicsHitCollection::Closest) const = 0;

  virtual bool SweepTestCapsule(wdPhysicsCastResult& out_result, float fCapsuleRadius, float fCapsuleHeight, const wdTransform& transform, const wdVec3& vDir, float fDistance, const wdPhysicsQueryParameters& params, wdPhysicsHitCollection collection = wdPhysicsHitCollection::Closest) const = 0;

  virtual bool OverlapTestSphere(float fSphereRadius, const wdVec3& vPosition, const wdPhysicsQueryParameters& params) const = 0;

  virtual bool OverlapTestCapsule(float fCapsuleRadius, float fCapsuleHeight, const wdTransform& transform, const wdPhysicsQueryParameters& params) const = 0;

  virtual void QueryShapesInSphere(wdPhysicsOverlapResultArray& out_results, float fSphereRadius, const wdVec3& vPosition, const wdPhysicsQueryParameters& params) const = 0;

  virtual wdVec3 GetGravity() const = 0;

  //////////////////////////////////////////////////////////////////////////
  // ABSTRACTION HELPERS
  //
  // These functions are used to be able to use certain physics functionality, without having a direct dependency on the exact implementation (Jolt / PhysX).
  // If no physics module is available, they simply do nothing.
  // Add functions on demand.

  /// \brief Adds a static actor with a box shape to pOwner.
  virtual void AddStaticCollisionBox(wdGameObject* pOwner, wdVec3 vBoxSize) {}

  struct JointConfig
  {
    wdGameObjectHandle m_hActorA;
    wdGameObjectHandle m_hActorB;
    wdTransform m_LocalFrameA = wdTransform::IdentityTransform();
    wdTransform m_LocalFrameB = wdTransform::IdentityTransform();
  };

  struct FixedJointConfig : JointConfig
  {
  };

  /// \brief Adds a fixed joint to pOwner.
  virtual void AddFixedJointComponent(wdGameObject* pOwner, const wdPhysicsWorldModuleInterface::FixedJointConfig& cfg) {}
};

/// \brief Used to apply a physical impulse on the object
struct WD_CORE_DLL wdMsgPhysicsAddImpulse : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgPhysicsAddImpulse, wdMessage);

  wdVec3 m_vGlobalPosition;
  wdVec3 m_vImpulse;
  wdUInt32 m_uiObjectFilterID = wdInvalidIndex;

  // Physics-engine specific information, may be available or not.
  void* m_pInternalPhysicsShape = nullptr;
  void* m_pInternalPhysicsActor = nullptr;
};

/// \brief Used to apply a physical force on the object
struct WD_CORE_DLL wdMsgPhysicsAddForce : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgPhysicsAddForce, wdMessage);

  wdVec3 m_vGlobalPosition;
  wdVec3 m_vForce;

  // Physics-engine specific information, may be available or not.
  void* m_pInternalPhysicsShape = nullptr;
  void* m_pInternalPhysicsActor = nullptr;
};

struct WD_CORE_DLL wdMsgPhysicsJointBroke : public wdEventMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgPhysicsJointBroke, wdEventMessage);

  wdGameObjectHandle m_hJointObject;
};

/// \brief Sent by components such as wdJoltGrabObjectComponent to indicate that the object has been grabbed or released.
struct WD_CORE_DLL wdMsgObjectGrabbed : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgObjectGrabbed, wdMessage);

  wdGameObjectHandle m_hGrabbedBy;
  bool m_bGotGrabbed = true;
};

/// \brief Send this to components such as wdJoltGrabObjectComponent to demand that m_hGrabbedObjectToRelease should no longer be grabbed.
struct WD_CORE_DLL wdMsgReleaseObjectGrab : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgReleaseObjectGrab, wdMessage);

  wdGameObjectHandle m_hGrabbedObjectToRelease;
};

//////////////////////////////////////////////////////////////////////////

#include <Foundation/Communication/Message.h>

struct WD_CORE_DLL wdSmcTriangle
{
  WD_DECLARE_POD_TYPE();

  wdUInt32 m_uiVertexIndices[3];
};

struct WD_CORE_DLL wdSmcSubMesh
{
  WD_DECLARE_POD_TYPE();

  wdUInt32 m_uiFirstTriangle = 0;
  wdUInt32 m_uiNumTriangles = 0;
  wdUInt16 m_uiSurfaceIndex = 0;
};

struct WD_CORE_DLL wdSmcDescription
{
  wdDeque<wdVec3> m_Vertices;
  wdDeque<wdSmcTriangle> m_Triangles;
  wdDeque<wdSmcSubMesh> m_SubMeshes;
  wdDeque<wdString> m_Surfaces;
};

struct WD_CORE_DLL wdMsgBuildStaticMesh : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgBuildStaticMesh, wdMessage);

  /// \brief Append data to this description to add meshes to the automatic static mesh generation
  wdSmcDescription* m_pStaticMeshDescription = nullptr;
};
