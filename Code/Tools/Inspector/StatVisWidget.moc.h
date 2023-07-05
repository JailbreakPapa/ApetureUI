#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Time.h>
#include <Inspector/ui_StatVisWidget.h>
#include <QAction>
#include <QGraphicsView>
#include <QListWidgetItem>
#include <ads/DockWidget.h>

class wdQtStatVisWidget : public ads::CDockWidget, public Ui_StatVisWidget
{
public:
  Q_OBJECT

public:
  static const wdUInt8 s_uiMaxColors = 9;

  wdQtStatVisWidget(QWidget* pParent, wdInt32 iWindowNumber);
  ~wdQtStatVisWidget();

  void UpdateStats();

  static wdQtStatVisWidget* s_pWidget;

  void AddStat(const wdString& sStatPath, bool bEnabled = true, bool bRaiseWindow = true);

  void Save();
  void Load();

private Q_SLOTS:

  void on_ComboTimeframe_currentIndexChanged(int index);
  void on_LineName_textChanged(const QString& text);
  void on_SpinMin_valueChanged(double val);
  void on_SpinMax_valueChanged(double val);
  void on_ToggleVisible();
  void on_ButtonRemove_clicked();
  void on_ListStats_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);

public:
  QAction m_ShowWindowAction;

private:
  QGraphicsPathItem* m_pPath[s_uiMaxColors];
  QGraphicsPathItem* m_pPathMax;
  QGraphicsScene m_Scene;

  static wdInt32 s_iCurColor;

  wdTime m_DisplayInterval;

  wdInt32 m_iWindowNumber;

  struct StatsData
  {
    StatsData() { m_pListItem = nullptr; }

    QListWidgetItem* m_pListItem;
    wdUInt8 m_uiColor;
  };

  wdMap<wdString, StatsData> m_Stats;
};

