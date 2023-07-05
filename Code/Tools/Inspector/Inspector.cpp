#include <Inspector/InspectorPCH.h>

#include <Foundation/Application/Application.h>
#include <Foundation/Communication/Telemetry.h>
#include <GuiFoundation/Theme/ApplicationTheme.h>
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
#include <QApplication>
#include <QSettings>
#include <qstylefactory.h>

class wdInspectorApp : public wdApplication
{
public:
  typedef wdApplication SUPER;

  wdInspectorApp()
    : wdApplication("wdInspector")
  {
  }



  virtual wdResult BeforeCoreSystemsStartup() override
  {
    wdStartup::AddApplicationTag("tool");
    wdStartup::AddApplicationTag("inspector");

    return wdApplication::BeforeCoreSystemsStartup();
  }

  virtual Execution Run() override
  {
    int iArgs = GetArgumentCount();
    char** cArgs = (char**)GetArgumentsArray();

    QApplication app(iArgs, cArgs);
    QCoreApplication::setOrganizationName("FEAR Engine");
    QCoreApplication::setApplicationName("wdInspector");
    QCoreApplication::setApplicationVersion("1.0.0");

    wdApplicationTheme::LoadBasePalette(app);
    wdQtMainWindow MainWindow;
 
    wdTelemetry::AcceptMessagesForSystem('CVAR', true, wdQtCVarsWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('CMD', true, wdQtCVarsWidget::ProcessTelemetryConsole, nullptr);
    wdTelemetry::AcceptMessagesForSystem(' LOG', true, wdQtLogDockWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem(' MEM', true, wdQtMemoryWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('TIME', true, wdQtTimeWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem(' APP', true, wdQtMainWindow::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('FILE', true, wdQtFileWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('INPT', true, wdQtInputWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('STRT', true, wdQtSubsystemsWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('STAT', true, wdQtMainWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('PLUG', true, wdQtPluginsWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('EVNT', true, wdQtGlobalEventsWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('RFLC', true, wdQtReflectionWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('TRAN', true, wdQtDataWidget::ProcessTelemetry, nullptr);
    wdTelemetry::AcceptMessagesForSystem('RESM', true, wdQtResourceWidget::ProcessTelemetry, nullptr);

    QSettings Settings;
    const QString sServer = Settings.value("LastConnection", QLatin1String("localhost:1040")).toString();

    wdTelemetry::ConnectToServer(sServer.toUtf8().data()).IgnoreResult();

    MainWindow.show();
    SetReturnCode(app.exec());

    wdTelemetry::CloseConnection();

    return wdApplication::Execution::Quit;
  }
};

WD_APPLICATION_ENTRY_POINT(wdInspectorApp);
