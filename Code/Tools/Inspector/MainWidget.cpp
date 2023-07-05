#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Utilities/ConversionUtils.h>
#include <Inspector/MainWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <QDir>
#include <QMenu>
#include <QSettings>
#include <QStandardPaths>

wdQtMainWidget* wdQtMainWidget::s_pWidget = nullptr;

wdQtMainWidget::wdQtMainWidget(QWidget* pParent)
  : ads::CDockWidget("Main", pParent)
{
  s_pWidget = this;

  setupUi(this);
  setWidget(MainWidgetFrame);

  this->setFeature(ads::CDockWidget::DockWidgetClosable, false);

  m_uiMaxStatSamples = 20000; // should be enough for 5 minutes of history at 60 Hz

  setContextMenuPolicy(Qt::NoContextMenu);

  TreeStats->setContextMenuPolicy(Qt::CustomContextMenu);

  ResetStats();

  LoadFavorites();

  QSettings Settings;
  Settings.beginGroup("MainWidget");

  splitter->restoreState(Settings.value("SplitterState", splitter->saveState()).toByteArray());
  splitter->restoreGeometry(Settings.value("SplitterSize", splitter->saveGeometry()).toByteArray());

  Settings.endGroup();
}

wdQtMainWidget::~wdQtMainWidget()
{
  SaveFavorites();
}

void wdQtMainWidget::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage Msg;

  while (wdTelemetry::RetrieveMessage('STAT', Msg) == WD_SUCCESS)
  {
    switch (Msg.GetMessageID())
    {
      case ' DEL':
      {
        wdString sStatName;
        Msg.GetReader() >> sStatName;

        wdMap<wdString, StatData>::Iterator it = s_pWidget->m_Stats.Find(sStatName);

        if (!it.IsValid())
          break;

        if (it.Value().m_pItem)
          delete it.Value().m_pItem;

        if (it.Value().m_pItemFavorite)
          delete it.Value().m_pItemFavorite;

        s_pWidget->m_Stats.Remove(it);
      }
      break;

      case ' SET':
      {
        wdString sStatName;
        Msg.GetReader() >> sStatName;

        StatData& sd = s_pWidget->m_Stats[sStatName];

        Msg.GetReader() >> sd.m_Value;

        StatSample ss;
        ss.m_Value = sd.m_Value.ConvertTo<double>();
        Msg.GetReader() >> ss.m_AtGlobalTime;

        sd.m_History.PushBack(ss);

        s_pWidget->m_MaxGlobalTime = wdMath::Max(s_pWidget->m_MaxGlobalTime, ss.m_AtGlobalTime);

        // remove excess samples
        if (sd.m_History.GetCount() > s_pWidget->m_uiMaxStatSamples)
          sd.m_History.PopFront(sd.m_History.GetCount() - s_pWidget->m_uiMaxStatSamples);

        if (sd.m_pItem == nullptr)
        {
          sd.m_pItem = s_pWidget->CreateStat(sStatName.GetData(), false);

          if (s_pWidget->m_Favorites.Find(sStatName).IsValid())
            sd.m_pItem->setCheckState(0, Qt::Checked);
        }

        const wdString sValue = sd.m_Value.ConvertTo<wdString>();
        sd.m_pItem->setData(1, Qt::DisplayRole, sValue.GetData());

        if (sd.m_pItemFavorite)
          sd.m_pItemFavorite->setData(1, Qt::DisplayRole, sValue.GetData());
      }
      break;
    }
  }
}

void wdQtMainWidget::on_ButtonConnect_clicked()
{
  QSettings Settings;
  const QString sServer = Settings.value("LastConnection", QLatin1String("localhost:1040")).toString();

  bool bOk = false;
  QString sRes = QInputDialog::getText(this, "Host", "Host Name or IP Address:\nDefault is 'localhost:1040'", QLineEdit::Normal, sServer, &bOk);

  if (!bOk)
    return;

  Settings.setValue("LastConnection", sRes);

  if (wdTelemetry::ConnectToServer(sRes.toUtf8().data()) == WD_SUCCESS)
  {
  }
}

