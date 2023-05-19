#pragma once

template <typename KEY, typename VALUE>
wdObjectMetaData<KEY, VALUE>::wdObjectMetaData()
{
  m_DefaultValue = VALUE();

  auto pStorage = WD_DEFAULT_NEW(Storage);
  pStorage->m_AcessingKey = KEY();
  pStorage->m_AccessMode = Storage::AccessMode::Nothing;
  SwapStorage(pStorage);
}

template <typename KEY, typename VALUE>
const VALUE* wdObjectMetaData<KEY, VALUE>::BeginReadMetaData(const KEY objectKey) const
{
  m_pMetaStorage->m_Mutex.Lock();
  WD_ASSERT_DEV(m_pMetaStorage->m_AccessMode == Storage::AccessMode::Nothing, "Already accessing some data");
  m_pMetaStorage->m_AccessMode = Storage::AccessMode::Read;
  m_pMetaStorage->m_AcessingKey = objectKey;

  const VALUE* pRes = nullptr;
  if (m_pMetaStorage->m_MetaData.TryGetValue(objectKey, pRes)) // TryGetValue is not const correct with the second parameter
    return pRes;

  return &m_DefaultValue;
}

template <typename KEY, typename VALUE>
void wdObjectMetaData<KEY, VALUE>::ClearMetaData(const KEY objectKey)
{
  WD_LOCK(m_pMetaStorage->m_Mutex);
  WD_ASSERT_DEV(m_pMetaStorage->m_AccessMode == Storage::AccessMode::Nothing, "Already accessing some data");

  if (HasMetaData(objectKey))
  {
    m_pMetaStorage->m_MetaData.Remove(objectKey);

    EventData e;
    e.m_ObjectKey = objectKey;
    e.m_pValue = &m_DefaultValue;

    m_pMetaStorage->m_DataModifiedEvent.Broadcast(e);
  }
}

template <typename KEY, typename VALUE>
bool wdObjectMetaData<KEY, VALUE>::HasMetaData(const KEY objectKey) const
{
  WD_LOCK(m_pMetaStorage->m_Mutex);
  const VALUE* pValue = nullptr;
  return m_pMetaStorage->m_MetaData.TryGetValue(objectKey, pValue);
}

template <typename KEY, typename VALUE>
VALUE* wdObjectMetaData<KEY, VALUE>::BeginModifyMetaData(const KEY objectKey)
{
  m_pMetaStorage->m_Mutex.Lock();
  WD_ASSERT_DEV(m_pMetaStorage->m_AccessMode == Storage::AccessMode::Nothing, "Already accessing some data");
  m_pMetaStorage->m_AccessMode = Storage::AccessMode::Write;
  m_pMetaStorage->m_AcessingKey = objectKey;

  return &m_pMetaStorage->m_MetaData[objectKey];
}

template <typename KEY, typename VALUE>
void wdObjectMetaData<KEY, VALUE>::EndReadMetaData() const
{
  WD_ASSERT_DEV(m_pMetaStorage->m_AccessMode == Storage::AccessMode::Read, "Not accessing data at the moment");

  m_pMetaStorage->m_AccessMode = Storage::AccessMode::Nothing;
  m_pMetaStorage->m_Mutex.Unlock();
}


template <typename KEY, typename VALUE>
void wdObjectMetaData<KEY, VALUE>::EndModifyMetaData(wdUInt32 uiModifiedFlags /*= 0xFFFFFFFF*/)
{
  WD_ASSERT_DEV(m_pMetaStorage->m_AccessMode == Storage::AccessMode::Write, "Not accessing data at the moment");
  m_pMetaStorage->m_AccessMode = Storage::AccessMode::Nothing;

  if (uiModifiedFlags != 0)
  {
    EventData e;
    e.m_ObjectKey = m_pMetaStorage->m_AcessingKey;
    e.m_pValue = &m_pMetaStorage->m_MetaData[m_pMetaStorage->m_AcessingKey];
    e.m_uiModifiedFlags = uiModifiedFlags;

    m_pMetaStorage->m_DataModifiedEvent.Broadcast(e);
  }

  m_pMetaStorage->m_Mutex.Unlock();
}


