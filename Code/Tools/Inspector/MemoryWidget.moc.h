#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Time.h>
#include <Inspector/ui_MemoryWidget.h>
#include <QAction>
#include <QGraphicsView>
#include <QPointer>
#include <ads/DockWidget.h>

class QTreeWidgetItem;

class wdQtMemoryWidget : public ads::CDockWidget, public Ui_MemoryWidget
{
public:
  Q_OBJECT

public:
  static const wdUInt8 s_uiMaxColors = 9;

  wdQtMemoryWidget(QWidget* pParent = 0);

  static wdQtMemoryWidget* s_pWidget;

private Q_SLOTS:

  void on_ListAllocators_itemChanged(QTreeWidgetItem* item);
  void on_ComboTimeframe_currentIndexChanged(int index);
  void on_actionEnableOnlyThis_triggered(bool);
  void on_actionEnableAll_triggered(bool);
  void on_actionDisableAll_triggered(bool);

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();
  void UpdateStats();

private:
  void CustomContextMenuRequested(const QPoint& pos);

  QGraphicsPathItem* m_pPath[s_uiMaxColors];
  QGraphicsPathItem* m_pPathMax;
  QGraphicsScene m_Scene;

  wdTime m_LastUsedMemoryStored;
  wdTime m_LastUpdatedAllocatorList;

  wdUInt32 m_uiMaxSamples;
  wdUInt32 m_uiDisplaySamples;

  wdUInt8 m_uiColorsUsed;
  bool m_bAllocatorsChanged;

  struct AllocatorData
  {
    wdDeque<wdUInt64> m_UsedMemory;
    wdString m_sName;

    bool m_bStillInUse = true;
    bool m_bReceivedData = false;
    bool m_bDisplay = true;
    wdUInt8 m_uiColor = 0xFF;
    wdUInt32 m_uiParentId = wdInvalidIndex;
    wdUInt64 m_uiAllocs = 0;
    wdUInt64 m_uiDeallocs = 0;
    wdUInt64 m_uiLiveAllocs = 0;
    wdUInt64 m_uiMaxUsedMemoryRecently = 0;
    wdUInt64 m_uiMaxUsedMemory = 0;
    QTreeWidgetItem* m_pTreeItem = nullptr;
  };

  AllocatorData m_Accu;

  wdMap<wdUInt32, AllocatorData> m_AllocatorData;
};

