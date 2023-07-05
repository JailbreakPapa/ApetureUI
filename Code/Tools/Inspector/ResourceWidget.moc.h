#pragma once

#include <Core/ResourceManager/Resource.h>
#include <Foundation/Basics.h>
#include <Foundation/Containers/Set.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Time/Time.h>
#include <Inspector/ui_ResourceWidget.h>
#include <ads/DockWidget.h>

class wdQtResourceWidget : public ads::CDockWidget, public Ui_ResourceWidget
{
public:
  Q_OBJECT

public:
  wdQtResourceWidget(QWidget* pParent = 0);

  static wdQtResourceWidget* s_pWidget;

private Q_SLOTS:

  void on_LineFilterByName_textChanged();
  void on_ComboResourceTypes_currentIndexChanged(int state);
  void on_CheckShowDeleted_toggled(bool checked);
  void on_ButtonSave_clicked();

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();
  void UpdateStats();

  void UpdateTable();

private:
  void UpdateAll();

  struct ResourceData
  {
    ResourceData()
    {
      m_pMainItem = nullptr;
      m_bUpdate = true;
    }

    bool m_bUpdate;
    QTableWidgetItem* m_pMainItem;
    wdString m_sResourceID;
    wdString m_sResourceType;
    wdResourcePriority m_Priority;
    wdBitflags<wdResourceFlags> m_Flags;
    wdResourceLoadDesc m_LoadingState;
    wdResource::MemoryUsage m_Memory;
    wdString m_sResourceDescription;
  };

  bool m_bShowDeleted;
  wdString m_sTypeFilter;
  wdString m_sNameFilter;
  wdTime m_LastTableUpdate;
  bool m_bUpdateTable;

  bool m_bUpdateTypeBox;
  wdSet<wdString> m_ResourceTypes;
  wdHashTable<wdUInt64, ResourceData> m_Resources;
};

