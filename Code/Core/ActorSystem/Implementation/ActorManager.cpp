#include <Core/CorePCH.h>

#include <Core/ActorSystem/Actor.h>
#include <Core/ActorSystem/ActorApiService.h>
#include <Core/ActorSystem/ActorManager.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Types/ScopeExit.h>
#include <Foundation/Types/UniquePtr.h>

//////////////////////////////////////////////////////////////////////////

static wdUniquePtr<wdActorManager> s_pActorManager;

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, wdActorManager)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    s_pActorManager = WD_DEFAULT_NEW(wdActorManager);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    s_pActorManager.Clear();
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
    if (s_pActorManager)
    {
      s_pActorManager->DestroyAllActors(nullptr, wdActorManager::DestructionMode::Immediate);
    }
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on


//////////////////////////////////////////////////////////////////////////

struct wdActorManagerImpl
{
  wdMutex m_Mutex;
  wdHybridArray<wdUniquePtr<wdActor>, 8> m_AllActors;
  wdHybridArray<wdUniquePtr<wdActorApiService>, 8> m_AllApiServices;
};

//////////////////////////////////////////////////////////////////////////

WD_IMPLEMENT_SINGLETON(wdActorManager);

wdCopyOnBroadcastEvent<const wdActorEvent&> wdActorManager::s_ActorEvents;

wdActorManager::wdActorManager()
  : m_SingletonRegistrar(this)
{
  m_pImpl = WD_DEFAULT_NEW(wdActorManagerImpl);
}

wdActorManager::~wdActorManager()
{
  Shutdown();
}

void wdActorManager::Shutdown()
{
  WD_LOCK(m_pImpl->m_Mutex);

  DestroyAllActors(nullptr, DestructionMode::Immediate);
  DestroyAllApiServices();

  s_ActorEvents.Clear();
}

void wdActorManager::AddActor(wdUniquePtr<wdActor>&& pActor)
{
  WD_LOCK(m_pImpl->m_Mutex);

  WD_ASSERT_DEV(pActor != nullptr, "Actor must exist to be added.");
  m_pImpl->m_AllActors.PushBack(std::move(pActor));

  wdActorEvent e;
  e.m_Type = wdActorEvent::Type::AfterActorCreation;
  e.m_pActor = m_pImpl->m_AllActors.PeekBack().Borrow();
  s_ActorEvents.Broadcast(e);
}

void wdActorManager::DestroyActor(wdActor* pActor, DestructionMode mode)
{
  WD_LOCK(m_pImpl->m_Mutex);

  pActor->m_State = wdActor::State::QueuedForDestruction;

  if (mode == DestructionMode::Immediate && m_bForceQueueActorDestruction == false)
  {
    for (wdUInt32 i = 0; i < m_pImpl->m_AllActors.GetCount(); ++i)
    {
      if (m_pImpl->m_AllActors[i] == pActor)
      {
        wdActorEvent e;
        e.m_Type = wdActorEvent::Type::BeforeActorDestruction;
        e.m_pActor = pActor;
        s_ActorEvents.Broadcast(e);

        m_pImpl->m_AllActors.RemoveAtAndCopy(i);
        break;
      }
    }
  }
}

void wdActorManager::DestroyAllActors(const void* pCreatedBy, DestructionMode mode)
{
  WD_LOCK(m_pImpl->m_Mutex);

  for (wdUInt32 i0 = m_pImpl->m_AllActors.GetCount(); i0 > 0; --i0)
  {
    const wdUInt32 i = i0 - 1;
    wdActor* pActor = m_pImpl->m_AllActors[i].Borrow();

    if (pCreatedBy == nullptr || pActor->GetCreatedBy() == pCreatedBy)
    {
      pActor->m_State = wdActor::State::QueuedForDestruction;

      if (mode == DestructionMode::Immediate && m_bForceQueueActorDestruction == false)
      {
        wdActorEvent e;
        e.m_Type = wdActorEvent::Type::BeforeActorDestruction;
        e.m_pActor = pActor;
        s_ActorEvents.Broadcast(e);

        m_pImpl->m_AllActors.RemoveAtAndCopy(i);
      }
    }
  }
}

void wdActorManager::GetAllActors(wdHybridArray<wdActor*, 8>& out_allActors)
{
  WD_LOCK(m_pImpl->m_Mutex);

  out_allActors.Clear();

  for (auto& pActor : m_pImpl->m_AllActors)
  {
    out_allActors.PushBack(pActor.Borrow());
  }
}

void wdActorManager::AddApiService(wdUniquePtr<wdActorApiService>&& pApiService)
{
  WD_LOCK(m_pImpl->m_Mutex);

  WD_ASSERT_DEV(pApiService != nullptr, "Invalid API service");
  WD_ASSERT_DEV(pApiService->m_State == wdActorApiService::State::New, "Actor API service already in use");

  for (auto& pExisting : m_pImpl->m_AllApiServices)
  {
    WD_ASSERT_ALWAYS(pApiService->GetDynamicRTTI() != pExisting->GetDynamicRTTI() || pExisting->m_State == wdActorApiService::State::QueuedForDestruction, "An actor API service of this type has already been added");
  }

  m_pImpl->m_AllApiServices.PushBack(std::move(pApiService));
}

