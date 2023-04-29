#include <Core/CorePCH.h>

#include <Core/Messages/DeleteObjectMessage.h>
#include <Core/Messages/HierarchyChangedMessages.h>
#include <Core/Messages/TransformChangedMessage.h>
#include <Core/Messages/UpdateLocalBoundsMessage.h>
#include <Core/World/EventMessageHandlerComponent.h>
#include <Core/World/World.h>

namespace
{
  static wdVariantArray GetDefaultTags()
  {
    wdVariantArray value(wdStaticAllocatorWrapper::GetAllocator());
    value.PushBack(wdStringView("CastShadow"));
    return value;
  }
} // namespace

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdGameObject, wdNoBase, 1, wdRTTINoAllocator)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Name", GetNameInternal, SetNameInternal),
    WD_ACCESSOR_PROPERTY("Active", GetActiveFlag, SetActiveFlag)->AddAttributes(new wdDefaultValueAttribute(true)),
    WD_ACCESSOR_PROPERTY("GlobalKey", GetGlobalKeyInternal, SetGlobalKeyInternal),
    WD_ENUM_ACCESSOR_PROPERTY("Mode", wdObjectMode, Reflection_GetMode, Reflection_SetMode),
    WD_ACCESSOR_PROPERTY("LocalPosition", GetLocalPosition, SetLocalPosition)->AddAttributes(new wdSuffixAttribute(" m")),
    WD_ACCESSOR_PROPERTY("LocalRotation", GetLocalRotation, SetLocalRotation),
    WD_ACCESSOR_PROPERTY("LocalScaling", GetLocalScaling, SetLocalScaling)->AddAttributes(new wdDefaultValueAttribute(wdVec3(1.0f, 1.0f, 1.0f))),
    WD_ACCESSOR_PROPERTY("LocalUniformScaling", GetLocalUniformScaling, SetLocalUniformScaling)->AddAttributes(new wdDefaultValueAttribute(1.0f)),
    WD_SET_MEMBER_PROPERTY("Tags", m_Tags)->AddAttributes(new wdTagSetWidgetAttribute("Default"), new wdDefaultValueAttribute(GetDefaultTags())),
    WD_SET_ACCESSOR_PROPERTY("Children", Reflection_GetChildren, Reflection_AddChild, Reflection_DetachChild)->AddFlags(wdPropertyFlags::PointerOwner | wdPropertyFlags::Hidden),
    WD_SET_ACCESSOR_PROPERTY("Components", Reflection_GetComponents, Reflection_AddComponent, Reflection_RemoveComponent)->AddFlags(wdPropertyFlags::PointerOwner),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_MESSAGEHANDLERS
  {
    WD_MESSAGE_HANDLER(wdMsgDeleteGameObject, OnMsgDeleteGameObject),
  }
  WD_END_MESSAGEHANDLERS;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on

void wdGameObject::Reflection_AddChild(wdGameObject* pChild)
{
  if (IsDynamic())
  {
    pChild->MakeDynamic();
  }

  AddChild(pChild->GetHandle(), TransformPreservation::PreserveLocal);

  // Check whether the child object was only dynamic because of its old parent
  // If that's the case make it static now.
  pChild->ConditionalMakeStatic();
}

void wdGameObject::Reflection_DetachChild(wdGameObject* pChild)
{
  DetachChild(pChild->GetHandle(), TransformPreservation::PreserveLocal);

  // The child object is now a top level object, check whether it should be static now.
  pChild->ConditionalMakeStatic();
}

wdHybridArray<wdGameObject*, 8> wdGameObject::Reflection_GetChildren() const
{
  ConstChildIterator it = GetChildren();

  wdHybridArray<wdGameObject*, 8> all;
  all.Reserve(GetChildCount());

  while (it.IsValid())
  {
    all.PushBack(it.m_pObject);
    ++it;
  }

  return all;
}

void wdGameObject::Reflection_AddComponent(wdComponent* pComponent)
{
  if (pComponent == nullptr)
    return;

  if (pComponent->IsDynamic())
  {
    MakeDynamic();
  }

  AddComponent(pComponent);
}

void wdGameObject::Reflection_RemoveComponent(wdComponent* pComponent)
{
  if (pComponent == nullptr)
    return;

  /*Don't call RemoveComponent here, Component is automatically removed when deleted.*/

  if (pComponent->IsDynamic())
  {
    ConditionalMakeStatic(pComponent);
  }
}

