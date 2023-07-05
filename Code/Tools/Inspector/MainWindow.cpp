#include <Inspector/InspectorPCH.h>

#include <Inspector/CVarsWidget.moc.h>
#include <Inspector/DataTransferWidget.moc.h>
#include <Inspector/FileWidget.moc.h>
#include <Inspector/GlobalEventsWidget.moc.h>
#include <Inspector/InputWidget.moc.h>
#include <Inspector/LogDockWidget.moc.h>
#include <Inspector/MainWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <Inspector/MemoryWidget.moc.h>
#include <Inspector/PluginsWidget.moc.h>
#include <Inspector/ReflectionWidget.moc.h>
#include <Inspector/ResourceWidget.moc.h>
#include <Inspector/SubsystemsWidget.moc.h>
#include <Inspector/TimeWidget.moc.h>

const int g_iDockingStateVersion = 1;

wdQtMainWindow* wdQtMainWindow::s_pWidget = nullptr;
void wdQtMainWindow::InitTPApps()
{
  optick = new QProcess;
  livepp = new QProcess;
}

void wdQtMainWindow::CloseTPApps()
{
  //Close TP Apps.
  optick->close();
  livepp->close();
  optick = nullptr;
  livepp = nullptr;
  delete optick;
  delete livepp;
}

