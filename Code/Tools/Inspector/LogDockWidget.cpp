#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Logging/LogEntry.h>
#include <GuiFoundation/Models/LogModel.moc.h>
#include <Inspector/LogDockWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <qlistwidget.h>

wdQtLogDockWidget* wdQtLogDockWidget::s_pWidget = nullptr;

wdQtLogDockWidget::wdQtLogDockWidget(QWidget* pParent)
  : ads::CDockWidget("Log", pParent)
{
  s_pWidget = this;
  setupUi(this);
  LogWidget->GetSearchWidget()->setPlaceholderText(QStringLiteral("Search Log"));

  this->setWidget(LogWidget);
}

void wdQtLogDockWidget::ResetStats()
{
  LogWidget->GetLog()->Clear();
}

void wdQtLogDockWidget::Log(const wdFormatString& text)
{
  wdStringBuilder tmp;

  wdLogEntry lm;
  lm.m_sMsg = text.GetText(tmp);
  lm.m_Type = wdLogMsgType::InfoMsg;
  lm.m_uiIndentation = 0;
  LogWidget->GetLog()->AddLogMsg(lm);
}

void wdQtLogDockWidget::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage Msg;

  while (wdTelemetry::RetrieveMessage(' LOG', Msg) == WD_SUCCESS)
  {
    wdLogEntry lm;
    wdInt8 iEventType = 0;

    Msg.GetReader() >> iEventType;
    Msg.GetReader() >> lm.m_uiIndentation;
    Msg.GetReader() >> lm.m_sTag;
    Msg.GetReader() >> lm.m_sMsg;

    if (iEventType == wdLogMsgType::EndGroup)
      Msg.GetReader() >> lm.m_fSeconds;

    lm.m_Type = (wdLogMsgType::Enum)iEventType;
    s_pWidget->LogWidget->GetLog()->AddLogMsg(lm);
  }
}