wdHybridArray<wdComponent*, wdGameObject::NUM_INPLACE_COMPONENTS> wdGameObject::Reflection_GetComponents() const
{
  return wdHybridArray<wdComponent*, wdGameObject::NUM_INPLACE_COMPONENTS>(m_Components);
}

wdObjectMode::Enum wdGameObject::Reflection_GetMode() const
{
  return m_Flags.IsSet(wdObjectFlags::ForceDynamic) ? wdObjectMode::ForceDynamic : wdObjectMode::Automatic;
}

void wdGameObject::Reflection_SetMode(wdObjectMode::Enum mode)
{
  if (Reflection_GetMode() == mode)
  {
    return;
  }

  if (mode == wdObjectMode::ForceDynamic)
  {
    m_Flags.Add(wdObjectFlags::ForceDynamic);
    MakeDynamic();
  }
  else
  {
    m_Flags.Remove(wdObjectFlags::ForceDynamic);
    ConditionalMakeStatic();
  }
}

bool wdGameObject::DetermineDynamicMode(wdComponent* pComponentToIgnore /*= nullptr*/) const
{
  if (m_Flags.IsSet(wdObjectFlags::ForceDynamic))
  {
    return true;
  }

  const wdGameObject* pParent = GetParent();
  if (pParent != nullptr && pParent->IsDynamic())
  {
    return true;
  }

  for (auto pComponent : m_Components)
  {
    if (pComponent != pComponentToIgnore && pComponent->IsDynamic())
    {
      return true;
    }
  }

  return false;
}

void wdGameObject::ConditionalMakeStatic(wdComponent* pComponentToIgnore /*= nullptr*/)
{
  if (!DetermineDynamicMode(pComponentToIgnore))
  {
    MakeStaticInternal();

    for (auto it = GetChildren(); it.IsValid(); ++it)
    {
      it->ConditionalMakeStatic();
    }
  }
}

void wdGameObject::MakeStaticInternal()
{
  if (IsStatic())
  {
    return;
  }

  m_Flags.Remove(wdObjectFlags::Dynamic);

  GetWorld()->RecreateHierarchyData(this, true);
}

void wdGameObject::UpdateGlobalTransformAndBoundsRecursive()
{
  if (IsStatic() && GetWorld()->ReportErrorWhenStaticObjectMoves())
  {
    wdLog::Error("Static object '{0}' was moved during runtime.", GetName());
  }

  wdSimdTransform oldGlobalTransform = GetGlobalTransformSimd();

  m_pTransformationData->UpdateGlobalTransformNonRecursive();

  if (wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem())
  {
    m_pTransformationData->UpdateGlobalBoundsAndSpatialData(*pSpatialSystem);
  }
  else
  {
    m_pTransformationData->UpdateGlobalBounds();
  }

  if (IsStatic() && m_Flags.IsSet(wdObjectFlags::StaticTransformChangesNotifications) && oldGlobalTransform != GetGlobalTransformSimd())
  {
    wdMsgTransformChanged msg;
    msg.m_OldGlobalTransform = wdSimdConversion::ToTransform(oldGlobalTransform);
    msg.m_NewGlobalTransform = GetGlobalTransform();

    SendMessage(msg);
  }

  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    it->UpdateGlobalTransformAndBoundsRecursive();
  }
}

void wdGameObject::ConstChildIterator::Next()
{
  m_pObject = m_pWorld->GetObjectUnchecked(m_pObject->m_uiNextSiblingIndex);
}

wdGameObject::~wdGameObject()
{
  // Since we are using the small array base class for components we have to cleanup ourself with the correct allocator.
  m_Components.Clear();
  m_Components.Compact(GetWorld()->GetAllocator());
}

void wdGameObject::operator=(const wdGameObject& other)
{
  WD_ASSERT_DEV(m_InternalId.m_WorldIndex == other.m_InternalId.m_WorldIndex, "Cannot copy between worlds.");

  m_InternalId = other.m_InternalId;
  m_Flags = other.m_Flags;
  m_sName = other.m_sName;

  m_uiParentIndex = other.m_uiParentIndex;
  m_uiFirstChildIndex = other.m_uiFirstChildIndex;
  m_uiLastChildIndex = other.m_uiLastChildIndex;

  m_uiNextSiblingIndex = other.m_uiNextSiblingIndex;
  m_uiPrevSiblingIndex = other.m_uiPrevSiblingIndex;
  m_uiChildCount = other.m_uiChildCount;

  m_uiTeamID = other.m_uiTeamID;

  m_uiHierarchyLevel = other.m_uiHierarchyLevel;
  m_pTransformationData = other.m_pTransformationData;
  m_pTransformationData->m_pObject = this;

  if (!m_pTransformationData->m_hSpatialData.IsInvalidated())
  {
    wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem();
    pSpatialSystem->UpdateSpatialDataObject(m_pTransformationData->m_hSpatialData, this);
  }

  m_Components.CopyFrom(other.m_Components, GetWorld()->GetAllocator());
  for (wdComponent* pComponent : m_Components)
  {
    WD_ASSERT_DEV(pComponent->m_pOwner == &other, "");
    pComponent->m_pOwner = this;
  }

  m_Tags = other.m_Tags;
}

