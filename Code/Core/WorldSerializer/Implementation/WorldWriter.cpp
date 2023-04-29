#include <Core/CorePCH.h>

#include <Core/WorldSerializer/WorldWriter.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/StringDeduplicationContext.h>

void wdWorldWriter::Clear()
{
  m_AllRootObjects.Clear();
  m_AllChildObjects.Clear();
  m_AllComponents.Clear();

  m_pStream = nullptr;
  m_pExclude = nullptr;

  // invalid handles
  {
    m_WrittenGameObjectHandles.Clear();
    m_WrittenGameObjectHandles[wdGameObjectHandle()] = 0;
  }
}

void wdWorldWriter::WriteWorld(wdStreamWriter& inout_stream, wdWorld& ref_world, const wdTagSet* pExclude)
{
  Clear();

  m_pStream = &inout_stream;
  m_pExclude = pExclude;

  WD_LOCK(ref_world.GetReadMarker());

  ref_world.Traverse(wdMakeDelegate(&wdWorldWriter::ObjectTraverser, this), wdWorld::TraversalMethod::DepthFirst);

  WriteToStream().IgnoreResult();
}

void wdWorldWriter::WriteObjects(wdStreamWriter& inout_stream, const wdDeque<const wdGameObject*>& rootObjects)
{
  Clear();

  m_pStream = &inout_stream;

  for (const wdGameObject* pObject : rootObjects)
  {
    // traversal function takes a non-const object, but we only read it anyway
    Traverse(const_cast<wdGameObject*>(pObject));
  }

  WriteToStream().IgnoreResult();
}

void wdWorldWriter::WriteObjects(wdStreamWriter& inout_stream, wdArrayPtr<const wdGameObject*> rootObjects)
{
  Clear();

  m_pStream = &inout_stream;

  for (const wdGameObject* pObject : rootObjects)
  {
    // traversal function takes a non-const object, but we only read it anyway
    Traverse(const_cast<wdGameObject*>(pObject));
  }

  WriteToStream().IgnoreResult();
}

wdResult wdWorldWriter::WriteToStream()
{
  const wdUInt8 uiVersion = 10;
  *m_pStream << uiVersion;

  // version 8: use string dedup instead of handle writer
  wdStringDeduplicationWriteContext stringDedupWriteContext(*m_pStream);
  m_pStream = &stringDedupWriteContext.Begin();

  IncludeAllComponentBaseTypes();

  wdUInt32 uiNumRootObjects = m_AllRootObjects.GetCount();
  wdUInt32 uiNumChildObjects = m_AllChildObjects.GetCount();
  wdUInt32 uiNumComponentTypes = m_AllComponents.GetCount();

  *m_pStream << uiNumRootObjects;
  *m_pStream << uiNumChildObjects;
  *m_pStream << uiNumComponentTypes;

  // this is used to sort all component types by name, to make the file serialization deterministic
  wdMap<wdString, const wdRTTI*> sortedTypes;

  for (auto it = m_AllComponents.GetIterator(); it.IsValid(); ++it)
  {
    sortedTypes[it.Key()->GetTypeName()] = it.Key();
  }

  AssignGameObjectIndices();
  AssignComponentHandleIndices(sortedTypes);

  for (const auto* pObject : m_AllRootObjects)
  {
    WriteGameObject(pObject);
  }

  for (const auto* pObject : m_AllChildObjects)
  {
    WriteGameObject(pObject);
  }

  for (auto it = sortedTypes.GetIterator(); it.IsValid(); ++it)
  {
    WriteComponentTypeInfo(it.Value());
  }

  for (auto it = sortedTypes.GetIterator(); it.IsValid(); ++it)
  {
    WriteComponentCreationData(m_AllComponents[it.Value()].m_Components);
  }

  for (auto it = sortedTypes.GetIterator(); it.IsValid(); ++it)
  {
    WriteComponentSerializationData(m_AllComponents[it.Value()].m_Components);
  }

  WD_SUCCEED_OR_RETURN(stringDedupWriteContext.End());
  m_pStream = &stringDedupWriteContext.GetOriginalStream();

  return WD_SUCCESS;
}


