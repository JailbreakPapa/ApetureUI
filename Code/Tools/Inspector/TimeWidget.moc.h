#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Time.h>
#include <Inspector/ui_TimeWidget.h>
#include <QGraphicsView>
#include <QListWidgetItem>
#include <ads/DockWidget.h>

class wdQtTimeWidget : public ads::CDockWidget, public Ui_TimeWidget
{
public:
  Q_OBJECT

public:
  static const wdUInt8 s_uiMaxColors = 9;

  wdQtTimeWidget(QWidget* pParent = 0);

  static wdQtTimeWidget* s_pWidget;

private Q_SLOTS:

  void on_ListClocks_itemChanged(QListWidgetItem* item);
  void on_ComboTimeframe_currentIndexChanged(int index);

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();
  void UpdateStats();

private:
  QGraphicsPathItem* m_pPath[s_uiMaxColors];
  QGraphicsPathItem* m_pPathMax;
  QGraphicsScene m_Scene;

  wdUInt32 m_uiMaxSamples;

  wdUInt8 m_uiColorsUsed;
  bool m_bClocksChanged;

  wdTime m_MaxGlobalTime;
  wdTime m_DisplayInterval;
  wdTime m_LastUpdatedClockList;

  struct TimeSample
  {
    wdTime m_AtGlobalTime;
    wdTime m_Timestep;
  };

  struct ClockData
  {
    wdDeque<TimeSample> m_TimeSamples;

    bool m_bDisplay = true;
    wdUInt8 m_uiColor = 0xFF;
    wdTime m_MinTimestep = wdTime::Seconds(60.0);
    wdTime m_MaxTimestep;
    QListWidgetItem* m_pListItem = nullptr;
  };

  wdMap<wdString, ClockData> m_ClockData;
};