void wdGameObject::MakeDynamic()
{
  if (IsDynamic())
  {
    return;
  }

  m_Flags.Add(wdObjectFlags::Dynamic);

  GetWorld()->RecreateHierarchyData(this, false);

  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    it->MakeDynamic();
  }
}

void wdGameObject::MakeStatic()
{
  WD_ASSERT_DEV(!DetermineDynamicMode(), "This object can't be static because it has a dynamic parent or dynamic component(s) attached.");

  MakeStaticInternal();
}

void wdGameObject::SetActiveFlag(bool bEnabled)
{
  if (m_Flags.IsSet(wdObjectFlags::ActiveFlag) == bEnabled)
    return;

  m_Flags.AddOrRemove(wdObjectFlags::ActiveFlag, bEnabled);

  UpdateActiveState(GetParent() == nullptr ? true : GetParent()->IsActive());
}

void wdGameObject::UpdateActiveState(bool bParentActive)
{
  const bool bSelfActive = bParentActive && m_Flags.IsSet(wdObjectFlags::ActiveFlag);

  if (bSelfActive != m_Flags.IsSet(wdObjectFlags::ActiveState))
  {
    m_Flags.AddOrRemove(wdObjectFlags::ActiveState, bSelfActive);

    for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
    {
      m_Components[i]->UpdateActiveState(bSelfActive);
    }

    // recursively update all children
    for (auto it = GetChildren(); it.IsValid(); ++it)
    {
      it->UpdateActiveState(bSelfActive);
    }
  }
}

void wdGameObject::SetGlobalKey(const wdHashedString& sName)
{
  GetWorld()->SetObjectGlobalKey(this, sName);
}

wdStringView wdGameObject::GetGlobalKey() const
{
  return GetWorld()->GetObjectGlobalKey(this);
}

const char* wdGameObject::GetGlobalKeyInternal() const
{
  return GetWorld()->GetObjectGlobalKey(this).GetStartPointer(); // we know that it's zero terminated
}

void wdGameObject::SetParent(const wdGameObjectHandle& hParent, wdGameObject::TransformPreservation preserve)
{
  wdWorld* pWorld = GetWorld();

  wdGameObject* pParent = nullptr;
  bool _ = pWorld->TryGetObject(hParent, pParent);
  pWorld->SetParent(this, pParent, preserve);
}

wdGameObject* wdGameObject::GetParent()
{
  return GetWorld()->GetObjectUnchecked(m_uiParentIndex);
}

const wdGameObject* wdGameObject::GetParent() const
{
  return GetWorld()->GetObjectUnchecked(m_uiParentIndex);
}

void wdGameObject::AddChild(const wdGameObjectHandle& hChild, wdGameObject::TransformPreservation preserve)
{
  wdWorld* pWorld = GetWorld();

  wdGameObject* pChild = nullptr;
  if (pWorld->TryGetObject(hChild, pChild))
  {
    pWorld->SetParent(pChild, this, preserve);
  }
}

void wdGameObject::DetachChild(const wdGameObjectHandle& hChild, wdGameObject::TransformPreservation preserve)
{
  wdWorld* pWorld = GetWorld();

  wdGameObject* pChild = nullptr;
  if (pWorld->TryGetObject(hChild, pChild))
  {
    if (pChild->GetParent() == this)
    {
      pWorld->SetParent(pChild, nullptr, preserve);
    }
  }
}

wdGameObject::ChildIterator wdGameObject::GetChildren()
{
  wdWorld* pWorld = GetWorld();
  return ChildIterator(pWorld->GetObjectUnchecked(m_uiFirstChildIndex), pWorld);
}

wdGameObject::ConstChildIterator wdGameObject::GetChildren() const
{
  const wdWorld* pWorld = GetWorld();
  return ConstChildIterator(pWorld->GetObjectUnchecked(m_uiFirstChildIndex), pWorld);
}

