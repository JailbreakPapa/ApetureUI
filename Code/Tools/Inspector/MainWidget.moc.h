#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Containers/Set.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Types/Variant.h>
#include <Inspector/ui_MainWidget.h>
#include <QMainWindow>
#include <ads/DockManager.h>

class QTreeWidgetItem;

class wdQtMainWidget : public ads::CDockWidget, public Ui_MainWidget
{
  Q_OBJECT
public:
  static wdQtMainWidget* s_pWidget;

  wdQtMainWidget(QWidget* pParent = nullptr);
  ~wdQtMainWidget();

  void ResetStats();
  void UpdateStats();
  virtual void closeEvent(QCloseEvent* pEvent) override;

  static void ProcessTelemetry(void* pUnuseed);

public Q_SLOTS:
  void ShowStatIn(bool);

private Q_SLOTS:
  void on_ButtonConnect_clicked();

  void on_TreeStats_itemChanged(QTreeWidgetItem* item, int column);
  void on_TreeStats_customContextMenuRequested(const QPoint& p);

private:
  void SaveFavorites();
  void LoadFavorites();

  QTreeWidgetItem* CreateStat(const char* szPath, bool bParent);
  void SetFavorite(const wdString& sStat, bool bFavorite);

  wdUInt32 m_uiMaxStatSamples;
  wdTime m_MaxGlobalTime;

  struct StatSample
  {
    wdTime m_AtGlobalTime;
    double m_Value;
  };

  struct StatData
  {
    wdDeque<StatSample> m_History;

    wdVariant m_Value;
    QTreeWidgetItem* m_pItem;
    QTreeWidgetItem* m_pItemFavorite;

    StatData()
    {
      m_pItem = nullptr;
      m_pItemFavorite = nullptr;
    }
  };

  friend class wdQtStatVisWidget;
  wdMap<wdString, StatData> m_Stats;
  wdSet<wdString> m_Favorites;
};

