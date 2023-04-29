
WD_ALWAYS_INLINE wdGameObject::ConstChildIterator::ConstChildIterator(wdGameObject* pObject, const wdWorld* pWorld)
  : m_pObject(pObject)
  , m_pWorld(pWorld)
{
}

WD_ALWAYS_INLINE const wdGameObject& wdGameObject::ConstChildIterator::operator*() const
{
  return *m_pObject;
}

WD_ALWAYS_INLINE const wdGameObject* wdGameObject::ConstChildIterator::operator->() const
{
  return m_pObject;
}

WD_ALWAYS_INLINE wdGameObject::ConstChildIterator::operator const wdGameObject*() const
{
  return m_pObject;
}

WD_ALWAYS_INLINE bool wdGameObject::ConstChildIterator::IsValid() const
{
  return m_pObject != nullptr;
}

WD_ALWAYS_INLINE void wdGameObject::ConstChildIterator::operator++()
{
  Next();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

WD_ALWAYS_INLINE wdGameObject::ChildIterator::ChildIterator(wdGameObject* pObject, const wdWorld* pWorld)
  : ConstChildIterator(pObject, pWorld)
{
}

WD_ALWAYS_INLINE wdGameObject& wdGameObject::ChildIterator::operator*()
{
  return *m_pObject;
}

WD_ALWAYS_INLINE wdGameObject* wdGameObject::ChildIterator::operator->()
{
  return m_pObject;
}

WD_ALWAYS_INLINE wdGameObject::ChildIterator::operator wdGameObject*()
{
  return m_pObject;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline wdGameObject::wdGameObject() = default;

WD_ALWAYS_INLINE wdGameObject::wdGameObject(const wdGameObject& other)
{
  *this = other;
}

WD_ALWAYS_INLINE wdGameObjectHandle wdGameObject::GetHandle() const
{
  return wdGameObjectHandle(m_InternalId);
}

WD_ALWAYS_INLINE bool wdGameObject::IsDynamic() const
{
  return m_Flags.IsSet(wdObjectFlags::Dynamic);
}

WD_ALWAYS_INLINE bool wdGameObject::IsStatic() const
{
  return !m_Flags.IsSet(wdObjectFlags::Dynamic);
}

WD_ALWAYS_INLINE bool wdGameObject::GetActiveFlag() const
{
  return m_Flags.IsSet(wdObjectFlags::ActiveFlag);
}

WD_ALWAYS_INLINE bool wdGameObject::IsActive() const
{
  return m_Flags.IsSet(wdObjectFlags::ActiveState);
}

WD_ALWAYS_INLINE void wdGameObject::SetName(wdStringView sName)
{
  m_sName.Assign(sName);
}

WD_ALWAYS_INLINE void wdGameObject::SetName(const wdHashedString& sName)
{
  m_sName = sName;
}

WD_ALWAYS_INLINE void wdGameObject::SetGlobalKey(wdStringView sKey)
{
  wdHashedString sGlobalKey;
  sGlobalKey.Assign(sKey);
  SetGlobalKey(sGlobalKey);
}

WD_ALWAYS_INLINE wdStringView wdGameObject::GetName() const
{
  return m_sName.GetView();
}

WD_ALWAYS_INLINE void wdGameObject::SetNameInternal(const char* szName)
{
  m_sName.Assign(szName);
}

WD_ALWAYS_INLINE const char* wdGameObject::GetNameInternal() const
{
  return m_sName;
}

WD_ALWAYS_INLINE void wdGameObject::SetGlobalKeyInternal(const char* szName)
{
  SetGlobalKey(szName);
}

WD_ALWAYS_INLINE bool wdGameObject::HasName(const wdTempHashedString& sName) const
{
  return m_sName == sName;
}

WD_ALWAYS_INLINE void wdGameObject::EnableChildChangesNotifications()
{
  m_Flags.Add(wdObjectFlags::ChildChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::DisableChildChangesNotifications()
{
  m_Flags.Remove(wdObjectFlags::ChildChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::EnableParentChangesNotifications()
{
  m_Flags.Add(wdObjectFlags::ParentChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::DisableParentChangesNotifications()
{
  m_Flags.Remove(wdObjectFlags::ParentChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::AddChildren(const wdArrayPtr<const wdGameObjectHandle>& children, wdGameObject::TransformPreservation preserve)
{
  for (wdUInt32 i = 0; i < children.GetCount(); ++i)
  {
    AddChild(children[i], preserve);
  }
}

WD_ALWAYS_INLINE void wdGameObject::DetachChildren(const wdArrayPtr<const wdGameObjectHandle>& children, wdGameObject::TransformPreservation preserve)
{
  for (wdUInt32 i = 0; i < children.GetCount(); ++i)
  {
    DetachChild(children[i], preserve);
  }
}

WD_ALWAYS_INLINE wdUInt32 wdGameObject::GetChildCount() const
{
  return m_uiChildCount;
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalPosition(wdVec3 vPosition)
{
  SetLocalPosition(wdSimdConversion::ToVec3(vPosition));
}

WD_ALWAYS_INLINE wdVec3 wdGameObject::GetLocalPosition() const
{
  return wdSimdConversion::ToVec3(m_pTransformationData->m_localPosition);
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalRotation(wdQuat qRotation)
{
  SetLocalRotation(wdSimdConversion::ToQuat(qRotation));
}

WD_ALWAYS_INLINE wdQuat wdGameObject::GetLocalRotation() const
{
  return wdSimdConversion::ToQuat(m_pTransformationData->m_localRotation);
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalScaling(wdVec3 vScaling)
{
  SetLocalScaling(wdSimdConversion::ToVec3(vScaling));
}

WD_ALWAYS_INLINE wdVec3 wdGameObject::GetLocalScaling() const
{
  return wdSimdConversion::ToVec3(m_pTransformationData->m_localScaling);
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalUniformScaling(float fScaling)
{
  SetLocalUniformScaling(wdSimdFloat(fScaling));
}

WD_ALWAYS_INLINE float wdGameObject::GetLocalUniformScaling() const
{
  return m_pTransformationData->m_localScaling.w();
}

WD_ALWAYS_INLINE wdTransform wdGameObject::GetLocalTransform() const
{
  return wdSimdConversion::ToTransform(GetLocalTransformSimd());
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalPosition(const wdVec3& vPosition)
{
  SetGlobalPosition(wdSimdConversion::ToVec3(vPosition));
}

WD_ALWAYS_INLINE wdVec3 wdGameObject::GetGlobalPosition() const
{
  return wdSimdConversion::ToVec3(m_pTransformationData->m_globalTransform.m_Position);
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalRotation(const wdQuat qRotation)
{
  SetGlobalRotation(wdSimdConversion::ToQuat(qRotation));
}

WD_ALWAYS_INLINE wdQuat wdGameObject::GetGlobalRotation() const
{
  return wdSimdConversion::ToQuat(m_pTransformationData->m_globalTransform.m_Rotation);
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalScaling(const wdVec3 vScaling)
{
  SetGlobalScaling(wdSimdConversion::ToVec3(vScaling));
}

WD_ALWAYS_INLINE wdVec3 wdGameObject::GetGlobalScaling() const
{
  return wdSimdConversion::ToVec3(m_pTransformationData->m_globalTransform.m_Scale);
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalTransform(const wdTransform& transform)
{
  SetGlobalTransform(wdSimdConversion::ToTransform(transform));
}

WD_ALWAYS_INLINE wdTransform wdGameObject::GetGlobalTransform() const
{
  return wdSimdConversion::ToTransform(m_pTransformationData->m_globalTransform);
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalPosition(const wdSimdVec4f& vPosition, UpdateBehaviorIfStatic updateBehavior)
{
  m_pTransformationData->m_localPosition = vPosition;

  if (IsStatic() && updateBehavior == UpdateBehaviorIfStatic::UpdateImmediately)
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdVec4f& wdGameObject::GetLocalPositionSimd() const
{
  return m_pTransformationData->m_localPosition;
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalRotation(const wdSimdQuat& qRotation, UpdateBehaviorIfStatic updateBehavior)
{
  m_pTransformationData->m_localRotation = qRotation;

  if (IsStatic() && updateBehavior == UpdateBehaviorIfStatic::UpdateImmediately)
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdQuat& wdGameObject::GetLocalRotationSimd() const
{
  return m_pTransformationData->m_localRotation;
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalScaling(const wdSimdVec4f& vScaling, UpdateBehaviorIfStatic updateBehavior)
{
  wdSimdFloat uniformScale = m_pTransformationData->m_localScaling.w();
  m_pTransformationData->m_localScaling = vScaling;
  m_pTransformationData->m_localScaling.SetW(uniformScale);

  if (IsStatic() && updateBehavior == UpdateBehaviorIfStatic::UpdateImmediately)
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdVec4f& wdGameObject::GetLocalScalingSimd() const
{
  return m_pTransformationData->m_localScaling;
}


WD_ALWAYS_INLINE void wdGameObject::SetLocalUniformScaling(const wdSimdFloat& fScaling, UpdateBehaviorIfStatic updateBehavior)
{
  m_pTransformationData->m_localScaling.SetW(fScaling);

  if (IsStatic() && updateBehavior == UpdateBehaviorIfStatic::UpdateImmediately)
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE wdSimdFloat wdGameObject::GetLocalUniformScalingSimd() const
{
  return m_pTransformationData->m_localScaling.w();
}

WD_ALWAYS_INLINE wdSimdTransform wdGameObject::GetLocalTransformSimd() const
{
  const wdSimdVec4f vScale = m_pTransformationData->m_localScaling * m_pTransformationData->m_localScaling.w();
  return wdSimdTransform(m_pTransformationData->m_localPosition, m_pTransformationData->m_localRotation, vScale);
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalPosition(const wdSimdVec4f& vPosition)
{
  m_pTransformationData->m_globalTransform.m_Position = vPosition;

  m_pTransformationData->UpdateLocalTransform();

  if (IsStatic())
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdVec4f& wdGameObject::GetGlobalPositionSimd() const
{
  return m_pTransformationData->m_globalTransform.m_Position;
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalRotation(const wdSimdQuat& qRotation)
{
  m_pTransformationData->m_globalTransform.m_Rotation = qRotation;

  m_pTransformationData->UpdateLocalTransform();

  if (IsStatic())
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdQuat& wdGameObject::GetGlobalRotationSimd() const
{
  return m_pTransformationData->m_globalTransform.m_Rotation;
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalScaling(const wdSimdVec4f& vScaling)
{
  m_pTransformationData->m_globalTransform.m_Scale = vScaling;

  m_pTransformationData->UpdateLocalTransform();

  if (IsStatic())
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdVec4f& wdGameObject::GetGlobalScalingSimd() const
{
  return m_pTransformationData->m_globalTransform.m_Scale;
}


WD_ALWAYS_INLINE void wdGameObject::SetGlobalTransform(const wdSimdTransform& transform)
{
  m_pTransformationData->m_globalTransform = transform;

  // wdTransformTemplate<Type>::SetLocalTransform will produce NaNs in w components
  // of pos and scale if scale.w is not set to 1 here. This only affects builds that
  // use WD_SIMD_IMPLEMENTATION_FPU, e.g. arm atm.
  m_pTransformationData->m_globalTransform.m_Scale.SetW(1.0f);
  m_pTransformationData->UpdateLocalTransform();

  if (IsStatic())
  {
    UpdateGlobalTransformAndBoundsRecursive();
  }
}

WD_ALWAYS_INLINE const wdSimdTransform& wdGameObject::GetGlobalTransformSimd() const
{
  return m_pTransformationData->m_globalTransform;
}

#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
WD_ALWAYS_INLINE void wdGameObject::SetVelocity(const wdVec3& vVelocity)
{
  m_pTransformationData->m_velocity = wdSimdVec4f(vVelocity.x, vVelocity.y, vVelocity.z, 1.0f);
}

WD_ALWAYS_INLINE wdVec3 wdGameObject::GetVelocity() const
{
  return wdSimdConversion::ToVec3(m_pTransformationData->m_velocity);
}
#endif

WD_ALWAYS_INLINE void wdGameObject::UpdateGlobalTransform()
{
  m_pTransformationData->UpdateGlobalTransformRecursive();
}

WD_ALWAYS_INLINE void wdGameObject::EnableStaticTransformChangesNotifications()
{
  m_Flags.Add(wdObjectFlags::StaticTransformChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::DisableStaticTransformChangesNotifications()
{
  m_Flags.Remove(wdObjectFlags::StaticTransformChangesNotifications);
}

WD_ALWAYS_INLINE wdBoundingBoxSphere wdGameObject::GetLocalBounds() const
{
  return wdSimdConversion::ToBBoxSphere(m_pTransformationData->m_localBounds);
}

WD_ALWAYS_INLINE wdBoundingBoxSphere wdGameObject::GetGlobalBounds() const
{
  return wdSimdConversion::ToBBoxSphere(m_pTransformationData->m_globalBounds);
}

WD_ALWAYS_INLINE const wdSimdBBoxSphere& wdGameObject::GetLocalBoundsSimd() const
{
  return m_pTransformationData->m_localBounds;
}

WD_ALWAYS_INLINE const wdSimdBBoxSphere& wdGameObject::GetGlobalBoundsSimd() const
{
  return m_pTransformationData->m_globalBounds;
}

WD_ALWAYS_INLINE wdSpatialDataHandle wdGameObject::GetSpatialData() const
{
  return m_pTransformationData->m_hSpatialData;
}

WD_ALWAYS_INLINE void wdGameObject::EnableComponentChangesNotifications()
{
  m_Flags.Add(wdObjectFlags::ComponentChangesNotifications);
}

WD_ALWAYS_INLINE void wdGameObject::DisableComponentChangesNotifications()
{
  m_Flags.Remove(wdObjectFlags::ComponentChangesNotifications);
}

template <typename T>
WD_ALWAYS_INLINE bool wdGameObject::TryGetComponentOfBaseType(T*& out_pComponent)
{
  return TryGetComponentOfBaseType(wdGetStaticRTTI<T>(), (wdComponent*&)out_pComponent);
}

template <typename T>
WD_ALWAYS_INLINE bool wdGameObject::TryGetComponentOfBaseType(const T*& out_pComponent) const
{
  return TryGetComponentOfBaseType(wdGetStaticRTTI<T>(), (const wdComponent*&)out_pComponent);
}

template <typename T>
void wdGameObject::TryGetComponentsOfBaseType(wdDynamicArray<T*>& out_components)
{
  out_components.Clear();

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf<T>())
    {
      out_components.PushBack(static_cast<T*>(pComponent));
    }
  }
}

template <typename T>
void wdGameObject::TryGetComponentsOfBaseType(wdDynamicArray<const T*>& out_components) const
{
  out_components.Clear();

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf<T>())
    {
      out_components.PushBack(static_cast<const T*>(pComponent));
    }
  }
}

WD_ALWAYS_INLINE wdArrayPtr<wdComponent* const> wdGameObject::GetComponents()
{
  return m_Components;
}

WD_ALWAYS_INLINE wdArrayPtr<const wdComponent* const> wdGameObject::GetComponents() const
{
  return wdMakeArrayPtr(const_cast<const wdComponent* const*>(m_Components.GetData()), m_Components.GetCount());
}

WD_ALWAYS_INLINE wdUInt16 wdGameObject::GetComponentVersion() const
{
  return m_Components.GetUserData<ComponentUserData>().m_uiVersion;
}

WD_ALWAYS_INLINE bool wdGameObject::SendMessage(wdMessage& ref_msg)
{
  return SendMessageInternal(ref_msg, false);
}

WD_ALWAYS_INLINE bool wdGameObject::SendMessage(wdMessage& ref_msg) const
{
  return SendMessageInternal(ref_msg, false);
}

WD_ALWAYS_INLINE bool wdGameObject::SendMessageRecursive(wdMessage& ref_msg)
{
  return SendMessageRecursiveInternal(ref_msg, false);
}

WD_ALWAYS_INLINE bool wdGameObject::SendMessageRecursive(wdMessage& ref_msg) const
{
  return SendMessageRecursiveInternal(ref_msg, false);
}

WD_ALWAYS_INLINE const wdTagSet& wdGameObject::GetTags() const
{
  return m_Tags;
}

WD_ALWAYS_INLINE wdUInt32 wdGameObject::GetStableRandomSeed() const
{
  return m_pTransformationData->m_uiStableRandomSeed;
}

WD_ALWAYS_INLINE void wdGameObject::SetStableRandomSeed(wdUInt32 uiSeed)
{
  m_pTransformationData->m_uiStableRandomSeed = uiSeed;
}

//////////////////////////////////////////////////////////////////////////

WD_ALWAYS_INLINE void wdGameObject::TransformationData::UpdateGlobalTransformWithoutParent()
{
  m_globalTransform.m_Position = m_localPosition;
  m_globalTransform.m_Rotation = m_localRotation;
  m_globalTransform.m_Scale = m_localScaling * m_localScaling.w();
}

WD_ALWAYS_INLINE void wdGameObject::TransformationData::UpdateGlobalTransformWithParent()
{
  const wdSimdVec4f vScale = m_localScaling * m_localScaling.w();
  const wdSimdTransform localTransform(m_localPosition, m_localRotation, vScale);
  m_globalTransform.SetGlobalTransform(m_pParentData->m_globalTransform, localTransform);
}

WD_FORCE_INLINE void wdGameObject::TransformationData::UpdateGlobalBounds()
{
  m_globalBounds = m_localBounds;
  m_globalBounds.Transform(m_globalTransform);
}

WD_ALWAYS_INLINE void wdGameObject::TransformationData::UpdateVelocity(const wdSimdFloat& fInvDeltaSeconds)
{
#if WD_ENABLED(WD_GAMEOBJECT_VELOCITY)
  // A w value != 0 indicates a custom velocity, don't overwrite it.
  wdSimdVec4b customVel = (m_velocity.Get<wdSwizzle::WWWW>() != wdSimdVec4f::ZeroVector());
  wdSimdVec4f newVel = (m_globalTransform.m_Position - m_lastGlobalPosition) * fInvDeltaSeconds;
  m_velocity = wdSimdVec4f::Select(customVel, m_velocity, newVel);

  m_lastGlobalPosition = m_globalTransform.m_Position;
  m_velocity.SetW(wdSimdFloat::Zero());
#endif
}
