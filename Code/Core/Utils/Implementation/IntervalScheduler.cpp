#include <Core/CorePCH.h>

#include <Core/Utils/IntervalScheduler.h>
#include <Foundation/SimdMath/SimdRandom.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_ENUM(wdUpdateRate, 1)
  WD_ENUM_CONSTANTS(wdUpdateRate::EveryFrame)
  WD_ENUM_CONSTANTS(wdUpdateRate::Max30fps, wdUpdateRate::Max20fps, wdUpdateRate::Max10fps)
  WD_ENUM_CONSTANTS(wdUpdateRate::Max5fps, wdUpdateRate::Max2fps, wdUpdateRate::Max1fps)
WD_END_STATIC_REFLECTED_ENUM;
// clang-format on

static wdTime s_Intervals[] = {
  wdTime::Zero(),              // EveryFrame
  wdTime::Seconds(1.0 / 30.0), // Max30fps
  wdTime::Seconds(1.0 / 20.0), // Max20fps
  wdTime::Seconds(1.0 / 10.0), // Max10fps
  wdTime::Seconds(1.0 / 5.0),  // Max5fps
  wdTime::Seconds(1.0 / 2.0),  // Max2fps
  wdTime::Seconds(1.0 / 1.0),  // Max1fps
};

static_assert(WD_ARRAY_SIZE(s_Intervals) == wdUpdateRate::Max1fps + 1);

wdTime wdUpdateRate::GetInterval(Enum updateRate)
{
  return s_Intervals[updateRate];
}

//////////////////////////////////////////////////////////////////////////

WD_ALWAYS_INLINE float GetRandomZeroToOne(int iPos, wdUInt32& ref_uiSeed)
{
  return wdSimdRandom::FloatZeroToOne(wdSimdVec4i(iPos), wdSimdVec4u(ref_uiSeed++)).x();
}

constexpr wdTime s_JitterRange = wdTime::Microseconds(10);

WD_ALWAYS_INLINE wdTime GetRandomTimeJitter(int iPos, wdUInt32& ref_uiSeed)
{
  const float x = wdSimdRandom::FloatZeroToOne(wdSimdVec4i(iPos), wdSimdVec4u(ref_uiSeed++)).x();
  return s_JitterRange * (x * 2.0f - 1.0f);
}

wdIntervalSchedulerBase::wdIntervalSchedulerBase(wdTime minInterval, wdTime maxInterval)
  : m_MinInterval(minInterval)
  , m_MaxInterval(maxInterval)
{
  m_fInvIntervalRange = 1.0 / (m_MaxInterval - m_MinInterval).GetSeconds();

  for (wdUInt32 i = 0; i < HistogramSize; ++i)
  {
    m_HistogramSlotValues[i] = GetHistogramSlotValue(i);
  }
}

wdIntervalSchedulerBase::~wdIntervalSchedulerBase() = default;

void wdIntervalSchedulerBase::AddOrUpdateWork(wdUInt64 workId, wdTime interval)
{
  DataMap::Iterator it;
  if (m_WorkIdToData.TryGetValue(workId, it))
  {
    wdTime oldInterval = it.Value().m_Interval;
    if (interval == oldInterval)
      return;

    m_Data.Remove(it);

    const wdUInt32 uiHistogramIndex = GetHistogramIndex(oldInterval);
    m_Histogram[uiHistogramIndex]--;
  }

  Data data;
  data.m_WorkId = workId;
  data.m_Interval = wdMath::Max(interval, wdTime::Zero());
  data.m_DueTime = m_CurrentTime + GetRandomZeroToOne(m_Data.GetCount(), m_uiSeed) * data.m_Interval;
  data.m_LastScheduledTime = m_CurrentTime;

  m_WorkIdToData[workId] = InsertData(data);

  const wdUInt32 uiHistogramIndex = GetHistogramIndex(data.m_Interval);
  m_Histogram[uiHistogramIndex]++;
}

void wdIntervalSchedulerBase::RemoveWork(wdUInt64 workId)
{
  DataMap::Iterator it;
  WD_VERIFY(m_WorkIdToData.Remove(workId, &it), "Entry not found");

  wdTime oldInterval = it.Value().m_Interval;
  m_Data.Remove(it);

  const wdUInt32 uiHistogramIndex = GetHistogramIndex(oldInterval);
  m_Histogram[uiHistogramIndex]--;
}

