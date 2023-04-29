#include <Foundation/Logging/Log.h>

WD_ALWAYS_INLINE wdComponent::wdComponent()
  : m_pMessageDispatchType(nullptr)
  , m_ComponentFlags(wdObjectFlags::ActiveFlag)
  , m_pManager(nullptr)
  , m_pOwner(nullptr)
{
  m_uiUniqueID = wdInvalidIndex;
}

WD_ALWAYS_INLINE wdComponent::~wdComponent()
{
  m_pMessageDispatchType = nullptr;
  m_pManager = nullptr;
  m_pOwner = nullptr;
  m_InternalId.Invalidate();
}

WD_ALWAYS_INLINE bool wdComponent::IsDynamic() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::Dynamic);
}

WD_ALWAYS_INLINE bool wdComponent::GetActiveFlag() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::ActiveFlag);
}

WD_ALWAYS_INLINE bool wdComponent::IsActive() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::ActiveState);
}

WD_ALWAYS_INLINE bool wdComponent::IsActiveAndInitialized() const
{
  return m_ComponentFlags.AreAllSet(wdObjectFlags::ActiveState | wdObjectFlags::Initialized);
}

WD_ALWAYS_INLINE wdComponentManagerBase* wdComponent::GetOwningManager()
{
  return m_pManager;
}

WD_ALWAYS_INLINE const wdComponentManagerBase* wdComponent::GetOwningManager() const
{
  return m_pManager;
}

WD_ALWAYS_INLINE wdGameObject* wdComponent::GetOwner()
{
  return m_pOwner;
}

WD_ALWAYS_INLINE const wdGameObject* wdComponent::GetOwner() const
{
  return m_pOwner;
}

WD_ALWAYS_INLINE wdComponentHandle wdComponent::GetHandle() const
{
  return wdComponentHandle(m_InternalId);
}

WD_ALWAYS_INLINE wdUInt32 wdComponent::GetUniqueID() const
{
  return m_uiUniqueID;
}

WD_ALWAYS_INLINE void wdComponent::SetUniqueID(wdUInt32 uiUniqueID)
{
  m_uiUniqueID = uiUniqueID;
}

WD_ALWAYS_INLINE bool wdComponent::IsInitialized() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::Initialized);
}

WD_ALWAYS_INLINE bool wdComponent::IsInitializing() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::Initializing);
}

WD_ALWAYS_INLINE bool wdComponent::IsSimulationStarted() const
{
  return m_ComponentFlags.IsSet(wdObjectFlags::SimulationStarted);
}

WD_ALWAYS_INLINE bool wdComponent::IsActiveAndSimulating() const
{
  return m_ComponentFlags.AreAllSet(wdObjectFlags::Initialized | wdObjectFlags::ActiveState) &&
         m_ComponentFlags.IsAnySet(wdObjectFlags::SimulationStarting | wdObjectFlags::SimulationStarted);
}
