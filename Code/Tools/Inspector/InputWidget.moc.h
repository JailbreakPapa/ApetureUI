#pragma once

#include <Core/Input/InputManager.h>
#include <Foundation/Basics.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_InputWidget.h>
#include <ads/DockWidget.h>

class wdQtInputWidget : public ads::CDockWidget, public Ui_InputWidget
{
public:
  Q_OBJECT

public:
  wdQtInputWidget(QWidget* pParent = 0);

  static wdQtInputWidget* s_pWidget;

private Q_SLOTS:
  virtual void on_ButtonClearSlots_clicked();
  virtual void on_ButtonClearActions_clicked();

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();

private:
  void ClearSlots();
  void ClearActions();

  void UpdateSlotTable(bool bRecreate);
  void UpdateActionTable(bool bRecreate);

  struct SlotData
  {
    wdInt32 m_iTableRow;
    wdUInt16 m_uiSlotFlags;
    wdKeyState::Enum m_KeyState;
    float m_fValue;
    float m_fDeadZone;

    SlotData()
    {
      m_iTableRow = -1;
      m_uiSlotFlags = 0;
      m_KeyState = wdKeyState::Up;
      m_fValue = 0;
      m_fDeadZone = 0;
    }
  };

  wdMap<wdString, SlotData> m_InputSlots;

  struct ActionData
  {
    wdInt32 m_iTableRow;
    wdKeyState::Enum m_KeyState;
    float m_fValue;
    bool m_bUseTimeScaling;

    wdString m_sTrigger[wdInputActionConfig::MaxInputSlotAlternatives];
    float m_fTriggerScaling[wdInputActionConfig::MaxInputSlotAlternatives];

    ActionData()
    {
      m_iTableRow = -1;
      m_KeyState = wdKeyState::Up;
      m_fValue = 0;
      m_bUseTimeScaling = false;
    }
  };

  wdMap<wdString, ActionData> m_InputActions;
};

