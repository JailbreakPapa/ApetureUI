#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <GuiFoundation/UIServices/UIServices.moc.h>
#include <Inspector/CVarsWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qspinbox.h>

class wdCommandInterpreterInspector : public wdCommandInterpreter
{
public:
  virtual void Interpret(wdCommandInterpreterState& inout_state) override
  {
    wdTelemetryMessage Msg;
    Msg.SetMessageID('CMD', 'EXEC');
    Msg.GetWriter() << inout_state.m_sInput;
    wdTelemetry::SendToServer(Msg);
  }

  virtual void AutoComplete(wdCommandInterpreterState& inout_state) override
  {
    wdTelemetryMessage Msg;
    Msg.SetMessageID('CMD', 'COMP');
    Msg.GetWriter() << inout_state.m_sInput;
    wdTelemetry::SendToServer(Msg);
  }
};

wdQtCVarsWidget* wdQtCVarsWidget::s_pWidget = nullptr;

wdQtCVarsWidget::wdQtCVarsWidget(QWidget* pParent)
  : ads::CDockWidget("CVars", pParent)
{
  s_pWidget = this;

  setupUi(this);
  setWidget(CVarWidget);

  connect(CVarWidget, &wdQtCVarWidget::onBoolChanged, this, &wdQtCVarsWidget::BoolChanged);
  connect(CVarWidget, &wdQtCVarWidget::onFloatChanged, this, &wdQtCVarsWidget::FloatChanged);
  connect(CVarWidget, &wdQtCVarWidget::onIntChanged, this, &wdQtCVarsWidget::IntChanged);
  connect(CVarWidget, &wdQtCVarWidget::onStringChanged, this, &wdQtCVarsWidget::StringChanged);

  CVarWidget->GetConsole().SetCommandInterpreter(WD_DEFAULT_NEW(wdCommandInterpreterInspector));

  ResetStats();
}

void wdQtCVarsWidget::ResetStats()
{
  m_CVarsBackup = m_CVars;
  m_CVars.Clear();
  CVarWidget->Clear();
}

void wdQtCVarsWidget::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage msg;

  bool bUpdateCVarsTable = false;
  bool bFillCVarsTable = false;

  while (wdTelemetry::RetrieveMessage('CVAR', msg) == WD_SUCCESS)
  {
    if (msg.GetMessageID() == ' CLR')
    {
      s_pWidget->m_CVars.Clear();
    }

    if (msg.GetMessageID() == 'SYNC')
    {
      for (auto it = s_pWidget->m_CVars.GetIterator(); it.IsValid(); ++it)
      {
        auto var = s_pWidget->m_CVarsBackup.Find(it.Key());

        if (var.IsValid() && it.Value().m_uiType == var.Value().m_uiType)
        {
          it.Value().m_bValue = var.Value().m_bValue;
          it.Value().m_fValue = var.Value().m_fValue;
          it.Value().m_sValue = var.Value().m_sValue;
          it.Value().m_iValue = var.Value().m_iValue;
        }
      }

      s_pWidget->CVarWidget->RebuildCVarUI(s_pWidget->m_CVars);

      s_pWidget->SyncAllCVarsToServer();
    }

    if (msg.GetMessageID() == 'DATA')
    {
      wdString sName;
      msg.GetReader() >> sName;

      wdCVarWidgetData& sd = s_pWidget->m_CVars[sName];

      msg.GetReader() >> sd.m_sPlugin;
      msg.GetReader() >> sd.m_uiType;
      msg.GetReader() >> sd.m_sDescription;

      switch (sd.m_uiType)
      {
        case wdCVarType::Bool:
          msg.GetReader() >> sd.m_bValue;
          break;
        case wdCVarType::Float:
          msg.GetReader() >> sd.m_fValue;
          break;
        case wdCVarType::Int:
          msg.GetReader() >> sd.m_iValue;
          break;
        case wdCVarType::String:
          msg.GetReader() >> sd.m_sValue;
          break;
      }

      if (sd.m_bNewEntry)
        bUpdateCVarsTable = true;

      bFillCVarsTable = true;
    }
  }

  if (bUpdateCVarsTable)
    s_pWidget->CVarWidget->RebuildCVarUI(s_pWidget->m_CVars);
  else if (bFillCVarsTable)
    s_pWidget->CVarWidget->UpdateCVarUI(s_pWidget->m_CVars);
}

void wdQtCVarsWidget::ProcessTelemetryConsole(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage msg;
  wdStringBuilder tmp;

  while (wdTelemetry::RetrieveMessage('CMD', msg) == WD_SUCCESS)
  {
    if (msg.GetMessageID() == 'RES')
    {
      msg.GetReader() >> tmp;
      s_pWidget->CVarWidget->AddConsoleStrings(tmp);
    }
  }
}

void wdQtCVarsWidget::SyncAllCVarsToServer()
{
  for (auto it = m_CVars.GetIterator(); it.IsValid(); ++it)
    SendCVarUpdateToServer(it.Key().GetData(), it.Value());
}

void wdQtCVarsWidget::SendCVarUpdateToServer(const char* szName, const wdCVarWidgetData& cvd)
{
  wdTelemetryMessage Msg;
  Msg.SetMessageID('SVAR', ' SET');
  Msg.GetWriter() << szName;
  Msg.GetWriter() << cvd.m_uiType;

  switch (cvd.m_uiType)
  {
    case wdCVarType::Bool:
      Msg.GetWriter() << cvd.m_bValue;
      break;

    case wdCVarType::Float:
      Msg.GetWriter() << cvd.m_fValue;
      break;

    case wdCVarType::Int:
      Msg.GetWriter() << cvd.m_iValue;
      break;

    case wdCVarType::String:
      Msg.GetWriter() << cvd.m_sValue;
      break;
  }

  wdTelemetry::SendToServer(Msg);
}

void wdQtCVarsWidget::BoolChanged(const char* szCVar, bool newValue)
{
  auto& cvarData = m_CVars[szCVar];
  cvarData.m_bValue = newValue;
  SendCVarUpdateToServer(szCVar, cvarData);
}

void wdQtCVarsWidget::FloatChanged(const char* szCVar, float newValue)
{
  auto& cvarData = m_CVars[szCVar];
  cvarData.m_fValue = newValue;
  SendCVarUpdateToServer(szCVar, cvarData);
}

void wdQtCVarsWidget::IntChanged(const char* szCVar, int newValue)
{
  auto& cvarData = m_CVars[szCVar];
  cvarData.m_iValue = newValue;
  SendCVarUpdateToServer(szCVar, cvarData);
}

void wdQtCVarsWidget::StringChanged(const char* szCVar, const char* newValue)
{
  auto& cvarData = m_CVars[szCVar];
  cvarData.m_sValue = newValue;
  SendCVarUpdateToServer(szCVar, cvarData);
}
