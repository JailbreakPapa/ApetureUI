#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/String.h>
#include <Inspector/ui_ReflectionWidget.h>
#include <ads/DockWidget.h>

class wdQtReflectionWidget : public ads::CDockWidget, public Ui_ReflectionWidget
{
public:
  Q_OBJECT

public:
  wdQtReflectionWidget(QWidget* pParent = 0);

  static wdQtReflectionWidget* s_pWidget;

private Q_SLOTS:

public:
  static void ProcessTelemetry(void* pUnuseed);

  void ResetStats();

private:
  struct PropertyData
  {
    wdString m_sType;
    wdString m_sPropertyName;
    wdInt8 m_iCategory;
  };

  struct TypeData
  {
    TypeData() { m_pTreeItem = nullptr; }

    QTreeWidgetItem* m_pTreeItem;

    wdUInt32 m_uiSize;
    wdString m_sParentType;
    wdString m_sPlugin;

    wdHybridArray<PropertyData, 16> m_Properties;
  };

  bool UpdateTree();

  wdMap<wdString, TypeData> m_Types;
};