wdGameObject* wdGameObject::FindChildByName(const wdTempHashedString& sName, bool bRecursive /*= true*/)
{
  /// \test Needs a unit test

  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    if (it->m_sName == sName)
    {
      return &(*it);
    }
  }

  if (bRecursive)
  {
    for (auto it = GetChildren(); it.IsValid(); ++it)
    {
      wdGameObject* pChild = it->FindChildByName(sName, bRecursive);

      if (pChild != nullptr)
        return pChild;
    }
  }

  return nullptr;
}

wdGameObject* wdGameObject::FindChildByPath(wdStringView sPath)
{
  /// \test Needs a unit test

  if (sPath.IsEmpty())
    return this;

  const char* szSep = sPath.FindSubString("/");
  wdUInt64 uiNameHash = 0;

  if (szSep == nullptr)
    uiNameHash = wdHashingUtils::StringHash(sPath);
  else
    uiNameHash = wdHashingUtils::StringHash(wdStringView(sPath.GetStartPointer(), szSep));

  wdGameObject* pNextChild = FindChildByName(wdTempHashedString(uiNameHash), false);

  if (szSep == nullptr || pNextChild == nullptr)
    return pNextChild;

  return pNextChild->FindChildByPath(wdStringView(szSep + 1, sPath.GetEndPointer()));
}


wdGameObject* wdGameObject::SearchForChildByNameSequence(wdStringView sObjectSequence, const wdRTTI* pExpectedComponent /*= nullptr*/)
{
  /// \test Needs a unit test

  if (sObjectSequence.IsEmpty())
  {
    // in case we are searching for a specific component type, verify that it exists on this object
    if (pExpectedComponent != nullptr)
    {
      wdComponent* pComp = nullptr;
      if (!TryGetComponentOfBaseType(pExpectedComponent, pComp))
        return nullptr;
    }

    return this;
  }

  const char* szSep = sObjectSequence.FindSubString("/");
  wdStringView sNextSequence;
  wdUInt64 uiNameHash = 0;

  if (szSep == nullptr)
  {
    uiNameHash = wdHashingUtils::StringHash(sObjectSequence);
  }
  else
  {
    uiNameHash = wdHashingUtils::StringHash(wdStringView(sObjectSequence.GetStartPointer(), szSep));
    sNextSequence = wdStringView(szSep + 1, sObjectSequence.GetEndPointer());
  }

  const wdTempHashedString name(uiNameHash);

  // first go through all direct children an see if any of them actually matches the current name
  // if so, continue the recursion from there and give them the remaining search path to continue
  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    if (it->m_sName == name)
    {
      wdGameObject* res = it->SearchForChildByNameSequence(sNextSequence, pExpectedComponent);
      if (res != nullptr)
        return res;
    }
  }

  // if no direct child fulfilled the requirements, just recurse with the full name sequence
  // however, we can skip any child that already fulfilled the next sequence name,
  // because that's definitely a lost cause
  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    if (it->m_sName != name)
    {
      wdGameObject* res = it->SearchForChildByNameSequence(sObjectSequence, pExpectedComponent);
      if (res != nullptr)
        return res;
    }
  }

  return nullptr;
}


void wdGameObject::SearchForChildrenByNameSequence(wdStringView sObjectSequence, const wdRTTI* pExpectedComponent, wdHybridArray<wdGameObject*, 8>& out_objects)
{
  /// \test Needs a unit test

  if (sObjectSequence.IsEmpty())
  {
    // in case we are searching for a specific component type, verify that it exists on this object
    if (pExpectedComponent != nullptr)
    {
      wdComponent* pComp = nullptr;
      if (!TryGetComponentOfBaseType(pExpectedComponent, pComp))
        return;
    }

    out_objects.PushBack(this);
    return;
  }

  const char* szSep = sObjectSequence.FindSubString("/");
  wdStringView sNextSequence;
  wdUInt64 uiNameHash = 0;

  if (szSep == nullptr)
  {
    uiNameHash = wdHashingUtils::StringHash(sObjectSequence);
  }
  else
  {
    uiNameHash = wdHashingUtils::StringHash(wdStringView(sObjectSequence.GetStartPointer(), szSep));
    sNextSequence = wdStringView(szSep + 1, sObjectSequence.GetEndPointer());
  }

  const wdTempHashedString name(uiNameHash);

  // first go through all direct children an see if any of them actually matches the current name
  // if so, continue the recursion from there and give them the remaining search path to continue
  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    if (it->m_sName == name)
    {
      it->SearchForChildrenByNameSequence(sNextSequence, pExpectedComponent, out_objects);
    }
  }

  // if no direct child fulfilled the requirements, just recurse with the full name sequence
  // however, we can skip any child that already fulfilled the next sequence name,
  // because that's definitely a lost cause
  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    if (it->m_sName != name) // TODO: in this function it is actually debatable whether to skip these or not
    {
      it->SearchForChildrenByNameSequence(sObjectSequence, pExpectedComponent, out_objects);
    }
  }
}

