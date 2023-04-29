#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Communication/Event.h>
#include <Foundation/Strings/HashedString.h>
#include <Foundation/Types/SharedPtr.h>
#include <Foundation/Types/Variant.h>

class wdStreamReader;
class wdStreamWriter;

/// \brief Flags for entries in wdBlackboard.
struct WD_CORE_DLL wdBlackboardEntryFlags
{
  using StorageType = wdUInt16;

  enum Enum
  {
    None = 0,
    Save = WD_BIT(0),          ///< Include the entry during serialization
    OnChangeEvent = WD_BIT(1), ///< Broadcast the 'ValueChanged' event when this entry's value is modified

    UserFlag0 = WD_BIT(7),
    UserFlag1 = WD_BIT(8),
    UserFlag2 = WD_BIT(9),
    UserFlag3 = WD_BIT(10),
    UserFlag4 = WD_BIT(11),
    UserFlag5 = WD_BIT(12),
    UserFlag6 = WD_BIT(13),
    UserFlag7 = WD_BIT(14),

    Invalid = WD_BIT(15),

    Default = None
  };

  struct Bits
  {
    StorageType Save : 1;
    StorageType OnChangeEvent : 1;
    StorageType Reserved : 5;
    StorageType UserFlag0 : 1;
    StorageType UserFlag1 : 1;
    StorageType UserFlag2 : 1;
    StorageType UserFlag3 : 1;
    StorageType UserFlag4 : 1;
    StorageType UserFlag5 : 1;
    StorageType UserFlag6 : 1;
    StorageType UserFlag7 : 1;
    StorageType Invalid : 1;
  };
};

WD_DECLARE_FLAGS_OPERATORS(wdBlackboardEntryFlags);
WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdBlackboardEntryFlags);


/// \brief A blackboard is a key/value store that provides OnChange events to be informed when a value changes.
///
/// Blackboards are used to gather typically small pieces of data. Some systems write the data, other systems read it.
/// Through the blackboard, arbitrary systems can interact.
///
/// For example this is commonly used in game AI, where some system gathers interesting pieces of data about the environment,
/// and then NPCs might use that information to make decisions.
class WD_CORE_DLL wdBlackboard : public wdRefCounted
{
private:
  wdBlackboard();

public:
  ~wdBlackboard();

  /// \brief Factory method to create a new blackboard.
  ///
  /// Since blackboards use shared ownership we need to make sure that blackboards are created in wdCore.dll.
  /// Some compilers (MSVC) create local v-tables which can become stale if a blackboard was registered as global but the DLL
  /// which created the blackboard is already unloaded.
  ///
  /// See https://groups.google.com/g/microsoft.public.vc.language/c/atSh_2VSc2w/m/EgJ3r_7OzVUJ?pli=1
  static wdSharedPtr<wdBlackboard> Create(wdAllocatorBase* pAllocator = wdFoundation::GetDefaultAllocator());

  /// \brief Factory method to get access to a globally registered blackboard.
  ///
  /// If a blackboard with that name was already created globally before, its reference is returned.
  /// Otherwise it will be created and permanently registered under that name.
  /// Global blackboards cannot be removed. Although you can change their name via "SetName()",
  /// the name under which they are registered globally will not change.
  ///
  /// If at some point you want to "remove" a global blackboard, instead call UnregisterAllEntries() to
  /// clear all its values.
  static wdSharedPtr<wdBlackboard> GetOrCreateGlobal(const wdHashedString& sBlackboardName, wdAllocatorBase* pAllocator = wdFoundation::GetDefaultAllocator());

  /// \brief Finds a global blackboard with the given name.
  static wdSharedPtr<wdBlackboard> FindGlobal(const wdTempHashedString& sBlackboardName);

  /// \brief Changes the name of the blackboard.
  ///
  /// \note For global blackboards this has no effect under which name they are found. A global blackboard continues to
  /// be found by the name under which it was originally registered.
  void SetName(wdStringView sName);
  const char* GetName() const { return m_sName; }
  const wdHashedString& GetNameHashed() const { return m_sName; }

  struct Entry
  {
    wdVariant m_Value;
    wdBitflags<wdBlackboardEntryFlags> m_Flags;

    /// The change counter is increased every time the entry's value changes.
    /// Read this and compare it to a previous known value, to detect whether the value was changed since the last check.
    wdUInt32 m_uiChangeCounter = 0;
  };

  struct EntryEvent
  {
    wdHashedString m_sName;
    wdVariant m_OldValue;
    const Entry* m_pEntry;
  };