wdQtMainWindow::wdQtMainWindow()
  : QMainWindow()
{
  s_pWidget = this;

  setupUi(this);

  InitTPApps();


  m_DockManager = new ads::CDockManager(this);
  m_DockManager->setConfigFlags(
    static_cast<ads::CDockManager::ConfigFlags>(ads::CDockManager::DockAreaHasCloseButton | ads::CDockManager::DockAreaCloseButtonClosesTab |
                                                ads::CDockManager::OpaqueSplitterResize | ads::CDockManager::AllTabsHaveCloseButton));

  QSettings Settings;
  SetAlwaysOnTop((OnTopMode)Settings.value("AlwaysOnTop", (int)WhenConnected).toInt());

  Settings.beginGroup("MainWindow");

  const bool bRestoreDockingState = Settings.value("DockingVersion") == g_iDockingStateVersion;

  if (bRestoreDockingState)
  {
    restoreGeometry(Settings.value("WindowGeometry", saveGeometry()).toByteArray());
  }

  // The dock manager will set ownership to null on add so there is no reason to provide an owner here.
  // Setting one will actually cause memory corruptions on shutdown for unknown reasons.
  wdQtMainWidget* pMainWidget = new wdQtMainWidget();
  wdQtLogDockWidget* pLogWidget = new wdQtLogDockWidget();
  wdQtMemoryWidget* pMemoryWidget = new wdQtMemoryWidget();
  wdQtTimeWidget* pTimeWidget = new wdQtTimeWidget();
  wdQtInputWidget* pInputWidget = new wdQtInputWidget();
  wdQtCVarsWidget* pCVarsWidget = new wdQtCVarsWidget();
  wdQtSubsystemsWidget* pSubsystemsWidget = new wdQtSubsystemsWidget();
  wdQtFileWidget* pFileWidget = new wdQtFileWidget();
  wdQtPluginsWidget* pPluginsWidget = new wdQtPluginsWidget();
  wdQtGlobalEventsWidget* pGlobalEventesWidget = new wdQtGlobalEventsWidget();
  wdQtReflectionWidget* pReflectionWidget = new wdQtReflectionWidget();
  wdQtDataWidget* pDataWidget = new wdQtDataWidget();
  wdQtResourceWidget* pResourceWidget = new wdQtResourceWidget();

  WD_VERIFY(nullptr != QWidget::connect(pMainWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pLogWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pTimeWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pMemoryWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pInputWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pCVarsWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pReflectionWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pSubsystemsWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pFileWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pPluginsWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(
    nullptr != QWidget::connect(pGlobalEventesWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pDataWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");
  WD_VERIFY(nullptr != QWidget::connect(pResourceWidget, &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");

  QMenu* pHistoryMenu = new QMenu;
  pHistoryMenu->setTearOffEnabled(true);
  pHistoryMenu->setTitle(QLatin1String("Stat Histories"));
  pHistoryMenu->setIcon(QIcon(":/Icons/Icons/StatHistory.png"));

  for (wdUInt32 i = 0; i < 10; ++i)
  {
    m_pStatHistoryWidgets[i] = new wdQtStatVisWidget(this, i);
    m_DockManager->addDockWidgetTab(ads::BottomDockWidgetArea, m_pStatHistoryWidgets[i]);

    WD_VERIFY(
      nullptr != QWidget::connect(m_pStatHistoryWidgets[i], &ads::CDockWidget::viewToggled, this, &wdQtMainWindow::DockWidgetVisibilityChanged), "");

    pHistoryMenu->addAction(&m_pStatHistoryWidgets[i]->m_ShowWindowAction);

    m_pActionShowStatIn[i] = new QAction(this);

    WD_VERIFY(nullptr != QWidget::connect(m_pActionShowStatIn[i], &QAction::triggered, wdQtMainWidget::s_pWidget, &wdQtMainWidget::ShowStatIn), "");
  }

  // delay this until after all widgets are created
  for (wdUInt32 i = 0; i < 10; ++i)
  {
    m_pStatHistoryWidgets[i]->toggleView(false); // hide
  }

  setContextMenuPolicy(Qt::NoContextMenu);

  menuWindows->addMenu(pHistoryMenu);

  pMemoryWidget->raise();

  m_DockManager->addDockWidget(ads::LeftDockWidgetArea, pMainWidget);
  m_DockManager->addDockWidget(ads::CenterDockWidgetArea, pLogWidget);

  m_DockManager->addDockWidget(ads::RightDockWidgetArea, pCVarsWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pGlobalEventesWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pDataWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pInputWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pPluginsWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pReflectionWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pResourceWidget);
  m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, pSubsystemsWidget);

  m_DockManager->addDockWidget(ads::BottomDockWidgetArea, pFileWidget);
  m_DockManager->addDockWidgetTab(ads::BottomDockWidgetArea, pMemoryWidget);
  m_DockManager->addDockWidgetTab(ads::BottomDockWidgetArea, pTimeWidget);


  pLogWidget->raise();
  pCVarsWidget->raise();

  if (bRestoreDockingState)
  {
    auto dockState = Settings.value("DockManagerState");
    if (dockState.isValid() && dockState.type() == QVariant::ByteArray)
    {
      m_DockManager->restoreState(dockState.toByteArray(), 1);
    }

    move(Settings.value("WindowPosition", pos()).toPoint());
    resize(Settings.value("WindowSize", size()).toSize());

    if (Settings.value("IsMaximized", isMaximized()).toBool())
    {
      showMaximized();
    }

    restoreState(Settings.value("WindowState", saveState()).toByteArray());
  }

  Settings.endGroup();

  for (wdInt32 i = 0; i < 10; ++i)
    m_pStatHistoryWidgets[i]->Load();

  SetupNetworkTimer();
}

wdQtMainWindow::~wdQtMainWindow()
{
  CloseTPApps();
  for (wdInt32 i = 0; i < 10; ++i)
  {
    m_pStatHistoryWidgets[i]->Save();
  }
  // The dock manager does not take ownership of dock widgets.
  auto dockWidgets = m_DockManager->dockWidgetsMap();
  for (auto it = dockWidgets.begin(); it != dockWidgets.end(); ++it)
  {
    m_DockManager->removeDockWidget(it.value());
    delete it.value();
  }
}

void wdQtMainWindow::closeEvent(QCloseEvent* pEvent)
{
  const bool bMaximized = isMaximized();
  if (bMaximized)
    showNormal();

  QSettings Settings;

  Settings.beginGroup("MainWindow");

  Settings.setValue("DockingVersion", g_iDockingStateVersion);
  Settings.setValue("DockManagerState", m_DockManager->saveState(1));
  Settings.setValue("WindowGeometry", saveGeometry());
  Settings.setValue("WindowState", saveState());
  Settings.setValue("IsMaximized", bMaximized);
  Settings.setValue("WindowPosition", pos());
  if (!bMaximized)
    Settings.setValue("WindowSize", size());

  Settings.endGroup();
}

void wdQtMainWindow::SetupNetworkTimer()
{
  // reset the timer to fire again
  if (m_pNetworkTimer == nullptr)
    m_pNetworkTimer = new QTimer(this);

  m_pNetworkTimer->singleShot(40, this, SLOT(UpdateNetworkTimeOut()));
}

void wdQtMainWindow::UpdateNetworkTimeOut()
{
  UpdateNetwork();

  SetupNetworkTimer();
}


void wdQtMainWindow::UpdateNetwork()
{
  bool bResetStats = false;

  {
    static wdUInt32 uiServerID = 0;
    static bool bConnected = false;
    static wdString sLastServerName;

    if (wdTelemetry::IsConnectedToServer())
    {
      if (uiServerID != wdTelemetry::GetServerID())
      {
        uiServerID = wdTelemetry::GetServerID();
        bResetStats = true;

        wdStringBuilder s;
        s.Format("Connected to new Server with ID {0}", uiServerID);

        wdQtLogDockWidget::s_pWidget->Log(s.GetData());
      }
      else if (!bConnected)
      {
        wdQtLogDockWidget::s_pWidget->Log("Reconnected to Server.");
      }

      if (sLastServerName != wdTelemetry::GetServerName())
      {
        sLastServerName = wdTelemetry::GetServerName();
        setWindowTitle(QString("wdInspector - %1").arg(sLastServerName.GetData()));
      }

      bConnected = true;
    }
    else
    {
      if (bConnected)
      {
        wdQtLogDockWidget::s_pWidget->Log("Lost Connection to Server.");
        setWindowTitle(QString("wdInspector - disconnected"));
        sLastServerName.Clear();
      }

      bConnected = false;
    }
  }

  if (bResetStats)
  {


    wdQtMainWidget::s_pWidget->ResetStats();
    wdQtLogDockWidget::s_pWidget->ResetStats();
    wdQtMemoryWidget::s_pWidget->ResetStats();
    wdQtTimeWidget::s_pWidget->ResetStats();
    wdQtInputWidget::s_pWidget->ResetStats();
    wdQtCVarsWidget::s_pWidget->ResetStats();
    wdQtReflectionWidget::s_pWidget->ResetStats();
    wdQtFileWidget::s_pWidget->ResetStats();
    wdQtPluginsWidget::s_pWidget->ResetStats();
    wdQtSubsystemsWidget::s_pWidget->ResetStats();
    wdQtGlobalEventsWidget::s_pWidget->ResetStats();
    wdQtDataWidget::s_pWidget->ResetStats();
    wdQtResourceWidget::s_pWidget->ResetStats();
  }

  UpdateAlwaysOnTop();

  wdQtMainWidget::s_pWidget->UpdateStats();
  wdQtPluginsWidget::s_pWidget->UpdateStats();
  wdQtSubsystemsWidget::s_pWidget->UpdateStats();
  wdQtMemoryWidget::s_pWidget->UpdateStats();
  wdQtTimeWidget::s_pWidget->UpdateStats();
  wdQtFileWidget::s_pWidget->UpdateStats();
  wdQtResourceWidget::s_pWidget->UpdateStats();
  // wdQtDataWidget::s_pWidget->UpdateStats();

  for (wdInt32 i = 0; i < 10; ++i)
    m_pStatHistoryWidgets[i]->UpdateStats();

  wdTelemetry::PerFrameUpdate();
}

void wdQtMainWindow::DockWidgetVisibilityChanged(bool bVisible)
{
  // TODO: add menu entry for qt main widget

  ActionShowWindowLog->setChecked(!wdQtLogDockWidget::s_pWidget->isClosed());
  ActionShowWindowMemory->setChecked(!wdQtMemoryWidget::s_pWidget->isClosed());
  ActionShowWindowTime->setChecked(!wdQtTimeWidget::s_pWidget->isClosed());
  ActionShowWindowInput->setChecked(!wdQtInputWidget::s_pWidget->isClosed());
  ActionShowWindowCVar->setChecked(!wdQtCVarsWidget::s_pWidget->isClosed());
  ActionShowWindowReflection->setChecked(!wdQtReflectionWidget::s_pWidget->isClosed());
  ActionShowWindowSubsystems->setChecked(!wdQtSubsystemsWidget::s_pWidget->isClosed());
  ActionShowWindowFile->setChecked(!wdQtFileWidget::s_pWidget->isClosed());
  ActionShowWindowPlugins->setChecked(!wdQtPluginsWidget::s_pWidget->isClosed());
  ActionShowWindowGlobalEvents->setChecked(!wdQtGlobalEventsWidget::s_pWidget->isClosed());
  ActionShowWindowData->setChecked(!wdQtDataWidget::s_pWidget->isClosed());
  ActionShowWindowResource->setChecked(!wdQtResourceWidget::s_pWidget->isClosed());

  for (wdInt32 i = 0; i < 10; ++i)
    m_pStatHistoryWidgets[i]->m_ShowWindowAction.setChecked(!m_pStatHistoryWidgets[i]->isClosed());
}


void wdQtMainWindow::SetAlwaysOnTop(OnTopMode Mode)
{
  m_OnTopMode = Mode;

  QSettings Settings;
  Settings.setValue("AlwaysOnTop", (int)m_OnTopMode);

  ActionNeverOnTop->setChecked((m_OnTopMode == Never) ? Qt::Checked : Qt::Unchecked);
  ActionAlwaysOnTop->setChecked((m_OnTopMode == Always) ? Qt::Checked : Qt::Unchecked);
  ActionOnTopWhenConnected->setChecked((m_OnTopMode == WhenConnected) ? Qt::Checked : Qt::Unchecked);

  UpdateAlwaysOnTop();
}

void wdQtMainWindow::UpdateAlwaysOnTop()
{
  static bool bOnTop = false;

  bool bNewState = bOnTop;

  if (m_OnTopMode == Always || (m_OnTopMode == WhenConnected && wdTelemetry::IsConnectedToServer()))
    bNewState = true;
  else
    bNewState = false;

  if (bOnTop != bNewState)
  {
    bOnTop = bNewState;

    hide();

    if (bOnTop)
      setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    else
      setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint | Qt::WindowStaysOnBottomHint);

    show();
  }
}

void wdQtMainWindow::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage Msg;

  while (wdTelemetry::RetrieveMessage(' APP', Msg) == WD_SUCCESS)
  {
    switch (Msg.GetMessageID())
    {
      case 'ASRT':
      {
        wdString sSourceFile, sFunction, sExpression, sMessage;
        wdUInt32 uiLine = 0;

        Msg.GetReader() >> sSourceFile;
        Msg.GetReader() >> uiLine;
        Msg.GetReader() >> sFunction;
        Msg.GetReader() >> sExpression;
        Msg.GetReader() >> sMessage;

        wdQtLogDockWidget::s_pWidget->Log("");
        wdQtLogDockWidget::s_pWidget->Log("<<< Application Assertion >>>");
        wdQtLogDockWidget::s_pWidget->Log("");

        wdQtLogDockWidget::s_pWidget->Log(wdFmt("    Expression: '{0}'", sExpression));
        wdQtLogDockWidget::s_pWidget->Log("");

        wdQtLogDockWidget::s_pWidget->Log(wdFmt("    Message: '{0}'", sMessage));
        wdQtLogDockWidget::s_pWidget->Log("");

        wdQtLogDockWidget::s_pWidget->Log(wdFmt("   File: '{0}'", sSourceFile));

        wdQtLogDockWidget::s_pWidget->Log(wdFmt("   Line: {0}", uiLine));

        wdQtLogDockWidget::s_pWidget->Log(wdFmt("   In Function: '{0}'", sFunction));

        wdQtLogDockWidget::s_pWidget->Log("");

        wdQtLogDockWidget::s_pWidget->Log(">>> Application Assertion <<<");
        wdQtLogDockWidget::s_pWidget->Log("");
      }
      break;
    }
  }
}
