#include <Core/CorePCH.h>

#include <Core/World/EventMessageHandlerComponent.h>
#include <Core/World/World.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>

namespace
{
  static wdStaticArray<wdDynamicArray<wdComponentHandle>*, 64> s_GlobalEventHandlerPerWorld;

  static void RegisterGlobalEventHandler(wdComponent* pComponent)
  {
    const wdUInt32 uiWorldIndex = pComponent->GetWorld()->GetIndex();
    s_GlobalEventHandlerPerWorld.EnsureCount(uiWorldIndex + 1);

    auto globalEventHandler = s_GlobalEventHandlerPerWorld[uiWorldIndex];
    if (globalEventHandler == nullptr)
    {
      globalEventHandler = WD_NEW(wdStaticAllocatorWrapper::GetAllocator(), wdDynamicArray<wdComponentHandle>);

      s_GlobalEventHandlerPerWorld[uiWorldIndex] = globalEventHandler;
    }

    globalEventHandler->PushBack(pComponent->GetHandle());
  }

  static void DeregisterGlobalEventHandler(wdComponent* pComponent)
  {
    wdUInt32 uiWorldIndex = pComponent->GetWorld()->GetIndex();
    auto globalEventHandler = s_GlobalEventHandlerPerWorld[uiWorldIndex];
    WD_ASSERT_DEV(globalEventHandler != nullptr, "Implementation error.");

    globalEventHandler->RemoveAndSwap(pComponent->GetHandle());
  }
} // namespace

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_ABSTRACT_COMPONENT_TYPE(wdEventMessageHandlerComponent, 3)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("HandleGlobalEvents", GetGlobalEventHandlerMode, SetGlobalEventHandlerMode),
    WD_ACCESSOR_PROPERTY("PassThroughUnhandledEvents", GetPassThroughUnhandledEvents, SetPassThroughUnhandledEvents),
  }
  WD_END_PROPERTIES;
}
WD_END_ABSTRACT_COMPONENT_TYPE;
// clang-format on

wdEventMessageHandlerComponent::wdEventMessageHandlerComponent() = default;
wdEventMessageHandlerComponent::~wdEventMessageHandlerComponent() = default;

void wdEventMessageHandlerComponent::SerializeComponent(wdWorldWriter& inout_stream) const
{
  SUPER::SerializeComponent(inout_stream);
  auto& s = inout_stream.GetStream();

  // version 2
  s << m_bIsGlobalEventHandler;

  // version 3
  s << m_bPassThroughUnhandledEvents;
}

void wdEventMessageHandlerComponent::DeserializeComponent(wdWorldReader& inout_stream)
{
  SUPER::DeserializeComponent(inout_stream);
  const wdUInt32 uiVersion = inout_stream.GetComponentTypeVersion(GetStaticRTTI());
  auto& s = inout_stream.GetStream();

  if (uiVersion >= 2)
  {
    bool bGlobalEH;
    s >> bGlobalEH;

    SetGlobalEventHandlerMode(bGlobalEH);
  }

  if (uiVersion >= 3)
  {
    s >> m_bPassThroughUnhandledEvents;
  }
}

void wdEventMessageHandlerComponent::Deinitialize()
{
  SetGlobalEventHandlerMode(false);

  SUPER::Deinitialize();
}

void wdEventMessageHandlerComponent::SetDebugOutput(bool bEnable)
{
  m_bDebugOutput = bEnable;
}

bool wdEventMessageHandlerComponent::GetDebugOutput() const
{
  return m_bDebugOutput;
}

void wdEventMessageHandlerComponent::SetGlobalEventHandlerMode(bool bEnable)
{
  if (m_bIsGlobalEventHandler == bEnable)
    return;

  m_bIsGlobalEventHandler = bEnable;

  if (bEnable)
  {
    RegisterGlobalEventHandler(this);
  }
  else
  {
    DeregisterGlobalEventHandler(this);
  }
}

void wdEventMessageHandlerComponent::SetPassThroughUnhandledEvents(bool bPassThrough)
{
  m_bPassThroughUnhandledEvents = bPassThrough;
}

// static
wdArrayPtr<wdComponentHandle> wdEventMessageHandlerComponent::GetAllGlobalEventHandler(const wdWorld* pWorld)
{
  wdUInt32 uiWorldIndex = pWorld->GetIndex();

  if (uiWorldIndex < s_GlobalEventHandlerPerWorld.GetCount())
  {
    if (auto globalEventHandler = s_GlobalEventHandlerPerWorld[uiWorldIndex])
    {
      return globalEventHandler->GetArrayPtr();
    }
  }

  return wdArrayPtr<wdComponentHandle>();
}


void wdEventMessageHandlerComponent::ClearGlobalEventHandlersForWorld(const wdWorld* pWorld)
{
  wdUInt32 uiWorldIndex = pWorld->GetIndex();

  if (uiWorldIndex < s_GlobalEventHandlerPerWorld.GetCount())
  {
    s_GlobalEventHandlerPerWorld[uiWorldIndex]->Clear();
  }
}

WD_STATICLINK_FILE(Core, Core_World_Implementation_EventMessageHandlerComponent);
