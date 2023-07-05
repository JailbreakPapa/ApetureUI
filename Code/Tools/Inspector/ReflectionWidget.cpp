#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Inspector/ReflectionWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <qlistwidget.h>

wdQtReflectionWidget* wdQtReflectionWidget::s_pWidget = nullptr;

wdQtReflectionWidget::wdQtReflectionWidget(QWidget* pParent)
  : ads::CDockWidget("Reflection Widget", pParent)
{
  s_pWidget = this;

  setupUi(this);
  setWidget(ReflectionWidgetFrame);

  ResetStats();
}

void wdQtReflectionWidget::ResetStats()
{
  TypeTree->clear();

  {
    QStringList Headers;
    Headers.append(" Type ");
    Headers.append(" Property ");
    Headers.append(" Size (Bytes) ");
    Headers.append(" Plugin ");

    TypeTree->setColumnCount(Headers.size());
    TypeTree->setHeaderLabels(Headers);
    TypeTree->header()->show();
  }
}

void wdQtReflectionWidget::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage msg;

  bool bUpdate = false;

  while (wdTelemetry::RetrieveMessage('RFLC', msg) == WD_SUCCESS)
  {
    if (msg.GetMessageID() == ' CLR')
    {
      s_pWidget->m_Types.Clear();
      s_pWidget->TypeTree->clear();
    }

    if (msg.GetMessageID() == 'DATA')
    {
      bUpdate = true;

      wdString sName;
      msg.GetReader() >> sName;

      TypeData& sd = s_pWidget->m_Types[sName];

      msg.GetReader() >> sd.m_sParentType;
      msg.GetReader() >> sd.m_uiSize;
      msg.GetReader() >> sd.m_sPlugin;

      {
        wdUInt32 num = 0;
        msg.GetReader() >> num;

        for (wdUInt32 i = 0; i < num; ++i)
        {
          PropertyData pd;
          msg.GetReader() >> pd.m_sPropertyName;
          msg.GetReader() >> pd.m_iCategory;
          msg.GetReader() >> pd.m_sType;

          for (wdUInt32 j = 0; j < sd.m_Properties.GetCount(); ++j)
          {
            if (sd.m_Properties[j].m_sPropertyName == pd.m_sPropertyName)
              goto found;
          }

          sd.m_Properties.PushBack(pd);

        found:
          continue;
        }
      }

      /// \todo should read the message handlers here
    }
  }

  if (bUpdate)
  {
    while (s_pWidget->UpdateTree())
    {
    }

    s_pWidget->TypeTree->resizeColumnToContents(3);
    s_pWidget->TypeTree->resizeColumnToContents(2);
    s_pWidget->TypeTree->resizeColumnToContents(1);
    s_pWidget->TypeTree->resizeColumnToContents(0);
  }
}

bool wdQtReflectionWidget::UpdateTree()
{
  bool bAddedAny = false;

  for (auto it = m_Types.GetIterator(); it.IsValid(); ++it)
  {
    if (it.Value().m_pTreeItem == nullptr)
    {
      bAddedAny = true;

      // has parent type, but parent type has not been inserted into tree yet
      if (!it.Value().m_sParentType.IsEmpty() && m_Types[it.Value().m_sParentType].m_pTreeItem == nullptr)
        continue;

      QTreeWidgetItem* pParent = (it.Value().m_sParentType.IsEmpty()) ? nullptr : m_Types[it.Value().m_sParentType].m_pTreeItem;

      QTreeWidgetItem* pItem = new QTreeWidgetItem();
      it.Value().m_pTreeItem = pItem;

      wdStringBuilder sText;
      sText.Format("{0}", it.Value().m_uiSize);

      pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      pItem->setText(0, it.Key().GetData());
      pItem->setText(2, sText.GetData());
      pItem->setText(3, it.Value().m_sPlugin.GetData());

      pItem->setIcon(0, QIcon(":/Icons/Icons/Type.png"));

      if (pParent)
      {
        pParent->addChild(pItem);
        pParent->setExpanded(false);
      }
      else
      {
        TypeTree->addTopLevelItem(pItem);
      }

      for (wdUInt32 i = 0; i < it.Value().m_Properties.GetCount(); ++i)
      {
        QTreeWidgetItem* pProperty = new QTreeWidgetItem();
        pProperty->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        switch (it.Value().m_Properties[i].m_iCategory)
        {
          case -1:
            pProperty->setText(0, "Message");
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Message.png"));
            break;
          case wdPropertyCategory::Member:
            pProperty->setText(0, it.Value().m_Properties[i].m_sType.GetData());
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Member.png"));
            break;
          case wdPropertyCategory::Function:
            pProperty->setText(0, "Function");
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Function.png"));
            break;
          case wdPropertyCategory::Array:
            pProperty->setText(0, it.Value().m_Properties[i].m_sType.GetData());
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Array.png"));
            break;
          case wdPropertyCategory::Set:
            pProperty->setText(0, it.Value().m_Properties[i].m_sType.GetData());
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Set.png"));
            break;
          case wdPropertyCategory::Map:
            pProperty->setText(0, it.Value().m_Properties[i].m_sType.GetData());
            pProperty->setIcon(0, QIcon(":/Icons/Icons/Map.png"));
            break;
        }

        pProperty->setText(1, it.Value().m_Properties[i].m_sPropertyName.GetData());

        pItem->addChild(pProperty);
      }
    }
  }

  return bAddedAny;
}