void wdWorldWriter::AssignGameObjectIndices()
{
  wdUInt32 uiGameObjectIndex = 1;
  for (const auto* pObject : m_AllRootObjects)
  {
    m_WrittenGameObjectHandles[pObject->GetHandle()] = uiGameObjectIndex;
    ++uiGameObjectIndex;
  }

  for (const auto* pObject : m_AllChildObjects)
  {
    m_WrittenGameObjectHandles[pObject->GetHandle()] = uiGameObjectIndex;
    ++uiGameObjectIndex;
  }
}

void wdWorldWriter::AssignComponentHandleIndices(const wdMap<wdString, const wdRTTI*>& sortedTypes)
{
  wdUInt16 uiTypeIndex = 0;

  WD_ASSERT_DEV(m_AllComponents.GetCount() <= wdMath::MaxValue<wdUInt16>(), "Too many types for world writer");

  // assign the component handle indices in the order in which the components are written
  for (auto it = sortedTypes.GetIterator(); it.IsValid(); ++it)
  {
    auto& components = m_AllComponents[it.Value()];

    components.m_uiSerializedTypeIndex = uiTypeIndex;
    ++uiTypeIndex;

    wdUInt32 uiComponentIndex = 1;
    components.m_HandleToIndex[wdComponentHandle()] = 0;

    for (const wdComponent* pComp : components.m_Components)
    {
      components.m_HandleToIndex[pComp->GetHandle()] = uiComponentIndex;
      ++uiComponentIndex;
    }
  }
}


void wdWorldWriter::IncludeAllComponentBaseTypes()
{
  wdDynamicArray<const wdRTTI*> allNow;
  allNow.Reserve(m_AllComponents.GetCount());
  for (auto it = m_AllComponents.GetIterator(); it.IsValid(); ++it)
  {
    allNow.PushBack(it.Key());
  }

  for (auto pRtti : allNow)
  {
    IncludeAllComponentBaseTypes(pRtti->GetParentType());
  }
}


void wdWorldWriter::IncludeAllComponentBaseTypes(const wdRTTI* pRtti)
{
  if (pRtti == nullptr || !pRtti->IsDerivedFrom<wdComponent>() || m_AllComponents.Contains(pRtti))
    return;

  // this is actually used to insert the type, but we have no component of this type
  m_AllComponents[pRtti];

  IncludeAllComponentBaseTypes(pRtti->GetParentType());
}


void wdWorldWriter::Traverse(wdGameObject* pObject)
{
  if (ObjectTraverser(pObject) == wdVisitorExecution::Continue)
  {
    for (auto it = pObject->GetChildren(); it.IsValid(); ++it)
    {
      Traverse(&(*it));
    }
  }
}

void wdWorldWriter::WriteGameObjectHandle(const wdGameObjectHandle& hObject)
{
  auto it = m_WrittenGameObjectHandles.Find(hObject);

  wdUInt32 uiIndex = 0;

  WD_ASSERT_DEV(it.IsValid(), "Referenced object does not exist in the scene. This can happen, if it was optimized away, because it had no name, no children and no essential components.");

  if (it.IsValid())
    uiIndex = it.Value();

  *m_pStream << uiIndex;
}

void wdWorldWriter::WriteComponentHandle(const wdComponentHandle& hComponent)
{
  wdUInt16 uiTypeIndex = 0;
  wdUInt32 uiIndex = 0;

  wdComponent* pComponent = nullptr;
  if (wdWorld::GetWorld(hComponent)->TryGetComponent(hComponent, pComponent))
  {
    if (auto* components = m_AllComponents.GetValue(pComponent->GetDynamicRTTI()))
    {
      auto it = components->m_HandleToIndex.Find(hComponent);
      WD_ASSERT_DEBUG(it.IsValid(), "Handle should always be in the written map at this point");

      if (it.IsValid())
      {
        uiTypeIndex = components->m_uiSerializedTypeIndex;
        uiIndex = it.Value();
      }
    }
  }

  *m_pStream << uiTypeIndex;
  *m_pStream << uiIndex;
}

