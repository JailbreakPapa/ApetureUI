#pragma once

#include <Foundation/Basics.h>
#include <Inspector/StatVisWidget.moc.h>
#include <Inspector/ui_MainWindow.h>
#include <QMainWindow>
#include <ads/DockManager.h>
#include <qprocess.h>

class wdQtMainWindow : public QMainWindow, public Ui_MainWindow
{
  enum OnTopMode
  {
    Never,
    Always,
    WhenConnected
  };

public:
  Q_OBJECT

public:
  wdQtMainWindow();
  ~wdQtMainWindow();

  static wdQtMainWindow* s_pWidget;

  static void ProcessTelemetry(void* pUnuseed);

  virtual void closeEvent(QCloseEvent* pEvent);

  void InitTPApps();

  void CloseTPApps();
  
public Q_SLOTS:
  void DockWidgetVisibilityChanged(bool bVisible);
  void UpdateNetworkTimeOut();

private Q_SLOTS:
  void on_ActionShowWindowLog_triggered();
  void on_ActionShowWindowMemory_triggered();
  void on_ActionShowWindowTime_triggered();
  void on_ActionShowWindowInput_triggered();
  void on_ActionShowWindowCVar_triggered();
  void on_ActionShowWindowReflection_triggered();
  void on_ActionShowWindowSubsystems_triggered();
  void on_ActionShowWindowPlugins_triggered();
  void on_ActionShowWindowFile_triggered();
  void on_ActionShowWindowGlobalEvents_triggered();
  void on_ActionShowWindowData_triggered();
  void on_ActionShowWindowResource_triggered();

  void on_ActionOnTopWhenConnected_triggered();
  void on_ActionAlwaysOnTop_triggered();
  void on_ActionNeverOnTop_triggered();

  void on_ActionOptick_triggered();
  void on_ActionLivePP_triggered();

private:
  void SetAlwaysOnTop(OnTopMode Mode);
  void UpdateAlwaysOnTop();
  void SetupNetworkTimer();
  void UpdateNetwork();

private:
  OnTopMode m_OnTopMode;
  QTimer* m_pNetworkTimer;

public:
  QProcess* optick = nullptr;
  QProcess* livepp = nullptr;

  ads::CDockManager* m_DockManager = nullptr;
  QAction* m_pActionShowStatIn[10];
  wdQtStatVisWidget* m_pStatHistoryWidgets[10];
};

