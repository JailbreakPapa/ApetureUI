#pragma once

/// \file

#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/SmallArray.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Types/TagSet.h>

#include <Core/World/ComponentManager.h>
#include <Core/World/GameObjectDesc.h>

// Avoid conflicts with windows.h
#ifdef SendMessage
#  undef SendMessage
#endif

/// \brief This class represents an object inside the world.
///
/// Game objects only consists of hierarchical data like transformation and a list of components.
/// You cannot derive from the game object class. To add functionality to an object you have to attach components to it.
/// To create an object instance call CreateObject on the world. Never store a direct pointer to an object but store an
/// wdGameObjectHandle instead.
///
/// \see wdWorld
/// \see wdComponent
/// \see wdGameObjectHandle
class WD_CORE_DLL wdGameObject final
{
private:
  enum
  {
#if WD_ENABLED(WD_PLATFORM_32BIT)
    NUM_INPLACE_COMPONENTS = 12
#else
    NUM_INPLACE_COMPONENTS = 6
#endif
  };

  friend class wdWorld;
  friend class wdInternal::WorldData;
  friend class wdMemoryUtils;

  wdGameObject();
  wdGameObject(const wdGameObject& other);
  ~wdGameObject();

  void operator=(const wdGameObject& other);

public:
  /// \brief Iterates over all children of one object.
  class WD_CORE_DLL ConstChildIterator
  {
  public:
    const wdGameObject& operator*() const;
    const wdGameObject* operator->() const;

    operator const wdGameObject*() const;

    /// \brief Advances the iterator to the next child object. The iterator will not be valid anymore, if the last child is reached.
    void Next();

    /// \brief Checks whether this iterator points to a valid object.
    bool IsValid() const;

    /// \brief Shorthand for 'Next'
    void operator++();

  private:
    friend class wdGameObject;

    ConstChildIterator(wdGameObject* pObject, const wdWorld* pWorld);

    wdGameObject* m_pObject = nullptr;
    const wdWorld* m_pWorld = nullptr;
  };

  class WD_CORE_DLL ChildIterator : public ConstChildIterator
  {
  public:
    wdGameObject& operator*();
    wdGameObject* operator->();

    operator wdGameObject*();

  private:
    friend class wdGameObject;

    ChildIterator(wdGameObject* pObject, const wdWorld* pWorld);
  };

  /// \brief Returns a handle to this object.
  wdGameObjectHandle GetHandle() const;

  /// \brief Makes this object and all its children dynamic. Dynamic objects might move during runtime.
  void MakeDynamic();

  /// \brief Makes this object static. Static objects don't move during runtime.
  void MakeStatic();

  /// \brief Returns whether this object is dynamic.
  bool IsDynamic() const;

  /// \brief Returns whether this object is static.
  bool IsStatic() const;

  /// \brief Sets the 'active flag' of the game object, which affects its final 'active state'.
  ///
  /// The active flag affects the 'active state' of the game object and all its children and attached components.
  /// When a game object does not have the active flag, it is switched to 'inactive'. The same happens for all its children and
  /// all components attached to those game objects.
  /// Thus removing the active flag from a game object recursively deactivates the entire sub-tree of objects and components.
  ///
  /// When the active flag is set on a game object, and all of its parent nodes have the flag set as well, then the active state
  /// will be set to true on it and all its children and attached components.
  ///
  /// \sa IsActive(), wdComponent::SetActiveFlag()
  void SetActiveFlag(bool bEnabled);

  /// \brief Checks whether the 'active flag' is set on this game object. Note that this does not mean that the game object is also in an 'active
  /// state'.
  ///
  /// \sa IsActive(), SetActiveFlag()
  bool GetActiveFlag() const;

  /// \brief Checks whether this game object is in an active state.
  ///
  /// The active state is determined by the active state of the parent game object and the 'active flag' of this game object.
  /// Only if the parent game object is active (and thus all of its parent objects as well) and this game object has the active flag set,
  /// will this game object be active.
  ///
  /// \sa wdGameObject::SetActiveFlag(), wdComponent::IsActive()
  bool IsActive() const;

  /// \brief Sets the name to identify this object. Does not have to be a unique name.
  void SetName(wdStringView sName);
  void SetName(const wdHashedString& sName);
  wdStringView GetName() const;
  bool HasName(const wdTempHashedString& sName) const;

  /// \brief Sets the global key to identify this object. Global keys must be unique within a world.
  void SetGlobalKey(wdStringView sGlobalKey);
  void SetGlobalKey(const wdHashedString& sGlobalKey);
  wdStringView GetGlobalKey() const;