wdVisitorExecution::Enum wdWorldWriter::ObjectTraverser(wdGameObject* pObject)
{
  if (m_pExclude && pObject->GetTags().IsAnySet(*m_pExclude))
    return wdVisitorExecution::Skip;

  if (pObject->GetParent())
    m_AllChildObjects.PushBack(pObject);
  else
    m_AllRootObjects.PushBack(pObject);

  auto components = pObject->GetComponents();

  for (const wdComponent* pComp : components)
  {
    m_AllComponents[pComp->GetDynamicRTTI()].m_Components.PushBack(pComp);
  }

  return wdVisitorExecution::Continue;
}

void wdWorldWriter::WriteGameObject(const wdGameObject* pObject)
{
  if (pObject->GetParent())
    WriteGameObjectHandle(pObject->GetParent()->GetHandle());
  else
    WriteGameObjectHandle(wdGameObjectHandle());

  wdStreamWriter& s = *m_pStream;

  s << pObject->GetName();
  s << pObject->GetGlobalKey();
  s << pObject->GetLocalPosition();
  s << pObject->GetLocalRotation();
  s << pObject->GetLocalScaling();
  s << pObject->GetLocalUniformScaling();
  s << pObject->GetActiveFlag();
  s << pObject->IsDynamic();
  pObject->GetTags().Save(s);
  s << pObject->GetTeamID();
  s << pObject->GetStableRandomSeed();
}

void wdWorldWriter::WriteComponentTypeInfo(const wdRTTI* pRtti)
{
  wdStreamWriter& s = *m_pStream;

  s << pRtti->GetTypeName();
  s << pRtti->GetTypeVersion();
}

void wdWorldWriter::WriteComponentCreationData(const wdDeque<const wdComponent*>& components)
{
  wdDefaultMemoryStreamStorage storage;
  wdMemoryStreamWriter memWriter(&storage);

  wdStreamWriter* pPrevStream = m_pStream;
  m_pStream = &memWriter;

  // write to memory stream
  {
    wdStreamWriter& s = *m_pStream;
    s << components.GetCount();

    wdUInt32 uiComponentIndex = 1;
    for (auto pComponent : components)
    {
      WriteGameObjectHandle(pComponent->GetOwner()->GetHandle());
      s << uiComponentIndex;
      ++uiComponentIndex;

      s << pComponent->GetActiveFlag();

      // version 7
      {
        wdUInt8 userFlags = 0;
        for (wdUInt8 i = 0; i < 8; ++i)
        {
          userFlags |= pComponent->GetUserFlag(i) ? WD_BIT(i) : 0;
        }

        s << userFlags;
      }
    }
  }

  m_pStream = pPrevStream;

  // write result to actual stream
  {
    wdStreamWriter& s = *m_pStream;
    s << storage.GetStorageSize32();

    WD_ASSERT_ALWAYS(storage.GetStorageSize64() <= wdMath::MaxValue<wdUInt32>(), "Slight file format change and version increase needed to support > 4GB worlds.");

    storage.CopyToStream(s).IgnoreResult();
  }
}

void wdWorldWriter::WriteComponentSerializationData(const wdDeque<const wdComponent*>& components)
{
  wdDefaultMemoryStreamStorage storage;
  wdMemoryStreamWriter memWriter(&storage);

  wdStreamWriter* pPrevStream = m_pStream;
  m_pStream = &memWriter;

  // write to memory stream
  for (auto pComp : components)
  {
    pComp->SerializeComponent(*this);
  }

  m_pStream = pPrevStream;

  // write result to actual stream
  {
    wdStreamWriter& s = *m_pStream;
    s << storage.GetStorageSize32();

    WD_ASSERT_ALWAYS(storage.GetStorageSize64() <= wdMath::MaxValue<wdUInt32>(), "Slight file format change and version increase needed to support > 4GB worlds.");

    storage.CopyToStream(s).IgnoreResult();
  }
}

WD_STATICLINK_FILE(Core, Core_WorldSerializer_Implementation_WorldWriter);