template <typename KEY, typename VALUE>
void wdObjectMetaData<KEY, VALUE>::AttachMetaDataToAbstractGraph(wdAbstractObjectGraph& inout_graph) const
{
  auto& AllNodes = inout_graph.GetAllNodes();

  WD_LOCK(m_pMetaStorage->m_Mutex);

  wdHashTable<const char*, wdVariant> DefaultValues;

  // store the default values in an easily accessible hash map, to be able to compare against them
  {
    DefaultValues.Reserve(m_DefaultValue.GetDynamicRTTI()->GetProperties().GetCount());

    for (const auto& pProp : m_DefaultValue.GetDynamicRTTI()->GetProperties())
    {
      if (pProp->GetCategory() != wdPropertyCategory::Member)
        continue;

      DefaultValues[pProp->GetPropertyName()] =
        wdReflectionUtils::GetMemberPropertyValue(static_cast<wdAbstractMemberProperty*>(pProp), &m_DefaultValue);
    }
  }

  // now serialize all properties that differ from the default value
  {
    wdVariant value;

    for (auto it = AllNodes.GetIterator(); it.IsValid(); ++it)
    {
      auto* pNode = it.Value();
      const wdUuid& guid = pNode->GetGuid();

      const VALUE* pMeta = nullptr;
      if (!m_pMetaStorage->m_MetaData.TryGetValue(guid, pMeta)) // TryGetValue is not const correct with the second parameter
        continue;                               // it is the default object, so all values are default -> skip

      for (const auto& pProp : pMeta->GetDynamicRTTI()->GetProperties())
      {
        if (pProp->GetCategory() != wdPropertyCategory::Member)
          continue;

        value = wdReflectionUtils::GetMemberPropertyValue(static_cast<wdAbstractMemberProperty*>(pProp), pMeta);

        if (value.IsValid() && DefaultValues[pProp->GetPropertyName()] != value)
        {
          pNode->AddProperty(pProp->GetPropertyName(), value);
        }
      }
    }
  }
}


template <typename KEY, typename VALUE>
void wdObjectMetaData<KEY, VALUE>::RestoreMetaDataFromAbstractGraph(const wdAbstractObjectGraph& graph)
{
  WD_LOCK(m_pMetaStorage->m_Mutex);

  wdHybridArray<wdString, 16> PropertyNames;

  // find all properties (names) that we want to read
  {
    for (const auto& pProp : m_DefaultValue.GetDynamicRTTI()->GetProperties())
    {
      if (pProp->GetCategory() != wdPropertyCategory::Member)
        continue;

      PropertyNames.PushBack(pProp->GetPropertyName());
    }
  }

  auto& AllNodes = graph.GetAllNodes();

  for (auto it = AllNodes.GetIterator(); it.IsValid(); ++it)
  {
    auto* pNode = it.Value();
    const wdUuid& guid = pNode->GetGuid();

    for (const auto& name : PropertyNames)
    {
      if (const auto* pProp = pNode->FindProperty(name))
      {
        VALUE* pValue = &m_pMetaStorage->m_MetaData[guid];

        wdReflectionUtils::SetMemberPropertyValue(
          static_cast<wdAbstractMemberProperty*>(pValue->GetDynamicRTTI()->FindPropertyByName(name)), pValue, pProp->m_Value);
      }
    }
  }
}

template <typename KEY, typename VALUE>
wdSharedPtr<typename wdObjectMetaData<KEY, VALUE>::Storage> wdObjectMetaData<KEY, VALUE>::SwapStorage(wdSharedPtr<typename wdObjectMetaData<KEY, VALUE>::Storage> pNewStorage)
{
  WD_ASSERT_ALWAYS(pNewStorage != nullptr, "Need a valid history storage object");

  auto retVal = m_pMetaStorage;

  m_EventsUnsubscriber.Unsubscribe();

  m_pMetaStorage = pNewStorage;

  m_pMetaStorage->m_DataModifiedEvent.AddEventHandler([this](const EventData& e) { m_DataModifiedEvent.Broadcast(e); }, m_EventsUnsubscriber);

  return retVal;
}