void wdQtMainWidget::SaveFavorites()
{
  QString sFile = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  QDir dir;
  dir.mkpath(sFile);

  sFile.append("/Favourites.stats");

  QFile f(sFile);
  if (!f.open(QIODevice::WriteOnly))
    return;

  QDataStream stream(&f);

  const wdUInt32 uiNumFavorites = m_Favorites.GetCount();
  stream << uiNumFavorites;

  for (wdSet<wdString>::Iterator it = m_Favorites.GetIterator(); it.IsValid(); ++it)
  {
    const QString s = it.Key().GetData();
    stream << s;
  }

  f.close();
}

void wdQtMainWidget::LoadFavorites()
{
  QString sFile = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  QDir dir;
  dir.mkpath(sFile);
  sFile.append("/Favourites.stats");

  QFile f(sFile);
  if (!f.open(QIODevice::ReadOnly))
    return;

  m_Favorites.Clear();

  QDataStream stream(&f);

  wdUInt32 uiNumFavorites = 0;
  stream >> uiNumFavorites;

  for (wdUInt32 i = 0; i < uiNumFavorites; ++i)
  {
    QString s;
    stream >> s;

    wdString wds = s.toUtf8().data();

    m_Favorites.Insert(wds);
  }

  f.close();
}

void wdQtMainWidget::ResetStats()
{
  m_Stats.Clear();
  TreeStats->clear();
  TreeFavorites->clear();
}

void wdQtMainWidget::UpdateStats()
{
  static bool bWasConnected = false;
  const bool bIsConnected = wdTelemetry::IsConnectedToServer();

  if (bIsConnected)
    LabelPing->setText(QString::fromUtf8("<p>Ping: %1ms</p>").arg((wdUInt32)wdTelemetry::GetPingToServer().GetMilliseconds()));

  if (bWasConnected == bIsConnected)
    return;

  bWasConnected = bIsConnected;

  if (!bIsConnected)
  {
    LabelPing->setText("<p>Ping: N/A</p>");
    LabelStatus->setText(
      "<p><span style=\" font-weight:600;\">Status: </span><span style=\" font-weight:600; color:#ff0000;\">Not Connected</span></p>");
    LabelServer->setText("<p>Server: N/A</p>");
  }
  else
  {
    LabelStatus->setText("<p><span style=\" font-weight:600;\">Status: </span><span style=\" font-weight:600; color:#00aa00;\">Connected</span></p>");
    LabelServer->setText(QString::fromUtf8("<p>Server: %1:%2</p>").arg(wdTelemetry::GetServerIP()).arg(wdTelemetry::s_uiPort));
  }
}


void wdQtMainWidget::closeEvent(QCloseEvent* pEvent)
{
  QSettings Settings;

  Settings.beginGroup("MainWidget");

  Settings.setValue("SplitterState", splitter->saveState());
  Settings.setValue("SplitterGeometry", splitter->saveGeometry());

  Settings.endGroup();
}

QTreeWidgetItem* wdQtMainWidget::CreateStat(const char* szPath, bool bParent)
{
  wdStringBuilder sCleanPath = szPath;
  if (sCleanPath.EndsWith("/"))
    sCleanPath.Shrink(0, 1);

  wdMap<wdString, StatData>::Iterator it = m_Stats.Find(sCleanPath.GetData());

  if (it.IsValid() && it.Value().m_pItem != nullptr)
    return it.Value().m_pItem;

  QTreeWidgetItem* pParent = nullptr;
  StatData& sd = m_Stats[sCleanPath.GetData()];

  {
    wdStringBuilder sParentPath = sCleanPath.GetData();
    sParentPath.PathParentDirectory(1);

    sd.m_pItem = new QTreeWidgetItem();
    sd.m_pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | (bParent ? Qt::NoItemFlags : Qt::ItemIsUserCheckable));
    sd.m_pItem->setData(0, Qt::UserRole, QString(sCleanPath.GetData()));

    if (bParent)
      sd.m_pItem->setIcon(0, QIcon(":/Icons/Icons/StatGroup.png"));
    else
      sd.m_pItem->setIcon(0, QIcon(":/Icons/Icons/Stat.png"));

    if (!bParent)
      sd.m_pItem->setCheckState(0, Qt::Unchecked);

    if (!sParentPath.IsEmpty())
    {
      pParent = CreateStat(sParentPath.GetData(), true);
      pParent->addChild(sd.m_pItem);
      pParent->setExpanded(false);
    }
    else
    {
      TreeStats->addTopLevelItem(sd.m_pItem);
    }
  }

  {
    wdString sFileName = sCleanPath.GetFileName();
    sd.m_pItem->setData(0, Qt::DisplayRole, sFileName.GetData());

    if (pParent)
      pParent->sortChildren(0, Qt::AscendingOrder);
    else
      TreeStats->sortByColumn(0, Qt::AscendingOrder);

    TreeStats->resizeColumnToContents(0);
  }

  return sd.m_pItem;
}

