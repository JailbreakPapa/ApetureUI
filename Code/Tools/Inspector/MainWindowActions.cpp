#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Inspector/CVarsWidget.moc.h>
#include <Inspector/DataTransferWidget.moc.h>
#include <Inspector/FileWidget.moc.h>
#include <Inspector/GlobalEventsWidget.moc.h>
#include <Inspector/InputWidget.moc.h>
#include <Inspector/LogDockWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <Inspector/MemoryWidget.moc.h>
#include <Inspector/PluginsWidget.moc.h>
#include <Inspector/ReflectionWidget.moc.h>
#include <Inspector/ResourceWidget.moc.h>
#include <Inspector/SubsystemsWidget.moc.h>
#include <Inspector/TimeWidget.moc.h>

void wdQtMainWindow::on_ActionShowWindowLog_triggered()
{
  wdQtLogDockWidget::s_pWidget->toggleView(ActionShowWindowLog->isChecked());
  wdQtLogDockWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowMemory_triggered()
{
  wdQtMemoryWidget::s_pWidget->toggleView(ActionShowWindowMemory->isChecked());
  wdQtMemoryWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowTime_triggered()
{
  wdQtTimeWidget::s_pWidget->toggleView(ActionShowWindowTime->isChecked());
  wdQtTimeWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowInput_triggered()
{
  wdQtInputWidget::s_pWidget->toggleView(ActionShowWindowInput->isChecked());
  wdQtInputWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowCVar_triggered()
{
  wdQtCVarsWidget::s_pWidget->toggleView(ActionShowWindowCVar->isChecked());
  wdQtCVarsWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowReflection_triggered()
{
  wdQtReflectionWidget::s_pWidget->toggleView(ActionShowWindowReflection->isChecked());
  wdQtReflectionWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowSubsystems_triggered()
{
  wdQtSubsystemsWidget::s_pWidget->toggleView(ActionShowWindowSubsystems->isChecked());
  wdQtSubsystemsWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowPlugins_triggered()
{
  wdQtPluginsWidget::s_pWidget->toggleView(ActionShowWindowPlugins->isChecked());
  wdQtPluginsWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowFile_triggered()
{
  wdQtFileWidget::s_pWidget->toggleView(ActionShowWindowFile->isChecked());
  wdQtFileWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowGlobalEvents_triggered()
{
  wdQtGlobalEventsWidget::s_pWidget->toggleView(ActionShowWindowGlobalEvents->isChecked());
  wdQtGlobalEventsWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowData_triggered()
{
  wdQtDataWidget::s_pWidget->toggleView(ActionShowWindowData->isChecked());
  wdQtDataWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionShowWindowResource_triggered()
{
  wdQtResourceWidget::s_pWidget->toggleView(ActionShowWindowResource->isChecked());
  wdQtResourceWidget::s_pWidget->raise();
}

void wdQtMainWindow::on_ActionOnTopWhenConnected_triggered()
{
  SetAlwaysOnTop(WhenConnected);
}

void wdQtMainWindow::on_ActionAlwaysOnTop_triggered()
{
  SetAlwaysOnTop(Always);
}

void wdQtMainWindow::on_ActionNeverOnTop_triggered()
{
  SetAlwaysOnTop(Never);
}
void wdQtMainWindow::on_ActionLivePP_triggered()
{
  livepp->start("LPP_Broker.exe");
}

void wdQtMainWindow::on_ActionOptick_triggered()
{
  /// Opens Optick. Since Inspector Requires OptickCore, it will configure OptickCore first, in which that Copy's The Profiler Over To Output.
  optick->start("Optick.exe");
  /// TODO: Add Check for if the application fully opened, if not, (If in Dev/Debug mode) head to the hard directory where optick is, and launch the application from there.
  /// If that doesn't work, report a error message to the user.
}


