#include <Core/CorePCH.h>

#include <Core/Utils/Blackboard.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/Stream.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Reflection/Reflection.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_BITFLAGS(wdBlackboardEntryFlags, 1)
  WD_BITFLAGS_CONSTANTS(wdBlackboardEntryFlags::Save, wdBlackboardEntryFlags::OnChangeEvent,
    wdBlackboardEntryFlags::UserFlag0, wdBlackboardEntryFlags::UserFlag1, wdBlackboardEntryFlags::UserFlag2, wdBlackboardEntryFlags::UserFlag3, wdBlackboardEntryFlags::UserFlag4, wdBlackboardEntryFlags::UserFlag5, wdBlackboardEntryFlags::UserFlag6, wdBlackboardEntryFlags::UserFlag7)
WD_END_STATIC_REFLECTED_BITFLAGS;
// clang-format on

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(Core, Blackboard)

  ON_CORESYSTEMS_SHUTDOWN
  {
    WD_LOCK(wdBlackboard::s_GlobalBlackboardsMutex);
    wdBlackboard::s_GlobalBlackboards.Clear();
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on

// static
wdMutex wdBlackboard::s_GlobalBlackboardsMutex;
wdHashTable<wdHashedString, wdSharedPtr<wdBlackboard>> wdBlackboard::s_GlobalBlackboards;

// static
wdSharedPtr<wdBlackboard> wdBlackboard::Create(wdAllocatorBase* pAllocator /*= wdFoundation::GetDefaultAllocator()*/)
{
  return WD_NEW(pAllocator, wdBlackboard);
}

// static
wdSharedPtr<wdBlackboard> wdBlackboard::GetOrCreateGlobal(const wdHashedString& sBlackboardName, wdAllocatorBase* pAllocator /*= wdFoundation::GetDefaultAllocator()*/)
{
  WD_LOCK(s_GlobalBlackboardsMutex);

  auto it = s_GlobalBlackboards.Find(sBlackboardName);

  if (it.IsValid())
  {
    return it.Value();
  }

  wdSharedPtr<wdBlackboard> pShrd = WD_NEW(pAllocator, wdBlackboard);
  pShrd->m_sName = sBlackboardName;
  s_GlobalBlackboards.Insert(sBlackboardName, pShrd);

  return pShrd;
}

// static
wdSharedPtr<wdBlackboard> wdBlackboard::FindGlobal(const wdTempHashedString& sBlackboardName)
{
  WD_LOCK(s_GlobalBlackboardsMutex);

  wdSharedPtr<wdBlackboard> pBlackboard;
  s_GlobalBlackboards.TryGetValue(sBlackboardName, pBlackboard);
  return pBlackboard;
}

wdBlackboard::wdBlackboard() = default;
wdBlackboard::~wdBlackboard() = default;

void wdBlackboard::SetName(wdStringView sName)
{
  WD_LOCK(s_GlobalBlackboardsMutex);
  m_sName.Assign(sName);
}

void wdBlackboard::RegisterEntry(const wdHashedString& sName, const wdVariant& initialValue, wdBitflags<wdBlackboardEntryFlags> flags /*= wdBlackboardEntryFlags::None*/)
{
  WD_ASSERT_ALWAYS(!flags.IsSet(wdBlackboardEntryFlags::Invalid), "The invalid flag is reserved for internal use.");

  bool bExisted = false;
  Entry& entry = m_Entries.FindOrAdd(sName, &bExisted);

  if (!bExisted || entry.m_Flags != flags)
  {
    ++m_uiBlackboardChangeCounter;
    entry.m_Flags |= flags;
  }

  if (!bExisted && entry.m_Value != initialValue)
  {
    // broadcasts the change event, in case we overwrite an existing entry
    SetEntryValue(sName, initialValue).IgnoreResult();
  }
}

void wdBlackboard::UnregisterEntry(const wdHashedString& sName)
{
  if (m_Entries.Remove(sName))
  {
    ++m_uiBlackboardChangeCounter;
  }
}

void wdBlackboard::UnregisterAllEntries()
{
  if (m_Entries.IsEmpty() == false)
  {
    ++m_uiBlackboardChangeCounter;
  }

  m_Entries.Clear();
}

wdResult wdBlackboard::SetEntryValue(const wdTempHashedString& sName, const wdVariant& value, bool bForce /*= false*/)
{
  auto itEntry = m_Entries.Find(sName);

  if (!itEntry.IsValid())
  {
    return WD_FAILURE;
  }

  Entry& entry = itEntry.Value();

  if (!bForce && entry.m_Value == value)
    return WD_SUCCESS;

  ++m_uiBlackboardEntryChangeCounter;
  ++entry.m_uiChangeCounter;

  if (entry.m_Flags.IsSet(wdBlackboardEntryFlags::OnChangeEvent))
  {
    EntryEvent e;
    e.m_sName = itEntry.Key();
    e.m_OldValue = entry.m_Value;
    e.m_pEntry = &entry;

    entry.m_Value = value;

    m_EntryEvents.Broadcast(e, 1); // limited recursion is allowed
  }
  else
  {
    entry.m_Value = value;
  }

  return WD_SUCCESS;
}

const wdBlackboard::Entry* wdBlackboard::GetEntry(const wdTempHashedString& sName) const
{
  auto itEntry = m_Entries.Find(sName);

  if (!itEntry.IsValid())
    return nullptr;

  return &itEntry.Value();
}

wdVariant wdBlackboard::GetEntryValue(const wdTempHashedString& sName, wdVariant fallback) const
{
  auto value = m_Entries.GetValue(sName);
  return value != nullptr ? value->m_Value : fallback;
}

wdBitflags<wdBlackboardEntryFlags> wdBlackboard::GetEntryFlags(const wdTempHashedString& sName) const
{
  auto itEntry = m_Entries.Find(sName);

  if (!itEntry.IsValid())
  {
    return wdBlackboardEntryFlags::Invalid;
  }

  return itEntry.Value().m_Flags;
}

wdResult wdBlackboard::Serialize(wdStreamWriter& inout_stream) const
{
  inout_stream.WriteVersion(1);

  wdUInt32 uiEntries = 0;

  for (auto it : m_Entries)
  {
    if (it.Value().m_Flags.IsSet(wdBlackboardEntryFlags::Save))
    {
      ++uiEntries;
    }
  }

  inout_stream << uiEntries;

  for (auto it : m_Entries)
  {
    const Entry& e = it.Value();

    if (e.m_Flags.IsSet(wdBlackboardEntryFlags::Save))
    {
      inout_stream << it.Key();
      inout_stream << e.m_Flags;
      inout_stream << e.m_Value;
    }
  }

  return WD_SUCCESS;
}

wdResult wdBlackboard::Deserialize(wdStreamReader& inout_stream)
{
  inout_stream.ReadVersion(1);

  wdUInt32 uiEntries = 0;
  inout_stream >> uiEntries;

  for (wdUInt32 e = 0; e < uiEntries; ++e)
  {
    wdHashedString name;
    inout_stream >> name;

    wdBitflags<wdBlackboardEntryFlags> flags;
    inout_stream >> flags;

    wdVariant value;
    inout_stream >> value;

    RegisterEntry(name, value, flags);
  }

  return WD_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdBlackboardCondition, wdNoBase, 1, wdRTTIDefaultAllocator<wdBlackboardCondition>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_ACCESSOR_PROPERTY("EntryName", GetEntryName, SetEntryName),
    WD_ENUM_MEMBER_PROPERTY("Operator", wdComparisonOperator, m_Operator),
    WD_MEMBER_PROPERTY("ComparisonValue", m_fComparisonValue),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on

bool wdBlackboardCondition::IsConditionMet(const wdBlackboard& blackboard) const
{
  auto pEntry = blackboard.GetEntry(m_sEntryName);
  if (pEntry != nullptr && pEntry->m_Value.IsNumber())
  {
    double fEntryValue = pEntry->m_Value.ConvertTo<double>();
    return wdComparisonOperator::Compare(m_Operator, fEntryValue, m_fComparisonValue);
  }

  return false;
}

constexpr wdTypeVersion s_BlackboardConditionVersion = 1;

wdResult wdBlackboardCondition::Serialize(wdStreamWriter& inout_stream) const
{
  inout_stream.WriteVersion(s_BlackboardConditionVersion);

  inout_stream << m_sEntryName;
  inout_stream << m_Operator;
  inout_stream << m_fComparisonValue;
  return WD_SUCCESS;
}

wdResult wdBlackboardCondition::Deserialize(wdStreamReader& inout_stream)
{
  const wdTypeVersion uiVersion = inout_stream.ReadVersion(s_BlackboardConditionVersion);

  inout_stream >> m_sEntryName;
  inout_stream >> m_Operator;
  inout_stream >> m_fComparisonValue;
  return WD_SUCCESS;
}

WD_STATICLINK_FILE(Core, Core_Utils_Implementation_Blackboard);