  /// \brief Enables or disabled notification message 'wdMsgChildrenChanged' when children are added or removed. The message is sent to this object and all its parent objects.
  void EnableChildChangesNotifications();
  void DisableChildChangesNotifications();

  /// \brief Enables or disabled notification message 'wdMsgParentChanged' when the parent changes. The message is sent to this object only.
  void EnableParentChangesNotifications();
  void DisableParentChangesNotifications();

  /// \brief Defines during re-parenting what transform is going to be preserved.
  enum class TransformPreservation
  {
    PreserveLocal,
    PreserveGlobal
  };

  /// \brief Sets the parent of this object to the given.
  void SetParent(const wdGameObjectHandle& hParent, wdGameObject::TransformPreservation preserve = TransformPreservation::PreserveGlobal);

  /// \brief Gets the parent of this object or nullptr if this is a top-level object.
  wdGameObject* GetParent();

  /// \brief Gets the parent of this object or nullptr if this is a top-level object.
  const wdGameObject* GetParent() const;

  /// \brief Adds the given object as a child object.
  void AddChild(const wdGameObjectHandle& hChild, wdGameObject::TransformPreservation preserve = TransformPreservation::PreserveGlobal);

  /// \brief Adds the given objects as child objects.
  void AddChildren(const wdArrayPtr<const wdGameObjectHandle>& children, wdGameObject::TransformPreservation preserve = TransformPreservation::PreserveGlobal);

  /// \brief Detaches the given child object from this object and makes it a top-level object.
  void DetachChild(const wdGameObjectHandle& hChild, wdGameObject::TransformPreservation preserve = TransformPreservation::PreserveGlobal);

  /// \brief Detaches the given child objects from this object and makes them top-level objects.
  void DetachChildren(const wdArrayPtr<const wdGameObjectHandle>& children, wdGameObject::TransformPreservation preserve = TransformPreservation::PreserveGlobal);

  /// \brief Returns the number of children.
  wdUInt32 GetChildCount() const;

  /// \brief Returns an iterator over all children of this object.
  ChildIterator GetChildren();

  /// \brief Returns an iterator over all children of this object.
  ConstChildIterator GetChildren() const;

  /// \brief Searches for a child object with the given name. Optionally traverses the entire hierarchy.
  wdGameObject* FindChildByName(const wdTempHashedString& sName, bool bRecursive = true);

  /// \brief Searches for a child using a path. Every path segment represents a child with a given name.
  ///
  /// Paths are separated with single slashes: /
  /// When an empty path is given, 'this' is returned.
  /// When on any part of the path the next child cannot be found, nullptr is returned.
  /// This function expects an exact path to the destination. It does not search the full hierarchy for
  /// the next child, as SearchChildByNameSequence() does.
  wdGameObject* FindChildByPath(wdStringView sPath);

  /// \brief Searches for a child similar to FindChildByName() but allows to search for multiple names in a sequence.
  ///
  /// The names in the sequence are separated with slashes.
  /// For example, calling this with "a/b" will first search the entire hierarchy below this object for a child
  /// named "a". If that is found, the search continues from there for a child called "b".
  /// If such a child is found and pExpectedComponent != nullptr, it is verified that the object
  /// contains a component of that type. If it doesn't the search continues (including back-tracking).
  wdGameObject* SearchForChildByNameSequence(wdStringView sObjectSequence, const wdRTTI* pExpectedComponent = nullptr);

  /// \brief Same as SearchForChildByNameSequence but returns ALL matches, in case the given path could mean multiple objects
  void SearchForChildrenByNameSequence(wdStringView sObjectSequence, const wdRTTI* pExpectedComponent, wdHybridArray<wdGameObject*, 8>& out_objects);

  wdWorld* GetWorld();
  const wdWorld* GetWorld() const;


  /// \brief Defines update behavior for global transforms when changing the local transform on a static game object
  enum class UpdateBehaviorIfStatic
  {
    None,              ///< Only sets the local transform, does not update
    UpdateImmediately, ///< Updates the hierarchy underneath the object immediately
  };

  /// \brief Changes the position of the object local to its parent.
  /// \note The rotation of the object itself does not affect the final global position!
  /// The local position is always in the space of the parent object. If there is no parent, local position and global position are
  /// identical.
  void SetLocalPosition(wdVec3 vPosition);
  wdVec3 GetLocalPosition() const;