wdWorld* wdGameObject::GetWorld()
{
  return wdWorld::GetWorld(m_InternalId.m_WorldIndex);
}

const wdWorld* wdGameObject::GetWorld() const
{
  return wdWorld::GetWorld(m_InternalId.m_WorldIndex);
}

wdVec3 wdGameObject::GetGlobalDirForwards() const
{
  wdCoordinateSystem coordinateSystem;
  GetWorld()->GetCoordinateSystem(GetGlobalPosition(), coordinateSystem);

  return GetGlobalRotation() * coordinateSystem.m_vForwardDir;
}

wdVec3 wdGameObject::GetGlobalDirRight() const
{
  wdCoordinateSystem coordinateSystem;
  GetWorld()->GetCoordinateSystem(GetGlobalPosition(), coordinateSystem);

  return GetGlobalRotation() * coordinateSystem.m_vRightDir;
}

wdVec3 wdGameObject::GetGlobalDirUp() const
{
  wdCoordinateSystem coordinateSystem;
  GetWorld()->GetCoordinateSystem(GetGlobalPosition(), coordinateSystem);

  return GetGlobalRotation() * coordinateSystem.m_vUpDir;
}

void wdGameObject::UpdateLocalBounds()
{
  wdMsgUpdateLocalBounds msg;
  msg.m_ResultingLocalBounds.SetInvalid();

  SendMessage(msg);

  const bool bIsAlwaysVisible = m_pTransformationData->m_localBounds.m_BoxHalfExtents.w() != wdSimdFloat::Zero();
  bool bRecreateSpatialData = false;

  if (m_pTransformationData->m_hSpatialData.IsInvalidated() == false)
  {
    // force spatial data re-creation if categories have changed
    bRecreateSpatialData |= m_pTransformationData->m_uiSpatialDataCategoryBitmask != msg.m_uiSpatialDataCategoryBitmask;

    // force spatial data re-creation if always visible flag has changed
    bRecreateSpatialData |= bIsAlwaysVisible != msg.m_bAlwaysVisible;

    // delete old spatial data if bounds are now invalid
    bRecreateSpatialData |= msg.m_bAlwaysVisible == false && msg.m_ResultingLocalBounds.IsValid() == false;
  }

  m_pTransformationData->m_localBounds = wdSimdConversion::ToBBoxSphere(msg.m_ResultingLocalBounds);
  m_pTransformationData->m_localBounds.m_BoxHalfExtents.SetW(msg.m_bAlwaysVisible ? 1.0f : 0.0f);
  m_pTransformationData->m_uiSpatialDataCategoryBitmask = msg.m_uiSpatialDataCategoryBitmask;

  wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem();
  if (pSpatialSystem != nullptr && (bRecreateSpatialData || m_pTransformationData->m_hSpatialData.IsInvalidated()))
  {
    m_pTransformationData->RecreateSpatialData(*pSpatialSystem);
  }

  if (IsStatic())
  {
    m_pTransformationData->UpdateGlobalBounds(pSpatialSystem);
  }
}

void wdGameObject::UpdateGlobalTransformAndBounds()
{
  m_pTransformationData->UpdateGlobalTransformRecursive();
  m_pTransformationData->UpdateGlobalBounds(GetWorld()->GetSpatialSystem());
}

void wdGameObject::UpdateGlobalBounds()
{
  m_pTransformationData->UpdateGlobalBounds(GetWorld()->GetSpatialSystem());
}

bool wdGameObject::TryGetComponentOfBaseType(const wdRTTI* pType, wdComponent*& out_pComponent)
{
  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf(pType))
    {
      out_pComponent = pComponent;
      return true;
    }
  }

  out_pComponent = nullptr;
  return false;
}

bool wdGameObject::TryGetComponentOfBaseType(const wdRTTI* pType, const wdComponent*& out_pComponent) const
{
  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf(pType))
    {
      out_pComponent = pComponent;
      return true;
    }
  }

  out_pComponent = nullptr;
  return false;
}


void wdGameObject::TryGetComponentsOfBaseType(const wdRTTI* pType, wdDynamicArray<wdComponent*>& out_components)
{
  out_components.Clear();

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf(pType))
    {
      out_components.PushBack(pComponent);
    }
  }
}

