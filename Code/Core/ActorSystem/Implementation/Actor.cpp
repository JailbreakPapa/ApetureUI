#include <Core/CorePCH.h>

#include <Core/ActorSystem/Actor.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActor, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

struct wdActorImpl
{
  wdString m_sName;
  const void* m_pCreatedBy = nullptr;
  wdHybridArray<wdUniquePtr<wdActorPlugin>, 4> m_AllPlugins;
  wdMap<const wdRTTI*, wdActorPlugin*> m_PluginLookupCache;
};


wdActor::wdActor(wdStringView sActorName, const void* pCreatedBy)
{
  m_pImpl = WD_DEFAULT_NEW(wdActorImpl);

  m_pImpl->m_sName = sActorName;
  m_pImpl->m_pCreatedBy = pCreatedBy;

  WD_ASSERT_DEV(!m_pImpl->m_sName.IsEmpty(), "Actor name must not be empty");
}

wdActor::~wdActor() = default;

wdStringView wdActor::GetName() const
{
  return m_pImpl->m_sName;
}

const void* wdActor::GetCreatedBy() const
{
  return m_pImpl->m_pCreatedBy;
}

void wdActor::AddPlugin(wdUniquePtr<wdActorPlugin>&& pPlugin)
{
  WD_ASSERT_DEV(pPlugin != nullptr, "Invalid actor plugin");
  WD_ASSERT_DEV(pPlugin->m_pOwningActor == nullptr, "Actor plugin already in use");

  pPlugin->m_pOwningActor = this;

  // register this plugin under its type and all its base types
  for (const wdRTTI* pRtti = pPlugin->GetDynamicRTTI(); pRtti != wdGetStaticRTTI<wdActorPlugin>(); pRtti = pRtti->GetParentType())
  {
    m_pImpl->m_PluginLookupCache[pRtti] = pPlugin.Borrow();
  }

  m_pImpl->m_AllPlugins.PushBack(std::move(pPlugin));
}

wdActorPlugin* wdActor::GetPlugin(const wdRTTI* pPluginType) const
{
  WD_ASSERT_DEV(pPluginType->IsDerivedFrom<wdActorPlugin>(), "The queried type has to derive from wdActorPlugin");

  return m_pImpl->m_PluginLookupCache.GetValueOrDefault(pPluginType, nullptr);
}

void wdActor::DestroyPlugin(wdActorPlugin* pPlugin)
{
  for (wdUInt32 i = 0; i < m_pImpl->m_AllPlugins.GetCount(); ++i)
  {
    if (m_pImpl->m_AllPlugins[i] == pPlugin)
    {
      m_pImpl->m_AllPlugins.RemoveAtAndSwap(i);
      break;
    }
  }
}

void wdActor::GetAllPlugins(wdHybridArray<wdActorPlugin*, 8>& out_allPlugins)
{
  out_allPlugins.Clear();

  for (auto& pPlugin : m_pImpl->m_AllPlugins)
  {
    out_allPlugins.PushBack(pPlugin.Borrow());
  }
}

void wdActor::UpdateAllPlugins()
{
  for (auto& pPlugin : m_pImpl->m_AllPlugins)
  {
    pPlugin->Update();
  }
}

void wdActor::Activate() {}

void wdActor::Update()
{
  UpdateAllPlugins();
}


WD_STATICLINK_FILE(Core, Core_ActorSystem_Implementation_Actor);
