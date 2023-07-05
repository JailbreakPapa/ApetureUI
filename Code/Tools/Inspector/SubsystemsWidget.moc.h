#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_SubsystemsWidget.h>
#include <ads/DockWidget.h>

class wdQtSubsystemsWidget : public ads::CDockWidget, public Ui_SubsystemsWidget
{
public:
  Q_OBJECT

public:
  wdQtSubsystemsWidget(QWidget* pParent = 0);

  static wdQtSubsystemsWidget* s_pWidget;

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();
  void UpdateStats();

private:
  void UpdateSubSystems();

  struct SubsystemData
  {
    wdString m_sPlugin;
    bool m_bStartupDone[wdStartupStage::ENUM_COUNT];
    wdString m_sDependencies;
  };

  bool m_bUpdateSubsystems;
  wdMap<wdString, SubsystemData> m_Subsystems;
};

