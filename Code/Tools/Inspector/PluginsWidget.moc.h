#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_PluginsWidget.h>
#include <ads/DockWidget.h>

class wdQtPluginsWidget : public ads::CDockWidget, public Ui_PluginsWidget
{
public:
  Q_OBJECT

public:
  wdQtPluginsWidget(QWidget* pParent = 0);

  static wdQtPluginsWidget* s_pWidget;

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();
  void UpdateStats();

private:
  void UpdatePlugins();

  struct PluginsData
  {
    bool m_bReloadable;
    wdString m_sDependencies;
  };

  bool m_bUpdatePlugins;
  wdMap<wdString, PluginsData> m_Plugins;
};