  void SetLocalRotation(wdQuat qRotation);
  wdQuat GetLocalRotation() const;

  void SetLocalScaling(wdVec3 vScaling);
  wdVec3 GetLocalScaling() const;

  void SetLocalUniformScaling(float fScaling);
  float GetLocalUniformScaling() const;

  wdTransform GetLocalTransform() const;

  void SetGlobalPosition(const wdVec3& vPosition);
  wdVec3 GetGlobalPosition() const;

  void SetGlobalRotation(const wdQuat qRotation);
  wdQuat GetGlobalRotation() const;

  void SetGlobalScaling(const wdVec3 vScaling);
  wdVec3 GetGlobalScaling() const;

  void SetGlobalTransform(const wdTransform& transform);
  wdTransform GetGlobalTransform() const;

  // Simd variants of above methods
  void SetLocalPosition(const wdSimdVec4f& vPosition, UpdateBehaviorIfStatic updateBehavior = UpdateBehaviorIfStatic::UpdateImmediately);
  const wdSimdVec4f& GetLocalPositionSimd() const;

  void SetLocalRotation(const wdSimdQuat& qRotation, UpdateBehaviorIfStatic updateBehavior = UpdateBehaviorIfStatic::UpdateImmediately);
  const wdSimdQuat& GetLocalRotationSimd() const;

  void SetLocalScaling(const wdSimdVec4f& vScaling, UpdateBehaviorIfStatic updateBehavior = UpdateBehaviorIfStatic::UpdateImmediately);
  const wdSimdVec4f& GetLocalScalingSimd() const;

  void SetLocalUniformScaling(const wdSimdFloat& fScaling, UpdateBehaviorIfStatic updateBehavior = UpdateBehaviorIfStatic::UpdateImmediately);
  wdSimdFloat GetLocalUniformScalingSimd() const;

  wdSimdTransform GetLocalTransformSimd() const;

  void SetGlobalPosition(const wdSimdVec4f& vPosition);
  const wdSimdVec4f& GetGlobalPositionSimd() const;

  void SetGlobalRotation(const wdSimdQuat& qRotation);
  const wdSimdQuat& GetGlobalRotationSimd() const;

  void SetGlobalScaling(const wdSimdVec4f& vScaling);
  const wdSimdVec4f& GetGlobalScalingSimd() const;

  void SetGlobalTransform(const wdSimdTransform& transform);
  const wdSimdTransform& GetGlobalTransformSimd() const;

  /// \brief Returns the 'forwards' direction of the world's wdCoordinateSystem, rotated into the object's global space
  wdVec3 GetGlobalDirForwards() const;
  /// \brief Returns the 'right' direction of the world's wdCoordinateSystem, rotated into the object's global space
  wdVec3 GetGlobalDirRight() const;
  /// \brief Returns the 'up' direction of the world's wdCoordinateSystem, rotated into the object's global space
  wdVec3 GetGlobalDirUp() const;

#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
  /// \brief Sets the object's velocity.
  ///
  /// This is used for some rendering techniques or for the computation of sound Doppler effect.
  /// It has no effect on the object's subsequent position.
  void SetVelocity(const wdVec3& vVelocity);

  /// \brief Returns the velocity of the object in units per second. This is not only the diff between last frame's position and this
  /// frame's position, but
  ///        also the time difference is divided out.
  wdVec3 GetVelocity() const;
#endif

  /// \brief Updates the global transform immediately. Usually this done during the world update after the "Post-async" phase.
  void UpdateGlobalTransform();

  /// \brief Enables or disabled notification message 'wdMsgTransformChanged' when this object is static and its transform changes.
  /// The notification message is sent to this object and thus also to all its components.
  void EnableStaticTransformChangesNotifications();
  void DisableStaticTransformChangesNotifications();


  wdBoundingBoxSphere GetLocalBounds() const;
  wdBoundingBoxSphere GetGlobalBounds() const;

  const wdSimdBBoxSphere& GetLocalBoundsSimd() const;
  const wdSimdBBoxSphere& GetGlobalBoundsSimd() const;

  /// \brief Invalidates the local bounds and sends a message to all components so they can add their bounds.
  void UpdateLocalBounds();

  /// \brief Updates the global bounds immediately. Usually this done during the world update after the "Post-async" phase.
  /// Note that this function does not ensure that the global transform is up-to-date. Use UpdateGlobalTransformAndBounds if you want to update both.
  void UpdateGlobalBounds();

