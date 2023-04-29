#include <Core/CorePCH.h>

#include <Core/Collection/CollectionComponent.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>

// clang-format off
WD_BEGIN_COMPONENT_TYPE(wdCollectionComponent, 1, wdComponentMode::Static)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("Collection", GetCollectionFile, SetCollectionFile)->AddAttributes(new wdAssetBrowserAttribute("CompatibleAsset_AssetCollection", wdDependencyFlags::Package)),
  }
  WD_END_PROPERTIES;
  WD_BEGIN_ATTRIBUTES
  {
    new wdCategoryAttribute("General"),
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdCollectionComponent::wdCollectionComponent() = default;
wdCollectionComponent::~wdCollectionComponent() = default;

void wdCollectionComponent::SerializeComponent(wdWorldWriter& inout_stream) const
{
  SUPER::SerializeComponent(inout_stream);
  auto& s = inout_stream.GetStream();

  s << m_hCollection;
}

void wdCollectionComponent::DeserializeComponent(wdWorldReader& inout_stream)
{
  SUPER::DeserializeComponent(inout_stream);
  // const wdUInt32 uiVersion = stream.GetComponentTypeVersion(GetStaticRTTI());
  auto& s = inout_stream.GetStream();

  s >> m_hCollection;
}

void wdCollectionComponent::SetCollectionFile(const char* szFile)
{
  wdCollectionResourceHandle hResource;

  if (!wdStringUtils::IsNullOrEmpty(szFile))
  {
    hResource = wdResourceManager::LoadResource<wdCollectionResource>(szFile);
    wdResourceManager::PreloadResource(hResource);
  }

  SetCollection(hResource);
}

const char* wdCollectionComponent::GetCollectionFile() const
{
  if (!m_hCollection.IsValid())
    return "";

  return m_hCollection.GetResourceID();
}

void wdCollectionComponent::SetCollection(const wdCollectionResourceHandle& hCollection)
{
  m_hCollection = hCollection;

  if (IsActiveAndSimulating())
  {
    InitiatePreload();
  }
}

void wdCollectionComponent::OnSimulationStarted()
{
  InitiatePreload();
}

void wdCollectionComponent::InitiatePreload()
{
  if (m_hCollection.IsValid())
  {
    wdResourceLock<wdCollectionResource> pCollection(m_hCollection, wdResourceAcquireMode::BlockTillLoaded_NeverFail);

    if (pCollection.GetAcquireResult() == wdResourceAcquireResult::Final)
    {
      pCollection->PreloadResources();
    }
  }
}

WD_STATICLINK_FILE(Core, Core_Collection_Implementation_CollectionComponent);