void wdActorManager::DestroyApiService(wdActorApiService* pApiService, DestructionMode mode /*= DestructionMode::Immediate*/)
{
  WD_LOCK(m_pImpl->m_Mutex);

  WD_ASSERT_DEV(pApiService != nullptr, "Invalid API service");

  pApiService->m_State = wdActorApiService::State::QueuedForDestruction;

  if (mode == DestructionMode::Immediate)
  {
    for (wdUInt32 i = 0; i < m_pImpl->m_AllApiServices.GetCount(); ++i)
    {
      if (m_pImpl->m_AllApiServices[i] == pApiService)
      {
        m_pImpl->m_AllApiServices.RemoveAtAndCopy(i);
        break;
      }
    }
  }
}

void wdActorManager::DestroyAllApiServices(DestructionMode mode /*= DestructionMode::Immediate*/)
{
  WD_LOCK(m_pImpl->m_Mutex);

  for (wdUInt32 i0 = m_pImpl->m_AllApiServices.GetCount(); i0 > 0; --i0)
  {
    const wdUInt32 i = i0 - 1;
    wdActorApiService* pApiService = m_pImpl->m_AllApiServices[i].Borrow();

    pApiService->m_State = wdActorApiService::State::QueuedForDestruction;

    if (mode == DestructionMode::Immediate)
    {
      m_pImpl->m_AllApiServices.RemoveAtAndCopy(i);
    }
  }
}

void wdActorManager::ActivateQueuedApiServices()
{
  WD_LOCK(m_pImpl->m_Mutex);

  for (auto& pManager : m_pImpl->m_AllApiServices)
  {
    if (pManager->m_State == wdActorApiService::State::New)
    {
      pManager->Activate();
      pManager->m_State = wdActorApiService::State::Active;
    }
  }
}

wdActorApiService* wdActorManager::GetApiService(const wdRTTI* pType)
{
  WD_LOCK(m_pImpl->m_Mutex);

  WD_ASSERT_DEV(pType->IsDerivedFrom<wdActorApiService>(), "The queried type has to derive from wdActorApiService");

  for (auto& pApiService : m_pImpl->m_AllApiServices)
  {
    if (pApiService->GetDynamicRTTI()->IsDerivedFrom(pType) && pApiService->m_State != wdActorApiService::State::QueuedForDestruction)
      return pApiService.Borrow();
  }

  return nullptr;
}

void wdActorManager::UpdateAllApiServices()
{
  WD_LOCK(m_pImpl->m_Mutex);

  for (auto& pApiService : m_pImpl->m_AllApiServices)
  {
    if (pApiService->m_State == wdActorApiService::State::Active)
    {
      pApiService->Update();
    }
  }
}

void wdActorManager::UpdateAllActors()
{
  WD_LOCK(m_pImpl->m_Mutex);

  m_bForceQueueActorDestruction = true;
  WD_SCOPE_EXIT(m_bForceQueueActorDestruction = false);

  for (wdUInt32 i0 = m_pImpl->m_AllActors.GetCount(); i0 > 0; --i0)
  {
    const wdUInt32 i = i0 - 1;
    wdActor* pActor = m_pImpl->m_AllActors[i].Borrow();

    if (pActor->m_State == wdActor::State::New)
    {
      pActor->m_State = wdActor::State::Active;

      pActor->Activate();

      wdActorEvent e;
      e.m_Type = wdActorEvent::Type::AfterActorActivation;
      e.m_pActor = pActor;
      s_ActorEvents.Broadcast(e);
    }

    if (pActor->m_State == wdActor::State::Active)
    {
      pActor->Update();
    }
  }
}

void wdActorManager::DestroyQueuedActors()
{
  WD_LOCK(m_pImpl->m_Mutex);

  WD_ASSERT_DEV(!m_bForceQueueActorDestruction, "Cannot execute this function right now");

  for (wdUInt32 i0 = m_pImpl->m_AllActors.GetCount(); i0 > 0; --i0)
  {
    const wdUInt32 i = i0 - 1;
    wdActor* pActor = m_pImpl->m_AllActors[i].Borrow();

    if (pActor->m_State == wdActor::State::QueuedForDestruction)
    {
      wdActorEvent e;
      e.m_Type = wdActorEvent::Type::BeforeActorDestruction;
      e.m_pActor = pActor;
      s_ActorEvents.Broadcast(e);

      m_pImpl->m_AllActors.RemoveAtAndCopy(i);
    }
  }
}

void wdActorManager::DestroyQueuedActorApiServices()
{
  WD_LOCK(m_pImpl->m_Mutex);

  for (wdUInt32 i0 = m_pImpl->m_AllApiServices.GetCount(); i0 > 0; --i0)
  {
    const wdUInt32 i = i0 - 1;
    wdActorApiService* pApiService = m_pImpl->m_AllApiServices[i].Borrow();

    if (pApiService->m_State == wdActorApiService::State::QueuedForDestruction)
    {
      m_pImpl->m_AllApiServices.RemoveAtAndCopy(i);
    }
  }
}

void wdActorManager::Update()
{
  WD_LOCK(m_pImpl->m_Mutex);

  DestroyQueuedActorApiServices();
  DestroyQueuedActors();
  ActivateQueuedApiServices();
  UpdateAllApiServices();
  UpdateAllActors();
}


WD_STATICLINK_FILE(Core, Core_ActorSystem_Implementation_ActorManager);