  /// \brief Updates the global transform and bounds immediately. Usually this done during the world update after the "Post-async" phase.
  void UpdateGlobalTransformAndBounds();


  /// \brief Returns a handle to the internal spatial data.
  wdSpatialDataHandle GetSpatialData() const;

  /// \brief Enables or disabled notification message 'wdMsgComponentsChanged' when components are added or removed. The message is sent to this object and all its parent objects.
  void EnableComponentChangesNotifications();
  void DisableComponentChangesNotifications();

  /// \brief Tries to find a component of the given base type in the objects components list and returns the first match.
  template <typename T>
  bool TryGetComponentOfBaseType(T*& out_pComponent);

  /// \brief Tries to find a component of the given base type in the objects components list and returns the first match.
  template <typename T>
  bool TryGetComponentOfBaseType(const T*& out_pComponent) const;

  /// \brief Tries to find a component of the given base type in the objects components list and returns the first match.
  bool TryGetComponentOfBaseType(const wdRTTI* pType, wdComponent*& out_pComponent);

  /// \brief Tries to find a component of the given base type in the objects components list and returns the first match.
  bool TryGetComponentOfBaseType(const wdRTTI* pType, const wdComponent*& out_pComponent) const;

  /// \brief Tries to find components of the given base type in the objects components list and returns all matches.
  template <typename T>
  void TryGetComponentsOfBaseType(wdDynamicArray<T*>& out_components);

  /// \brief Tries to find components of the given base type in the objects components list and returns all matches.
  template <typename T>
  void TryGetComponentsOfBaseType(wdDynamicArray<const T*>& out_components) const;

  /// \brief Tries to find components of the given base type in the objects components list and returns all matches.
  void TryGetComponentsOfBaseType(const wdRTTI* pType, wdDynamicArray<wdComponent*>& out_components);

  /// \brief Tries to find components of the given base type in the objects components list and returns all matches.
  void TryGetComponentsOfBaseType(const wdRTTI* pType, wdDynamicArray<const wdComponent*>& out_components) const;

  /// \brief Returns a list of all components attached to this object.
  wdArrayPtr<wdComponent* const> GetComponents();

  /// \brief Returns a list of all components attached to this object.
  wdArrayPtr<const wdComponent* const> GetComponents() const;

  /// \brief Returns the current version of components attached to this object.
  /// This version is increased whenever components are added or removed and can be used for cache validation.
  wdUInt16 GetComponentVersion() const;


  /// \brief Sends a message to all components of this object.
  bool SendMessage(wdMessage& ref_msg);

  /// \brief Sends a message to all components of this object.
  bool SendMessage(wdMessage& ref_msg) const;

  /// \brief Sends a message to all components of this object and then recursively to all children.
  bool SendMessageRecursive(wdMessage& ref_msg);

  /// \brief Sends a message to all components of this object and then recursively to all children.
  bool SendMessageRecursive(wdMessage& ref_msg) const;


  /// \brief Queues the message for the given phase. The message is processed after the given delay in the corresponding phase.
  void PostMessage(const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType = wdObjectMsgQueueType::NextFrame) const;

  /// \brief Queues the message for the given phase. The message is processed after the given delay in the corresponding phase.
  void PostMessageRecursive(const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType = wdObjectMsgQueueType::NextFrame) const;