  /// \brief Registers an entry with a name, value and flags.
  ///
  /// If the entry already exists, it will add the entry flags that hadn't been set before, but NOT change the value.
  /// Thus you can use it to make sure that a value exists with a given start value, but keep it unchanged, if it already existed.
  void RegisterEntry(const wdHashedString& sName, const wdVariant& initialValue, wdBitflags<wdBlackboardEntryFlags> flags = wdBlackboardEntryFlags::None);

  /// \brief Removes the named entry. Does nothing, if no such entry exists.
  void UnregisterEntry(const wdHashedString& sName);

  ///  \brief Removes all entries.
  void UnregisterAllEntries();

  /// \brief Sets the value of the named entry.
  ///
  /// If the entry doesn't exist, WD_FAILURE is returned.
  ///
  /// If the 'OnChangeEvent' flag is set for this entry, OnEntryEvent() will be broadcast.
  /// However, if the new value is no different to the old, no event will be broadcast, unless 'force' is set to true.
  ///
  /// Returns WD_FAILURE, if the named entry hasn't been registered before.
  wdResult SetEntryValue(const wdTempHashedString& sName, const wdVariant& value, bool bForce = false);

  /// \brief Returns a pointer to the named entry, or nullptr if no such entry was registered.
  const Entry* GetEntry(const wdTempHashedString& sName) const;

  /// \brief Returns the flags of the named entry, or wdBlackboardEntryFlags::Invalid, if no such entry was registered.
  wdBitflags<wdBlackboardEntryFlags> GetEntryFlags(const wdTempHashedString& sName) const;

  /// \brief Returns the value of the named entry, or the fallback wdVariant, if no such entry was registered.
  wdVariant GetEntryValue(const wdTempHashedString& sName, wdVariant fallback = {}) const;

  /// \brief Grants read access to the entire map of entries.
  const wdHashTable<wdHashedString, Entry>& GetAllEntries() const { return m_Entries; }

  /// \brief Allows you to register to the OnEntryEvent. This is broadcast whenever an entry is modified that has the flag wdBlackboardEntryFlags::OnChangeEvent.
  const wdEvent<EntryEvent>& OnEntryEvent() const { return m_EntryEvents; }

  /// \brief This counter is increased every time an entry is added or removed (but not when it is modified).
  ///
  /// Comparing this value to a previous known value allows to quickly detect whether the set of entries has changed.
  wdUInt32 GetBlackboardChangeCounter() const { return m_uiBlackboardChangeCounter; }

  /// \brief This counter is increased every time any entry's value is modified.
  ///
  /// Comparing this value to a previous known value allows to quickly detect whether any entry has changed recently.
  wdUInt32 GetBlackboardEntryChangeCounter() const { return m_uiBlackboardEntryChangeCounter; }

  /// \brief Stores all entries that have the 'Save' flag in the stream.
  wdResult Serialize(wdStreamWriter& inout_stream) const;

  /// \brief Restores entries from the stream.
  ///
  /// If the blackboard already contains entries, the deserialized data is ADDED to the blackboard.
  /// If deserialized entries overlap with existing ones, the deserialized entries will overwrite the existing ones (both values and flags).
  wdResult Deserialize(wdStreamReader& inout_stream);

private:
  wdHashedString m_sName;
  wdEvent<EntryEvent> m_EntryEvents;
  wdUInt32 m_uiBlackboardChangeCounter = 0;
  wdUInt32 m_uiBlackboardEntryChangeCounter = 0;
  wdHashTable<wdHashedString, Entry> m_Entries;

  WD_MAKE_SUBSYSTEM_STARTUP_FRIEND(Core, Blackboard);
  static wdMutex s_GlobalBlackboardsMutex;
  static wdHashTable<wdHashedString, wdSharedPtr<wdBlackboard>> s_GlobalBlackboards;
};

//////////////////////////////////////////////////////////////////////////

struct WD_CORE_DLL wdBlackboardCondition
{
  wdHashedString m_sEntryName;
  double m_fComparisonValue = 0.0;
  wdEnum<wdComparisonOperator> m_Operator;

  bool IsConditionMet(const wdBlackboard& blackboard) const;

  wdResult Serialize(wdStreamWriter& inout_stream) const;
  wdResult Deserialize(wdStreamReader& inout_stream);

  const char* GetEntryName() const { return m_sEntryName; }
  void SetEntryName(const char* szName) { m_sEntryName.Assign(szName); }
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdBlackboardCondition);