void wdGameObject::TryGetComponentsOfBaseType(const wdRTTI* pType, wdDynamicArray<const wdComponent*>& out_components) const
{
  out_components.Clear();

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    if (pComponent->IsInstanceOf(pType))
    {
      out_components.PushBack(pComponent);
    }
  }
}

void wdGameObject::SetTeamID(wdUInt16 uiId)
{
  m_uiTeamID = uiId;

  for (auto it = GetChildren(); it.IsValid(); ++it)
  {
    it->SetTeamID(uiId);
  }
}

wdUInt64 wdGameObject::GetNumFramesSinceVisible() const
{
  if (!m_pTransformationData->m_hSpatialData.IsInvalidated())
  {
    const wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem();
    return pSpatialSystem->GetNumFramesSinceVisible(m_pTransformationData->m_hSpatialData);
  }

  return 0;
}

void wdGameObject::OnMsgDeleteGameObject(wdMsgDeleteGameObject& msg)
{
  GetWorld()->DeleteObjectNow(GetHandle(), msg.m_bDeleteEmptyParents);
}

void wdGameObject::AddComponent(wdComponent* pComponent)
{
  WD_ASSERT_DEV(pComponent->m_pOwner == nullptr, "Component must not be added twice.");
  WD_ASSERT_DEV(IsDynamic() || !pComponent->IsDynamic(), "Cannot attach a dynamic component to a static object. Call MakeDynamic() first.");

  pComponent->m_pOwner = this;
  m_Components.PushBack(pComponent, GetWorld()->GetAllocator());
  m_Components.GetUserData<ComponentUserData>().m_uiVersion++;

  pComponent->UpdateActiveState(IsActive());

  if (m_Flags.IsSet(wdObjectFlags::ComponentChangesNotifications))
  {
    wdMsgComponentsChanged msg;
    msg.m_Type = wdMsgComponentsChanged::Type::ComponentAdded;
    msg.m_hOwner = GetHandle();
    msg.m_hComponent = pComponent->GetHandle();

    SendNotificationMessage(msg);
  }
}

void wdGameObject::RemoveComponent(wdComponent* pComponent)
{
  wdUInt32 uiIndex = m_Components.IndexOf(pComponent);
  WD_ASSERT_DEV(uiIndex != wdInvalidIndex, "Component not found");

  pComponent->m_pOwner = nullptr;
  m_Components.RemoveAtAndSwap(uiIndex);
  m_Components.GetUserData<ComponentUserData>().m_uiVersion++;

  if (m_Flags.IsSet(wdObjectFlags::ComponentChangesNotifications))
  {
    wdMsgComponentsChanged msg;
    msg.m_Type = wdMsgComponentsChanged::Type::ComponentRemoved;
    msg.m_hOwner = GetHandle();
    msg.m_hComponent = pComponent->GetHandle();

    SendNotificationMessage(msg);
  }
}

bool wdGameObject::SendMessageInternal(wdMessage& msg, bool bWasPostedMsg)
{
  bool bSentToAny = false;

  const wdRTTI* pRtti = wdGetStaticRTTI<wdGameObject>();
  bSentToAny |= pRtti->DispatchMessage(this, msg);

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    bSentToAny |= pComponent->SendMessageInternal(msg, bWasPostedMsg);
  }

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  if (!bSentToAny && msg.GetDebugMessageRouting())
  {
    wdLog::Warning("wdGameObject::SendMessage: None of the target object's components had a handler for messages of type {0}.", msg.GetId());
  }
#endif

  return bSentToAny;
}

bool wdGameObject::SendMessageInternal(wdMessage& msg, bool bWasPostedMsg) const
{
  bool bSentToAny = false;

  const wdRTTI* pRtti = wdGetStaticRTTI<wdGameObject>();
  bSentToAny |= pRtti->DispatchMessage(this, msg);

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    // forward only to 'const' message handlers
    const wdComponent* pComponent = m_Components[i];
    bSentToAny |= pComponent->SendMessageInternal(msg, bWasPostedMsg);
  }

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  if (!bSentToAny && msg.GetDebugMessageRouting())
  {
    wdLog::Warning("wdGameObject::SendMessage (const): None of the target object's components had a handler for messages of type {0}.", msg.GetId());
  }
#endif

  return bSentToAny;
}

