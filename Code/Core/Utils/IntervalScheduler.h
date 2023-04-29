#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Reflection/Reflection.h>

struct WD_CORE_DLL wdUpdateRate
{
  using StorageType = wdUInt8;

  enum Enum
  {
    EveryFrame,
    Max30fps,
    Max20fps,
    Max10fps,
    Max5fps,
    Max2fps,
    Max1fps,

    Default = Max30fps
  };

  static wdTime GetInterval(Enum updateRate);
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdUpdateRate);

//////////////////////////////////////////////////////////////////////////

/// \brief Helper class to schedule work in intervals typically larger than the duration of one frame
///
/// Tries to maintain an even workload per frame and also keep the given interval for a work as best as possible.
/// A typical use case would be e.g. component update functions that don't need to be called every frame.
class WD_CORE_DLL wdIntervalSchedulerBase
{
protected:
  wdIntervalSchedulerBase(wdTime minInterval, wdTime maxInterval);
  ~wdIntervalSchedulerBase();

  void AddOrUpdateWork(wdUInt64 workId, wdTime interval);
  void RemoveWork(wdUInt64 workId);

  wdTime GetInterval(wdUInt64 workId) const;

  /// \brief Advances the scheduler by deltaTime and triggers runWorkCallback for each work that should be run during this update step.
  /// Since it is not possible to maintain the exact interval all the time the actual delta time for the work is also passed to runWorkCallback.
  void Update(wdTime deltaTime, wdDelegate<void(wdUInt64, wdTime)> runWorkCallback);

private:
  wdUInt32 GetHistogramIndex(wdTime value);
  wdTime GetHistogramSlotValue(wdUInt32 uiIndex);

  wdTime m_MinInterval;
  wdTime m_MaxInterval;
  double m_fInvIntervalRange;

  wdTime m_CurrentTime;
  double m_fNumWorkToSchedule = 0.0;

  wdUInt32 m_uiSeed = 0;

  static constexpr wdUInt32 HistogramSize = 32;
  wdUInt32 m_Histogram[HistogramSize] = {};
  wdTime m_HistogramSlotValues[HistogramSize] = {};

  struct Data
  {
    wdUInt64 m_WorkId = 0;
    wdTime m_Interval;
    wdTime m_DueTime;
    wdTime m_LastScheduledTime;
  };

  using DataMap = wdMap<wdTime, Data>;
  DataMap m_Data;
  wdHashTable<wdUInt64, DataMap::Iterator> m_WorkIdToData;

  DataMap::Iterator InsertData(Data& data);
  wdDynamicArray<DataMap::Iterator> m_ScheduledWork;
};

//////////////////////////////////////////////////////////////////////////

/// \brief \see wdIntervalSchedulerBase
template <typename T>
class wdIntervalScheduler : public wdIntervalSchedulerBase
{
  using SUPER = wdIntervalSchedulerBase;

public:
  WD_ALWAYS_INLINE wdIntervalScheduler(wdTime minInterval = wdTime::Milliseconds(1), wdTime maxInterval = wdTime::Seconds(1))
    : SUPER(minInterval, maxInterval)
  {
    static_assert(sizeof(T) <= sizeof(wdUInt64), "sizeof T must be smaller or equal to 8 bytes");
  }

  WD_ALWAYS_INLINE void AddOrUpdateWork(const T& work, wdTime interval)
  {
    SUPER::AddOrUpdateWork(*reinterpret_cast<const wdUInt64*>(&work), interval);
  }

  WD_ALWAYS_INLINE void RemoveWork(const T& work)
  {
    SUPER::RemoveWork(*reinterpret_cast<const wdUInt64*>(&work));
  }

  WD_ALWAYS_INLINE wdTime GetInterval(const T& work) const
  {
    return SUPER::GetInterval(*reinterpret_cast<const wdUInt64*>(&work));
  }

  // reference to the work that should be run and time passed since this work has been last run.
  using RunWorkCallback = wdDelegate<void(const T&, wdTime)>;

  WD_ALWAYS_INLINE void Update(wdTime deltaTime, RunWorkCallback runWorkCallback)
  {
    SUPER::Update(deltaTime, [&](wdUInt64 uiWorkId, wdTime deltaTime) {
      if (runWorkCallback.IsValid())
      {
        runWorkCallback(*reinterpret_cast<T*>(&uiWorkId), deltaTime);
      }
    });
  }
};
