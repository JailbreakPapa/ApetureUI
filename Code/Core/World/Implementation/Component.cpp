#include <Core/CorePCH.h>

#include <Core/World/World.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdComponent, 1, wdRTTINoAllocator)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Active", GetActiveFlag, SetActiveFlag)->AddAttributes(new wdDefaultValueAttribute(true)),
  }
  WD_END_PROPERTIES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

void wdComponent::SetActiveFlag(bool bEnabled)
{
  if (m_ComponentFlags.IsSet(wdObjectFlags::ActiveFlag) != bEnabled)
  {
    m_ComponentFlags.AddOrRemove(wdObjectFlags::ActiveFlag, bEnabled);

    UpdateActiveState(GetOwner() == nullptr ? true : GetOwner()->IsActive());
  }
}

void wdComponent::UpdateActiveState(bool bOwnerActive)
{
  const bool bSelfActive = bOwnerActive && m_ComponentFlags.IsSet(wdObjectFlags::ActiveFlag);

  if (m_ComponentFlags.IsSet(wdObjectFlags::ActiveState) != bSelfActive)
  {
    m_ComponentFlags.AddOrRemove(wdObjectFlags::ActiveState, bSelfActive);

    if (IsInitialized())
    {
      if (bSelfActive)
      {
        // Don't call OnActivated & EnsureSimulationStarted here since there might be other components
        // that are needed in the OnSimulation callback but are activated right after this component.
        // Instead add the component to the initialization batch again.
        // There initialization will be skipped since the component is already initialized.
        GetWorld()->AddComponentToInitialize(GetHandle());
      }
      else
      {
        OnDeactivated();

        m_ComponentFlags.Remove(wdObjectFlags::SimulationStarted);
      }
    }
  }
}

wdWorld* wdComponent::GetWorld()
{
  return m_pManager->GetWorld();
}

const wdWorld* wdComponent::GetWorld() const
{
  return m_pManager->GetWorld();
}

void wdComponent::SerializeComponent(wdWorldWriter& inout_stream) const {}

void wdComponent::DeserializeComponent(wdWorldReader& inout_stream) {}

void wdComponent::EnsureInitialized()
{
  WD_ASSERT_DEV(m_pOwner != nullptr, "Owner must not be null");

  if (IsInitializing())
  {
    wdLog::Error("Recursive initialize call is ignored.");
    return;
  }

  if (!IsInitialized())
  {
    m_pMessageDispatchType = GetDynamicRTTI();

    m_ComponentFlags.Add(wdObjectFlags::Initializing);

    Initialize();

    m_ComponentFlags.Remove(wdObjectFlags::Initializing);
    m_ComponentFlags.Add(wdObjectFlags::Initialized);
  }
}

void wdComponent::EnsureSimulationStarted()
{
  WD_ASSERT_DEV(IsActiveAndInitialized(), "Must not be called on uninitialized or inactive components.");
  WD_ASSERT_DEV(GetWorld()->GetWorldSimulationEnabled(), "Must not be called when the world is not simulated.");

  if (m_ComponentFlags.IsSet(wdObjectFlags::SimulationStarting))
  {
    wdLog::Error("Recursive simulation started call is ignored.");
    return;
  }

  if (!IsSimulationStarted())
  {
    m_ComponentFlags.Add(wdObjectFlags::SimulationStarting);

    OnSimulationStarted();

    m_ComponentFlags.Remove(wdObjectFlags::SimulationStarting);
    m_ComponentFlags.Add(wdObjectFlags::SimulationStarted);
  }
}

bool wdComponent::SendMessageInternal(wdMessage& msg, bool bWasPostedMsg)
{
  if (!IsActiveAndInitialized() && !IsInitializing())
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (msg.GetDebugMessageRouting())
      wdLog::Warning("Discarded message with ID {0} because component of type '{1}' is neither initialized nor active at the moment", msg.GetId(),
        GetDynamicRTTI()->GetTypeName());
#endif

    return false;
  }

  if (m_pMessageDispatchType->DispatchMessage(this, msg))
    return true;

  if (m_ComponentFlags.IsSet(wdObjectFlags::UnhandledMessageHandler) && OnUnhandledMessage(msg, bWasPostedMsg))
    return true;

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  if (msg.GetDebugMessageRouting())
    wdLog::Warning("Component type '{0}' does not have a message handler for messages of type {1}", GetDynamicRTTI()->GetTypeName(), msg.GetId());
#endif

  return false;
}

bool wdComponent::SendMessageInternal(wdMessage& msg, bool bWasPostedMsg) const
{
  if (!IsActiveAndInitialized() && !IsInitializing())
  {
#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
    if (msg.GetDebugMessageRouting())
      wdLog::Warning("Discarded message with ID {0} because component of type '{1}' is neither initialized nor active at the moment", msg.GetId(),
        GetDynamicRTTI()->GetTypeName());
#endif

    return false;
  }

  if (m_pMessageDispatchType->DispatchMessage(this, msg))
    return true;

  if (m_ComponentFlags.IsSet(wdObjectFlags::UnhandledMessageHandler) && OnUnhandledMessage(msg, bWasPostedMsg))
    return true;

#if WD_ENABLED(WD_COMPILE_FOR_DEBUG)
  if (msg.GetDebugMessageRouting())
    wdLog::Warning(
      "(const) Component type '{0}' does not have a CONST message handler for messages of type {1}", GetDynamicRTTI()->GetTypeName(), msg.GetId());
#endif

  return false;
}

void wdComponent::PostMessage(const wdMessage& msg, wdTime delay, wdObjectMsgQueueType::Enum queueType) const
{
  GetWorld()->PostMessage(GetHandle(), msg, delay, queueType);
}

bool wdComponent::HandlesMessage(const wdMessage& msg) const
{
  return m_pMessageDispatchType->CanHandleMessage(msg.GetId());
}

void wdComponent::Initialize() {}

void wdComponent::Deinitialize()
{
  WD_ASSERT_DEV(m_pOwner != nullptr, "Owner must still be valid");

  SetActiveFlag(false);
}

void wdComponent::OnActivated() {}

void wdComponent::OnDeactivated() {}

void wdComponent::OnSimulationStarted() {}

void wdComponent::EnableUnhandledMessageHandler(bool enable)
{
  m_ComponentFlags.AddOrRemove(wdObjectFlags::UnhandledMessageHandler, enable);
}

bool wdComponent::OnUnhandledMessage(wdMessage& msg, bool bWasPostedMsg)
{
  return false;
}

bool wdComponent::OnUnhandledMessage(wdMessage& msg, bool bWasPostedMsg) const
{
  return false;
}

void wdComponent::SetUserFlag(wdUInt8 uiFlagIndex, bool bSet)
{
  WD_ASSERT_DEBUG(uiFlagIndex < 8, "Flag index {0} is out of the valid range [0 - 7]", uiFlagIndex);

  m_ComponentFlags.AddOrRemove(static_cast<wdObjectFlags::Enum>(wdObjectFlags::UserFlag0 << uiFlagIndex), bSet);
}

bool wdComponent::GetUserFlag(wdUInt8 uiFlagIndex) const
{
  WD_ASSERT_DEBUG(uiFlagIndex < 8, "Flag index {0} is out of the valid range [0 - 7]", uiFlagIndex);

  return m_ComponentFlags.IsSet(static_cast<wdObjectFlags::Enum>(wdObjectFlags::UserFlag0 << uiFlagIndex));
}


WD_STATICLINK_FILE(Core, Core_World_Implementation_Component);