bool wdGameObject::SendMessageRecursiveInternal(wdMessage& msg, bool bWasPostedMsg)
{
  bool bSentToAny = false;

  const wdRTTI* pRtti = wdGetStaticRTTI<wdGameObject>();
  bSentToAny |= pRtti->DispatchMessage(this, msg);

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    wdComponent* pComponent = m_Components[i];
    bSentToAny |= pComponent->SendMessageInternal(msg, bWasPostedMsg);
  }

  for (auto childIt = GetChildren(); childIt.IsValid(); ++childIt)
  {
    bSentToAny |= childIt->SendMessageRecursiveInternal(msg, bWasPostedMsg);
  }

  // should only be evaluated at the top function call
  // #if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  //  if (!bSentToAny && msg.GetDebugMessageRouting())
  //  {
  //    wdLog::Warning("wdGameObject::SendMessageRecursive: None of the target object's components had a handler for messages of type {0}.",
  //    msg.GetId());
  //  }
  // #endif
  // #
  return bSentToAny;
}

bool wdGameObject::SendMessageRecursiveInternal(wdMessage& msg, bool bWasPostedMsg) const
{
  bool bSentToAny = false;

  const wdRTTI* pRtti = wdGetStaticRTTI<wdGameObject>();
  bSentToAny |= pRtti->DispatchMessage(this, msg);

  for (wdUInt32 i = 0; i < m_Components.GetCount(); ++i)
  {
    // forward only to 'const' message handlers
    const wdComponent* pComponent = m_Components[i];
    bSentToAny |= pComponent->SendMessageInternal(msg, bWasPostedMsg);
  }

  for (auto childIt = GetChildren(); childIt.IsValid(); ++childIt)
  {
    bSentToAny |= childIt->SendMessageRecursiveInternal(msg, bWasPostedMsg);
  }

  // should only be evaluated at the top function call
  // #if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  //  if (!bSentToAny && msg.GetDebugMessageRouting())
  //  {
  //    wdLog::Warning("wdGameObject::SendMessageRecursive(const): None of the target object's components had a handler for messages of type
  //    {0}.", msg.GetId());
  //  }
  // #endif
  // #
  return bSentToAny;
}

void wdGameObject::PostMessage(const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  GetWorld()->PostMessage(GetHandle(), msg, delay, queueType);
}

void wdGameObject::PostMessageRecursive(const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  GetWorld()->PostMessageRecursive(GetHandle(), msg, delay, queueType);
}

void wdGameObject::SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent)
{
  if (auto pEventMsg = wdDynamicCast<wdEventMessage*>(&ref_msg))
  {
    pEventMsg->FillFromSenderComponent(pSenderComponent);
  }

  wdHybridArray<wdComponent*, 4> eventMsgHandlers;
  GetWorld()->FindEventMsgHandlers(ref_msg, this, eventMsgHandlers);

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  if (ref_msg.GetDebugMessageRouting())
  {
    if (eventMsgHandlers.IsEmpty())
    {
      wdLog::Warning("wdGameObject::SendEventMessage: None of the target object's components had a handler for messages of type {0}.", ref_msg.GetId());
    }
  }
#endif

  for (auto pEventMsgHandler : eventMsgHandlers)
  {
    pEventMsgHandler->SendMessage(ref_msg);
  }
}

void wdGameObject::SendEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent) const
{
  if (auto pEventMsg = wdDynamicCast<wdEventMessage*>(&ref_msg))
  {
    pEventMsg->FillFromSenderComponent(pSenderComponent);
  }

  wdHybridArray<const wdComponent*, 4> eventMsgHandlers;
  GetWorld()->FindEventMsgHandlers(ref_msg, this, eventMsgHandlers);

  for (auto pEventMsgHandler : eventMsgHandlers)
  {
    pEventMsgHandler->SendMessage(ref_msg);
  }
}

void wdGameObject::PostEventMessage(wdMessage& ref_msg, const wdComponent* pSenderComponent, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  if (auto pEventMsg = wdDynamicCast<wdEventMessage*>(&ref_msg))
  {
    pEventMsg->FillFromSenderComponent(pSenderComponent);
  }

  wdHybridArray<const wdComponent*, 4> eventMsgHandlers;
  GetWorld()->FindEventMsgHandlers(ref_msg, this, eventMsgHandlers);

  for (auto pEventMsgHandler : eventMsgHandlers)
  {
    pEventMsgHandler->PostMessage(ref_msg);
  }
}

void wdGameObject::SetTags(const wdTagSet& tags)
{
  if (wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem())
  {
    if (m_Tags != tags)
    {
      m_Tags = tags;
      m_pTransformationData->RecreateSpatialData(*pSpatialSystem);
    }
  }
  else
  {
    m_Tags = tags;
  }
}

