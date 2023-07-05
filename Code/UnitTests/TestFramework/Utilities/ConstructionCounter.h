#pragma once

#include <Foundation/Algorithm/HashingUtils.h>
#include <TestFramework/TestFrameworkDLL.h>

struct wdConstructionCounter
{
  /// Dummy m_iData, such that one can test the constructor with initialization
  wdInt32 m_iData;
  bool m_valid;

  /// Default Constructor
  wdConstructionCounter()
    : m_iData(0)
    , m_valid(true)
  {
    ++s_iConstructions;
  }

  /// Constructor with initialization
  wdConstructionCounter(wdInt32 d)
    : m_iData(d)
    , m_valid(true)
  {
    ++s_iConstructions;
  }

  /// Copy Constructor
  wdConstructionCounter(const wdConstructionCounter& cc)
    : m_iData(cc.m_iData)
    , m_valid(true)
  {
    ++s_iConstructions;
  }

  /// Move construction counts as a construction as well.
  wdConstructionCounter(wdConstructionCounter&& cc) noexcept
    : m_iData(cc.m_iData)
    , m_valid(true)
  {
    cc.m_iData = 0; // data has been moved, so "destroy" it.
    ++s_iConstructions;
  }

  /// Destructor
  ~wdConstructionCounter()
  {
    WD_ASSERT_ALWAYS(m_valid, "Destroying object twice");
    m_valid = false;
    ++s_iDestructions;
  }

  /// Assignment does not change the construction counter, because it is only executed on already constructed objects.
  void operator=(const wdConstructionCounter& cc) { m_iData = cc.m_iData; }
  /// Move assignment does not change the construction counter, because it is only executed on already constructed objects.
  void operator=(const wdConstructionCounter&& cc) noexcept { m_iData = cc.m_iData; }

  bool operator==(const wdConstructionCounter& cc) const { return m_iData == cc.m_iData; }

  bool operator!=(const wdConstructionCounter& cc) const { return m_iData != cc.m_iData; }

  bool operator<(const wdConstructionCounter& rhs) const { return m_iData < rhs.m_iData; }

  /// Checks whether n constructions have been done since the last check.
  static bool HasConstructed(wdInt32 iCons)
  {
    const bool b = s_iConstructions == s_iConstructionsLast + iCons;
    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    if (!b)
      PrintStats();

    return (b);
  }

  /// Checks whether n destructions have been done since the last check.
  static bool HasDestructed(wdInt32 iCons)
  {
    const bool b = s_iDestructions == s_iDestructionsLast + iCons;
    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    if (!b)
      PrintStats();

    return (b);
  }

  /// Checks whether n constructions and destructions have been done since the last check.
  static bool HasDone(wdInt32 iCons, wdInt32 iDes)
  {
    const bool bc = (s_iConstructions == (s_iConstructionsLast + iCons));
    const bool bd = (s_iDestructions == (s_iDestructionsLast + iDes));

    if (!(bc && bd))
      PrintStats();

    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    return (bc && bd);
  }

  /// For debugging and getting tests right: Prints out the current number of constructions and destructions
  static void PrintStats()
  {
    printf("Constructions: %d (New: %i), Destructions: %d (New: %i) \n", s_iConstructions, s_iConstructions - s_iConstructionsLast, s_iDestructions,
      s_iDestructions - s_iDestructionsLast);
  }

  /// Checks that all instances have been destructed.
  static bool HasAllDestructed()
  {
    if (s_iConstructions != s_iDestructions)
      PrintStats();

    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    return (s_iConstructions == s_iDestructions);
  }

  static void Reset()
  {
    s_iConstructions = 0;
    s_iConstructionsLast = 0;
    s_iDestructions = 0;
    s_iDestructionsLast = 0;
  }

  static wdInt32 s_iConstructions;
  static wdInt32 s_iConstructionsLast;
  static wdInt32 s_iDestructions;
  static wdInt32 s_iDestructionsLast;
};

struct wdConstructionCounterRelocatable
{
  WD_DECLARE_MEM_RELOCATABLE_TYPE();

  /// Dummy m_iData, such that one can test the constructor with initialization
  wdInt32 m_iData;

  /// Bool to track if the element was default constructed or received valid data.
  bool m_valid = false;

  wdConstructionCounterRelocatable() = default;

  wdConstructionCounterRelocatable(wdInt32 d)
    : m_iData(d)
    , m_valid(true)
  {
    s_iConstructions++;
  }

  wdConstructionCounterRelocatable(const wdConstructionCounterRelocatable& other) = delete;

  wdConstructionCounterRelocatable(wdConstructionCounterRelocatable&& other) noexcept
  {
    m_iData = other.m_iData;
    m_valid = other.m_valid;

    other.m_valid = false;
  }

  ~wdConstructionCounterRelocatable()
  {
    if (m_valid)
      s_iDestructions++;
  }

  void operator=(wdConstructionCounterRelocatable&& other) noexcept
  {
    m_iData = other.m_iData;
    m_valid = other.m_valid;

    other.m_valid = false;
    ;
  }

  /// For debugging and getting tests right: Prints out the current number of constructions and destructions
  static void PrintStats()
  {
    printf("Constructions: %d (New: %i), Destructions: %d (New: %i) \n", s_iConstructions, s_iConstructions - s_iConstructionsLast, s_iDestructions,
      s_iDestructions - s_iDestructionsLast);
  }

  /// Checks whether n constructions and destructions have been done since the last check.
  static bool HasDone(wdInt32 iCons, wdInt32 iDes)
  {
    const bool bc = (s_iConstructions == (s_iConstructionsLast + iCons));
    const bool bd = (s_iDestructions == (s_iDestructionsLast + iDes));

    if (!(bc && bd))
      PrintStats();

    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    return (bc && bd);
  }

  /// Checks that all instances have been destructed.
  static bool HasAllDestructed()
  {
    if (s_iConstructions != s_iDestructions)
      PrintStats();

    s_iConstructionsLast = s_iConstructions;
    s_iDestructionsLast = s_iDestructions;

    return (s_iConstructions == s_iDestructions);
  }

  static void Reset()
  {
    s_iConstructions = 0;
    s_iConstructionsLast = 0;
    s_iDestructions = 0;
    s_iDestructionsLast = 0;
  }

  static wdInt32 s_iConstructions;
  static wdInt32 s_iConstructionsLast;
  static wdInt32 s_iDestructions;
  static wdInt32 s_iDestructionsLast;
};

template <>
struct wdHashHelper<wdConstructionCounter>
{
  static wdUInt32 Hash(const wdConstructionCounter& value) { return wdHashHelper<wdInt32>::Hash(value.m_iData); }

  WD_ALWAYS_INLINE static bool Equal(const wdConstructionCounter& a, const wdConstructionCounter& b) { return a == b; }
};