wdTime wdIntervalSchedulerBase::GetInterval(wdUInt64 workId) const
{
  DataMap::Iterator it;
  WD_VERIFY(m_WorkIdToData.TryGetValue(workId, it), "Entry not found");
  return it.Value().m_Interval;
}

void wdIntervalSchedulerBase::Update(wdTime deltaTime, wdDelegate<void(wdUInt64, wdTime)> runWorkCallback)
{
  if (deltaTime <= wdTime::Zero())
    return;

  if (m_Data.IsEmpty())
  {
    m_fNumWorkToSchedule = 0.0;
  }
  else
  {
    double fNumWork = 0;
    for (wdUInt32 i = 0; i < HistogramSize; ++i)
    {
      fNumWork += (1.0 / wdMath::Max(m_HistogramSlotValues[i], deltaTime).GetSeconds()) * m_Histogram[i];
    }
    fNumWork *= deltaTime.GetSeconds();

    if (m_fNumWorkToSchedule == 0.0)
    {
      m_fNumWorkToSchedule = fNumWork;
    }
    else
    {
      // running average of num work per update to prevent huge spikes
      m_fNumWorkToSchedule = wdMath::Lerp<double>(m_fNumWorkToSchedule, fNumWork, 0.05);
    }

    const float fRemainder = static_cast<float>(wdMath::Fraction(m_fNumWorkToSchedule));
    const int pos = static_cast<int>(m_CurrentTime.GetNanoseconds());
    const wdUInt32 extra = GetRandomZeroToOne(pos, m_uiSeed) < fRemainder ? 1 : 0;
    const wdUInt32 uiScheduleCount = wdMath::Min(static_cast<wdUInt32>(m_fNumWorkToSchedule) + extra, m_Data.GetCount());

    // schedule work
    {
      auto it = m_Data.GetIterator();
      for (wdUInt32 i = 0; i < uiScheduleCount; ++i, ++it)
      {
        auto& data = it.Value();
        runWorkCallback(data.m_WorkId, m_CurrentTime - data.m_LastScheduledTime);

        // add a little bit of random jitter so we don't end up with perfect timings that might collide with other work
        data.m_DueTime = m_CurrentTime + wdMath::Max(data.m_Interval, deltaTime) + GetRandomTimeJitter(i, m_uiSeed);
        data.m_LastScheduledTime = m_CurrentTime;

        m_ScheduledWork.PushBack(it);
      }
    }

    // re-sort
    for (auto& it : m_ScheduledWork)
    {
      Data data = it.Value();
      m_WorkIdToData[data.m_WorkId] = InsertData(data);
      m_Data.Remove(it);
    }
    m_ScheduledWork.Clear();
  }

  m_CurrentTime += deltaTime;
}

wdUInt32 wdIntervalSchedulerBase::GetHistogramIndex(wdTime value)
{
  constexpr wdUInt32 maxSlotIndex = HistogramSize - 1;
  const double x = wdMath::Max((value - m_MinInterval).GetSeconds() * m_fInvIntervalRange, 0.0);
  const double i = wdMath::Sqrt(x) * maxSlotIndex;
  return wdMath::Min(static_cast<wdUInt32>(i), maxSlotIndex);
}

wdTime wdIntervalSchedulerBase::GetHistogramSlotValue(wdUInt32 uiIndex)
{
  constexpr double norm = 1.0 / (HistogramSize - 1.0);
  const double x = uiIndex * norm;
  return (x * x) * (m_MaxInterval - m_MinInterval) + m_MinInterval;
}

wdIntervalSchedulerBase::DataMap::Iterator wdIntervalSchedulerBase::InsertData(Data& data)
{
  // make sure that we have a unique due time since the map can't store multiple keys with the same value
  int pos = 0;
  while (m_Data.Contains(data.m_DueTime))
  {
    data.m_DueTime += GetRandomTimeJitter(pos++, m_uiSeed);
  }

  return m_Data.Insert(data.m_DueTime, data);
}


WD_STATICLINK_FILE(Core, Core_Utils_Implementation_IntervalScheduler);