void wdQtMainWidget::SetFavorite(const wdString& sStat, bool bFavorite)
{
  StatData& sd = m_Stats[sStat];

  if (bFavorite)
  {
    m_Favorites.Insert(sStat);

    if (!sd.m_pItemFavorite)
    {
      sd.m_pItemFavorite = new QTreeWidgetItem();
      TreeFavorites->addTopLevelItem(sd.m_pItemFavorite);
      sd.m_pItemFavorite->setData(0, Qt::DisplayRole, sStat.GetData());
      sd.m_pItemFavorite->setData(1, Qt::DisplayRole, sd.m_Value.ConvertTo<wdString>().GetData());
      sd.m_pItemFavorite->setIcon(0, QIcon(":/Icons/Icons/StatFavorite.png"));

      TreeFavorites->resizeColumnToContents(0);
    }
  }
  else
  {
    if (sd.m_pItemFavorite)
    {
      m_Favorites.Remove(sStat);

      delete sd.m_pItemFavorite;
      sd.m_pItemFavorite = nullptr;
    }
  }
}

void wdQtMainWidget::on_TreeStats_itemChanged(QTreeWidgetItem* item, int column)
{
  if (column == 0)
  {
    wdString sPath = item->data(0, Qt::UserRole).toString().toUtf8().data();

    SetFavorite(sPath, (item->checkState(0) == Qt::Checked));
  }
}

void wdQtMainWidget::on_TreeStats_customContextMenuRequested(const QPoint& p)
{
  if (!TreeStats->currentItem())
    return;

  QMenu mSub;
  mSub.setTitle("Show in");
  mSub.setIcon(QIcon(":/Icons/Icons/StatHistory.png"));

  QMenu m;
  m.addMenu(&mSub);

  for (wdInt32 i = 0; i < 10; ++i)
  {
    wdQtMainWindow::s_pWidget->m_pActionShowStatIn[i]->setText(wdQtMainWindow::s_pWidget->m_pStatHistoryWidgets[i]->LineName->text());
    mSub.addAction(wdQtMainWindow::s_pWidget->m_pActionShowStatIn[i]);
  }

  if (TreeStats->currentItem()->childCount() > 0)
    mSub.setEnabled(false);

  m.exec(TreeStats->viewport()->mapToGlobal(p));
}


void wdQtMainWidget::ShowStatIn(bool)
{
  if (!TreeStats->currentItem())
    return;

  QAction* pAction = (QAction*)sender();

  wdInt32 iHistoryWidget = 0;
  for (iHistoryWidget = 0; iHistoryWidget < 10; ++iHistoryWidget)
  {
    if (wdQtMainWindow::s_pWidget->m_pActionShowStatIn[iHistoryWidget] == pAction)
      goto found;
  }

  return;

found:

  wdString sPath = TreeStats->currentItem()->data(0, Qt::UserRole).toString().toUtf8().data();

  wdQtMainWindow::s_pWidget->m_pStatHistoryWidgets[iHistoryWidget]->AddStat(sPath);
}
