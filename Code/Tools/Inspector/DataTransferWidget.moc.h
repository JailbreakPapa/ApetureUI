#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_DataTransferWidget.h>
#include <ads/DockWidget.h>

class wdQtDataWidget : public ads::CDockWidget, public Ui_DataTransferWidget
{
public:
  Q_OBJECT

public:
  wdQtDataWidget(QWidget* pParent = 0);

  static wdQtDataWidget* s_pWidget;

private Q_SLOTS:
  virtual void on_ButtonRefresh_clicked();
  virtual void on_ComboTransfers_currentIndexChanged(int index);
  virtual void on_ComboItems_currentIndexChanged(int index);
  virtual void on_ButtonSave_clicked();
  virtual void on_ButtonOpen_clicked();

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();

private:
  struct TransferDataObject
  {
    wdString m_sMimeType;
    wdString m_sExtension;
    wdContiguousMemoryStreamStorage m_Storage;
    wdString m_sFileName;
  };

  struct TransferData
  {
    wdMap<wdString, TransferDataObject> m_Items;
  };

  bool SaveToFile(TransferDataObject& item, const char* szFile);

  TransferDataObject* GetCurrentItem();
  TransferData* GetCurrentTransfer();

  wdMap<wdString, TransferData> m_Transfers;
};

