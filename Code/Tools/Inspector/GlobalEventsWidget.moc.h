#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Communication/GlobalEvent.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_GlobalEventsWidget.h>
#include <ads/DockWidget.h>

class wdQtGlobalEventsWidget : public ads::CDockWidget, public Ui_GlobalEventsWidget
{
public:
  Q_OBJECT

public:
  wdQtGlobalEventsWidget(QWidget* pParent = 0);

  static wdQtGlobalEventsWidget* s_pWidget;

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();

private:
  void UpdateTable(bool bRecreate);

  struct GlobalEventsData
  {
    wdInt32 m_iTableRow;
    wdUInt32 m_uiTimesFired;
    wdUInt16 m_uiNumHandlers;
    wdUInt16 m_uiNumHandlersOnce;

    GlobalEventsData()
    {
      m_iTableRow = -1;

      m_uiTimesFired = 0;
      m_uiNumHandlers = 0;
      m_uiNumHandlersOnce = 0;
    }
  };

  wdMap<wdString, GlobalEventsData> m_Events;
};