  /// \brief Delivers an wdEventMessage to the closest (parent) object containing an wdEventMessageHandlerComponent.
  ///
  /// Regular SendMessage() and PostMessage() send a message directly to the target object (and all attached components).
  /// SendMessageRecursive() and PostMessageRecursive() send a message 'down' the graph to the target object and all children.
  ///
  /// In contrast, SendEventMessage() / PostEventMessage() bubble the message 'up' the graph.
  /// They do so by inspecting the chain of parent objects for the existence of an wdEventMessageHandlerComponent
  /// (typically a script component). If such a component is found, the message is delivered to it directly, and no other component.
  /// If it is found, but does not handle this type of message, the message is discarded and NOT tried to be delivered
  /// to anyone else.
  ///
  /// If no such component is found in all parent objects, the message is delivered to one wdEventMessageHandlerComponent
  /// instances that is set to 'handle global events' (typically used for level-logic scripts), no matter where in the graph it resides.
  /// If multiple global event handler component exist that handle the same message type, the result is non-deterministic.
  ///
  /// \param msg The message to deliver.
  /// \param senderComponent The component that triggered the event in the first place. May be nullptr.
  ///        If not null, this information is stored in \a msg as wdEventMessage::m_hSenderObject and wdEventMessage::m_hSenderComponent.
  ///        This information is used to pass through more contextual information for the event handler.
  ///        For instance, a trigger would pass through which object entered the trigger.
  ///        A projectile component sending a 'take damage event' to the hit object, would pass through itself (the projectile)
  ///        such that the handling code can detect which object was responsible for the damage (and using the wdGameObject's team-ID,
  ///        it can detect which player fired the projectile).
  void SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent);

  /// \copydoc wdGameObject::SendEventMessage()
  void SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent) const;

  /// \copydoc wdGameObject::SendEventMessage()
  ///
  /// \param queueType In which update phase to deliver the message.
  /// \param delay An optional delay before delivering the message.
  void PostEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent, wdTime delay, wdObjectMsgQueueType::Enum queueType = wdObjectMsgQueueType::NextFrame) const;


  /// \brief Returns the tag set associated with this object.
  const wdTagSet& GetTags() const;

  /// \brief Sets the tag set associated with this object.
  void SetTags(const wdTagSet& tags);

  /// \brief Adds the given tag to the object's tags.
  void SetTag(const wdTag& tag);

  /// \brief Removes the given tag from the object's tags.
  void RemoveTag(const wdTag& tag);

  /// \brief Returns the 'team ID' that was given during creation (/see wdGameObjectDesc)
  ///
  /// It is automatically passed on to objects created by this object.
  /// This makes it possible to identify which player or team an object belongs to.
  const wdUInt16& GetTeamID() const { return m_uiTeamID; }

  /// \brief Changes the team ID for this object and all children recursively.
  void SetTeamID(wdUInt16 uiId);

  /// \brief Returns a random value that is chosen once during object creation and remains stable even throughout serialization.
  ///
  /// This value is intended to be used for choosing random variations of components. For instance, if a component has two
  /// different meshes it can use for variation, this seed should be used to decide which one to use.
  ///
  /// The stable random seed can also be set from the outside, which is what the editor does, to assign a truly stable seed value.
  /// Therefore, each object placed in the editor will always have the same seed value, and objects won't change their appearance
  /// on every run of the game.
  ///
  /// The stable seed is also propagated through prefab instances, such that every prefab instance gets a different value, but
  /// in a deterministic fashion.
  wdUInt32 GetStableRandomSeed() const;

  /// \brief Overwrites the object's random seed value.
  ///
  /// See \a GetStableRandomSeed() for details.
  ///
  /// It should not be necessary to manually change this value, unless you want to make the seed deterministic according to a custom rule.
  void SetStableRandomSeed(wdUInt32 uiSeed);

  /// \brief Returns the number of frames since this object was last visible in any view.
  ///
  /// This value can be used to skip update logic of invisible objects.
  wdUInt64 GetNumFramesSinceVisible() const;