void wdGameObject::SetTag(const wdTag& tag)
{
  if (wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem())
  {
    if (m_Tags.IsSet(tag) == false)
    {
      m_Tags.Set(tag);
      m_pTransformationData->RecreateSpatialData(*pSpatialSystem);
    }
  }
  else
  {
    m_Tags.Set(tag);
  }
}

void wdGameObject::RemoveTag(const wdTag& tag)
{
  if (wdSpatialSystem* pSpatialSystem = GetWorld()->GetSpatialSystem())
  {
    if (m_Tags.IsSet(tag))
    {
      m_Tags.Remove(tag);
      m_pTransformationData->RecreateSpatialData(*pSpatialSystem);
    }
  }
  else
  {
    m_Tags.Remove(tag);
  }
}

void wdGameObject::FixComponentPointer(wdComponent* pOldPtr, wdComponent* pNewPtr)
{
  wdUInt32 uiIndex = m_Components.IndexOf(pOldPtr);
  WD_ASSERT_DEV(uiIndex != wdInvalidIndex, "Memory corruption?");
  m_Components[uiIndex] = pNewPtr;
}

void wdGameObject::SendNotificationMessage(wdMessage& msg)
{
  wdGameObject* pObject = this;
  while (pObject != nullptr)
  {
    pObject->SendMessage(msg);

    pObject = pObject->GetParent();
  }
}

//////////////////////////////////////////////////////////////////////////

void wdGameObject::TransformationData::UpdateLocalTransform()
{
  wdSimdTransform tLocal;

  if (m_pParentData != nullptr)
  {
    tLocal.SetLocalTransform(m_pParentData->m_globalTransform, m_globalTransform);
  }
  else
  {
    tLocal = m_globalTransform;
  }

  m_localPosition = tLocal.m_Position;
  m_localRotation = tLocal.m_Rotation;
  m_localScaling = tLocal.m_Scale;
  m_localScaling.SetW(1.0f);
}

void wdGameObject::TransformationData::UpdateGlobalTransformNonRecursive()
{
  if (m_pParentData != nullptr)
  {
    UpdateGlobalTransformWithParent();
  }
  else
  {
    UpdateGlobalTransformWithoutParent();
  }
}

void wdGameObject::TransformationData::UpdateGlobalTransformRecursive()
{
  if (m_pParentData != nullptr)
  {
    m_pParentData->UpdateGlobalTransformRecursive();
    UpdateGlobalTransformWithParent();
  }
  else
  {
    UpdateGlobalTransformWithoutParent();
  }
}

void wdGameObject::TransformationData::UpdateGlobalBounds(wdSpatialSystem* pSpatialSystem)
{
  if (pSpatialSystem == nullptr)
  {
    UpdateGlobalBounds();
  }
  else
  {
    UpdateGlobalBoundsAndSpatialData(*pSpatialSystem);
  }
}

void wdGameObject::TransformationData::UpdateGlobalBoundsAndSpatialData(wdSpatialSystem& ref_spatialSystem)
{
  wdSimdBBoxSphere oldGlobalBounds = m_globalBounds;

  UpdateGlobalBounds();

  const bool bIsAlwaysVisible = m_localBounds.m_BoxHalfExtents.w() != wdSimdFloat::Zero();
  if (m_hSpatialData.IsInvalidated() == false && bIsAlwaysVisible == false && m_globalBounds != oldGlobalBounds)
  {
    ref_spatialSystem.UpdateSpatialDataBounds(m_hSpatialData, m_globalBounds);
  }
}

void wdGameObject::TransformationData::RecreateSpatialData(wdSpatialSystem& ref_spatialSystem)
{
  if (m_hSpatialData.IsInvalidated() == false)
  {
    ref_spatialSystem.DeleteSpatialData(m_hSpatialData);
    m_hSpatialData.Invalidate();
  }

  const bool bIsAlwaysVisible = m_localBounds.m_BoxHalfExtents.w() != wdSimdFloat::Zero();
  if (bIsAlwaysVisible)
  {
    m_hSpatialData = ref_spatialSystem.CreateSpatialDataAlwaysVisible(m_pObject, m_uiSpatialDataCategoryBitmask, m_pObject->m_Tags);
  }
  else if (m_localBounds.IsValid())
  {
    UpdateGlobalBounds();
    m_hSpatialData = ref_spatialSystem.CreateSpatialData(m_globalBounds, m_pObject, m_uiSpatialDataCategoryBitmask, m_pObject->m_Tags);
  }
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_GameObject);