private:
  friend class wdComponentManagerBase;
  friend class wdGameObjectTest;

  // only needed until reflection can deal with wdStringView
  void SetNameInternal(const char* szName);
  const char* GetNameInternal() const;
  void SetGlobalKeyInternal(const char* szKey);
  const char* GetGlobalKeyInternal() const;

  bool SendMessageInternal(wdMessage& msg, bool bWasPostedMsg);
  bool SendMessageInternal(wdMessage& msg, bool bWasPostedMsg) const;
  bool SendMessageRecursiveInternal(wdMessage& msg, bool bWasPostedMsg);
  bool SendMessageRecursiveInternal(wdMessage& msg, bool bWasPostedMsg) const;

  WD_ALLOW_PRIVATE_PROPERTIES(wdGameObject);

  // Add / Detach child used by the reflected property keep their local transform as
  // updating that is handled by the editor.
  void Reflection_AddChild(wdGameObject* pChild);
  void Reflection_DetachChild(wdGameObject* pChild);
  wdHybridArray<wdGameObject*, 8> Reflection_GetChildren() const;
  void Reflection_AddComponent(wdComponent* pComponent);
  void Reflection_RemoveComponent(wdComponent* pComponent);
  wdHybridArray<wdComponent*, NUM_INPLACE_COMPONENTS> Reflection_GetComponents() const;

  wdObjectMode::Enum Reflection_GetMode() const;
  void Reflection_SetMode(wdObjectMode::Enum mode);

  bool DetermineDynamicMode(wdComponent* pComponentToIgnore = nullptr) const;
  void ConditionalMakeStatic(wdComponent* pComponentToIgnore = nullptr);
  void MakeStaticInternal();

  void UpdateGlobalTransformAndBoundsRecursive();

  void OnMsgDeleteGameObject(wdMsgDeleteGameObject& msg);

  void AddComponent(wdComponent* pComponent);
  void RemoveComponent(wdComponent* pComponent);
  void FixComponentPointer(wdComponent* pOldPtr, wdComponent* pNewPtr);

  // Updates the active state of this object and all children and attached components recursively, depending on the enabled states.
  void UpdateActiveState(bool bParentActive);

  void SendNotificationMessage(wdMessage& msg);

  struct WD_CORE_DLL alignas(16) TransformationData
  {
    WD_DECLARE_POD_TYPE();

    wdGameObject* m_pObject;
    TransformationData* m_pParentData;

#if WD_ENABLED(WD_PLATFORM_32BIT)
    wdUInt64 m_uiPadding;
#endif

    wdSimdVec4f m_localPosition;
    wdSimdQuat m_localRotation;
    wdSimdVec4f m_localScaling; // x,y,z = non-uniform scaling, w = uniform scaling

    wdSimdTransform m_globalTransform;

#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
    wdSimdVec4f m_lastGlobalPosition;
    wdSimdVec4f m_velocity; // w != 0 indicates custom velocity
#endif

    wdSimdBBoxSphere m_localBounds; // m_BoxHalfExtents.w != 0 indicates that the object should be always visible
    wdSimdBBoxSphere m_globalBounds;

    wdSpatialDataHandle m_hSpatialData;
    wdUInt32 m_uiSpatialDataCategoryBitmask;

    wdUInt32 m_uiStableRandomSeed = 0;

    wdUInt32 m_uiPadding2[1];

    /// \brief Recomputes the local transform from this object's global transform and, if available, the parent's global transform.
    void UpdateLocalTransform();

    /// \brief Calls UpdateGlobalTransformWithoutParent or UpdateGlobalTransformWithParent depending on whether there is a parent transform.
    /// In case there is a parent transform it also recursively calls itself on the parent transform to ensure everything is up-to-date.
    void UpdateGlobalTransformRecursive();

    /// \brief Calls UpdateGlobalTransformWithoutParent or UpdateGlobalTransformWithParent depending on whether there is a parent transform.
    /// Assumes that the parent's global transform is already up to date.
    void UpdateGlobalTransformNonRecursive();

    /// \brief Updates the global transform by copying the object's local transform into the global transform.
    /// This is for objects that have no parent.
    void UpdateGlobalTransformWithoutParent();

    /// \brief Updates the global transform by combining the parents global transform with this object's local transform.
    /// Assumes that the parent's global transform is already up to date.
    void UpdateGlobalTransformWithParent();

    void UpdateGlobalBounds(wdSpatialSystem* pSpatialSystem);
    void UpdateGlobalBounds();
    void UpdateGlobalBoundsAndSpatialData(wdSpatialSystem& ref_spatialSystem);

    void UpdateVelocity(const wdSimdFloat& fInvDeltaSeconds);

    void RecreateSpatialData(wdSpatialSystem& ref_spatialSystem);
  };

  wdGameObjectId m_InternalId;
  wdHashedString m_sName;

#if WD_ENABLED(WD_PLATFORM_32BIT)
  wdUInt32 m_uiNamePadding;
#endif

  wdBitflags<wdObjectFlags> m_Flags;

  wdUInt32 m_uiParentIndex = 0;
  wdUInt32 m_uiFirstChildIndex = 0;
  wdUInt32 m_uiLastChildIndex = 0;

  wdUInt32 m_uiNextSiblingIndex = 0;
  wdUInt32 m_uiPrevSiblingIndex = 0;
  wdUInt32 m_uiChildCount = 0;

  wdUInt16 m_uiHierarchyLevel = 0;

  /// An int that will be passed on to objects spawned from this one, which allows to identify which team or player it belongs to.
  wdUInt16 m_uiTeamID = 0;

  TransformationData* m_pTransformationData = nullptr;

#if WD_ENABLED(WD_PLATFORM_32BIT)
  wdUInt32 m_uiPadding = 0;
#endif

  wdSmallArrayBase<wdComponent*, NUM_INPLACE_COMPONENTS> m_Components;

  struct ComponentUserData
  {
    wdUInt16 m_uiVersion;
    wdUInt16 m_uiUnused;
  };

  wdTagSet m_Tags;
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdGameObject);

#include <Core/World/Implementation/GameObject_inl.h>
