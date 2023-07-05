#include <Inspector/InspectorPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Foundation/IO/OSFile.h>
#include <GuiFoundation/GuiFoundationDLL.h>
#include <Inspector/FileWidget.moc.h>
#include <Inspector/MainWindow.moc.h>
#include <qgraphicsitem.h>

wdQtFileWidget* wdQtFileWidget::s_pWidget = nullptr;

wdQtFileWidget::wdQtFileWidget(QWidget* pParent)
  : ads::CDockWidget("File Operations", pParent)
{
  s_pWidget = this;

  setupUi(this);
  setWidget(Frame);

  ResetStats();
}


wdQtFileWidget::~wdQtFileWidget() {}

void wdQtFileWidget::ResetStats()
{
  m_iMaxID = 0;
  m_bUpdateTable = true;
  m_FileOps.Clear();
  m_FileOps.Reserve(10000);
  m_LastTableUpdate = wdTime::Seconds(0);

  Table->clear();

  {
    QStringList Headers;
    Headers.append(" # ");
    Headers.append(" Operation ");
    Headers.append(" Duration (ms)");
    Headers.append(" Bytes ");
    Headers.append(" Thread ");
    Headers.append(" File ");

    Table->setColumnCount(Headers.size());
    Table->setHorizontalHeaderLabels(Headers);
    Table->horizontalHeader()->show();
  }

  Table->resizeColumnsToContents();
  Table->sortByColumn(0, Qt::DescendingOrder);
}

void wdQtFileWidget::ProcessTelemetry(void* pUnuseed)
{
  if (!s_pWidget)
    return;

  wdTelemetryMessage Msg;

  while (wdTelemetry::RetrieveMessage('FILE', Msg) == WD_SUCCESS)
  {
    s_pWidget->m_bUpdateTable = true;

    wdInt32 iFileID = 0;
    Msg.GetReader() >> iFileID;

    s_pWidget->m_iMaxID = wdMath::Max(s_pWidget->m_iMaxID, iFileID);
    FileOpData& data = s_pWidget->m_FileOps[iFileID];

    if (data.m_StartTime.GetSeconds() == 0.0)
      data.m_StartTime = wdTime::Now();

    switch (Msg.GetMessageID())
    {
      case 'OPEN':
      {
        wdUInt8 uiMode = 0;
        bool bSuccess = false;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> uiMode;
        Msg.GetReader() >> bSuccess;

        switch (uiMode)
        {
          case wdFileOpenMode::Write:
          case wdFileOpenMode::Append:
            data.m_State = bSuccess ? OpenWriting : OpenWritingFailed;
            break;
          case wdFileOpenMode::Read:
            data.m_State = bSuccess ? OpenReading : OpenReadingFailed;
            break;
          default:
            WD_REPORT_FAILURE("Unknown File Open Mode {0}", uiMode);
            break;
        }
      }
      break;
      case 'CLOS':
      {
        switch (data.m_State)
        {
          case OpenReading:
            data.m_State = ClosedReading;
            break;
          case OpenWriting:
            data.m_State = ClosedWriting;
            break;
          default:

            // dangling 'read' or 'write', just ignore it
            s_pWidget->m_FileOps.Remove(iFileID);
            return;
        }
      }
      break;
      case 'WRIT':
      {
        wdUInt64 uiSize;
        bool bSuccess = false;

        Msg.GetReader() >> uiSize;
        Msg.GetReader() >> bSuccess;

        data.m_uiBytesAccessed += uiSize;

        if (data.m_State == None)
          data.m_State = OpenWriting;

        if (!bSuccess)
          data.m_State = OpenWritingFailed;
      }
      break;
      case 'READ':
      {
        wdUInt64 uiRead;
        Msg.GetReader() >> uiRead;

        data.m_uiBytesAccessed += uiRead;

        if (data.m_State == None)
          data.m_State = OpenReading;
      }
      break;

      case 'EXST':
      {
        bool bSuccess;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> bSuccess;

        data.m_State = bSuccess ? FileExists : FileExistsFailed;
      }
      break;

      case ' DEL':
      {
        bool bSuccess;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> bSuccess;

        data.m_State = bSuccess ? FileDelete : FileDeleteFailed;
      }
      break;

      case 'CDIR':
      {
        bool bSuccess;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> bSuccess;

        data.m_State = bSuccess ? CreateDirs : CreateDirsFailed;
      }
      break;

      case 'COPY':
      {
        bool bSuccess;
        wdString sFile1, sFile2;

        Msg.GetReader() >> sFile1;
        Msg.GetReader() >> sFile2;
        Msg.GetReader() >> bSuccess;

        wdStringBuilder s;
        s.Format("'{0}' -> '{1}'", sFile1, sFile2);
        data.m_sFile = s.GetData();

        data.m_State = bSuccess ? FileCopy : FileCopyFailed;
      }
      break;

      case 'STAT':
      {
        bool bSuccess;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> bSuccess;

        data.m_State = bSuccess ? FileStat : FileStatFailed;
      }
      break;

      case 'CASE':
      {
        bool bSuccess;

        Msg.GetReader() >> data.m_sFile;
        Msg.GetReader() >> bSuccess;

        data.m_State = bSuccess ? FileCasing : FileCasingFailed;
      }
      break;
    }

    double dTime = 0.0;
    Msg.GetReader() >> dTime;
    data.m_BlockedDuration += wdTime::Seconds(dTime);

    wdUInt8 uiThreadTypes = 0;
    Msg.GetReader() >> uiThreadTypes;
    data.m_uiThreadTypes |= uiThreadTypes;
  }
}

QTableWidgetItem* wdQtFileWidget::GetStateString(FileOpState State) const
{
  QTableWidgetItem* pItem = new QTableWidgetItem();
  pItem->setTextAlignment(Qt::AlignCenter);

  switch (State)
  {
    case None:
      pItem->setText("Unknown");
      pItem->setForeground(Qt::red);
      break;
    case ClosedReading:
      pItem->setText("Read");
      pItem->setForeground(QColor::fromRgb(110, 60, 185));
      break;
    case ClosedWriting:
      pItem->setText("Write");
      pItem->setForeground(QColor::fromRgb(255, 140, 0));
      break;
    case CreateDirs:
      pItem->setText("MakeDir");
      pItem->setForeground(Qt::darkYellow);
      break;
    case CreateDirsFailed:
      pItem->setText("MakeDir (fail)");
      pItem->setForeground(Qt::red);
      break;
    case FileCopy:
      pItem->setText("Copy");
      pItem->setForeground(QColor::fromRgb(255, 0, 255));
      break;
    case FileCopyFailed:
      pItem->setText("Copy (fail)");
      pItem->setForeground(Qt::red);
      break;
    case FileDelete:
      pItem->setText("Delete");
      pItem->setForeground(Qt::darkYellow);
      break;
    case FileDeleteFailed:
      pItem->setText("Delete (fail)");
      pItem->setForeground(Qt::red);
      break;
    case FileExists:
      pItem->setText("Exists");
      pItem->setForeground(Qt::lightGray);
      break;
    case FileExistsFailed:
      pItem->setText("Exists (not)");
      pItem->setForeground(Qt::red);
      break;
    case OpenReading:
      pItem->setText("Read (Open)");
      pItem->setForeground(QColor::fromRgb(160, 90, 255));
      break;
    case OpenReadingFailed:
      pItem->setText("Read (fail)");
      pItem->setForeground(Qt::red);
      break;
    case OpenWriting:
      pItem->setText("Write (Open)");
      pItem->setForeground(QColor::fromRgb(255, 64, 0));
      break;
    case OpenWritingFailed:
      pItem->setText("Write (fail)");
      pItem->setForeground(Qt::red);
      break;
    case FileStat:
      pItem->setText("Stat");
      pItem->setForeground(QColor::fromRgb(128, 128, 128));
      break;
    case FileStatFailed:
      pItem->setText("Stat (fail)");
      pItem->setForeground(Qt::red);
      break;
    case FileCasing:
      pItem->setText("Casing");
      pItem->setForeground(Qt::cyan);
      break;
    case FileCasingFailed:
      pItem->setText("Casing (fail)");
      pItem->setForeground(Qt::red);
      break;
    default:
      WD_REPORT_FAILURE("Unknown File Operation {0}", (wdInt32)State);
      break;
  }

  return pItem;
}

void wdQtFileWidget::UpdateTable()
{
  if (!m_bUpdateTable)
    return;

  if (wdTime::Now() - m_LastTableUpdate < wdTime::Seconds(0.3))
    return;

  m_LastTableUpdate = wdTime::Now();

  m_bUpdateTable = false;

  wdQtScopedUpdatesDisabled _1(Table);

  Table->setSortingEnabled(false);
  Table->clear();

  {
    QStringList Headers;
    Headers.append(" # ");
    Headers.append(" Operation ");
    Headers.append(" Duration (ms) ");
    Headers.append(" Bytes ");
    Headers.append(" Thread ");
    Headers.append(" File ");

    Table->setColumnCount(Headers.size());
    Table->setHorizontalHeaderLabels(Headers);
    Table->horizontalHeader()->show();
  }

  const double fMinDuration = SpinMinDuration->value();
  const wdUInt32 uiMMaxElements = SpinLimitToRecent->value();
  wdString sFilter = LineFilterByName->text().toUtf8().data();

  const wdUInt32 iThread = ComboThread->currentIndex();
  const wdUInt8 uiThreadFilter = (iThread == 0) ? 0xFF : (1 << (iThread - 1));

  wdUInt32 uiRow = 0;
  for (wdHashTable<wdUInt32, FileOpData>::Iterator it = m_FileOps.GetIterator(); it.IsValid(); ++it)
  {
    if ((uiThreadFilter & it.Value().m_uiThreadTypes) == 0)
      continue;

    if (it.Value().m_BlockedDuration.GetSeconds() < fMinDuration)
      continue;

    if ((uiMMaxElements > 0) && (m_iMaxID - it.Key() > uiMMaxElements))
      continue;

    if (!sFilter.IsEmpty() && (it.Value().m_sFile.FindSubString_NoCase(sFilter.GetData()) == nullptr))
      continue;

    if (uiRow >= (wdUInt32)Table->rowCount())
      Table->insertRow(Table->rowCount());

    QTableWidgetItem* pItem;

    pItem = new QTableWidgetItem();
    pItem->setData(Qt::DisplayRole, QVariant((wdUInt64)it.Value().m_StartTime.GetMicroseconds()));
    Table->setItem(uiRow, 0, pItem);

    pItem = GetStateString(it.Value().m_State);
    Table->setItem(uiRow, 1, pItem);

    pItem = new QTableWidgetItem();
    pItem->setData(Qt::DisplayRole, QVariant(it.Value().m_BlockedDuration.GetSeconds() * 1000.0));
    Table->setItem(uiRow, 2, pItem);

    pItem = new QTableWidgetItem();
    pItem->setData(Qt::DisplayRole, QVariant(it.Value().m_uiBytesAccessed));
    Table->setItem(uiRow, 3, pItem);

    pItem = new QTableWidgetItem();
    pItem->setTextAlignment(Qt::AlignCenter);

    wdStringBuilder sThread;

    if ((it.Value().m_uiThreadTypes & (1 << 0)) != 0) // Main Thread
      pItem->setForeground(QColor::fromRgb(255, 64, 0));
    else if ((it.Value().m_uiThreadTypes & (1 << 2)) != 0) // Other Thread
      pItem->setForeground(QColor::fromRgb(160, 90, 255));
    else // Task Loading Thread
      pItem->setForeground(QColor::fromRgb(0, 255, 0));

    if ((it.Value().m_uiThreadTypes & (1 << 0)) != 0) // Main Thread
      sThread.Append(" Main ");
    if ((it.Value().m_uiThreadTypes & (1 << 1)) != 0) // Loading Thread
      sThread.Append(" Loading ");
    if ((it.Value().m_uiThreadTypes & (1 << 2)) != 0) // Other Thread
      sThread.Append(" Other ");

    pItem->setData(Qt::DisplayRole, QVariant(sThread.GetData()));
    Table->setItem(uiRow, 4, pItem);

    pItem = new QTableWidgetItem();
    pItem->setData(Qt::DisplayRole, QVariant(it.Value().m_sFile.GetData()));
    Table->setItem(uiRow, 5, pItem);

    ++uiRow;
  }

  Table->setRowCount(uiRow);
  Table->setSortingEnabled(true);
}

void wdQtFileWidget::UpdateStats()
{
  if (!m_bUpdateTable)
    return;

  UpdateTable();
}

void wdQtFileWidget::on_SpinLimitToRecent_valueChanged(int val)
{
  m_bUpdateTable = true;
}

void wdQtFileWidget::on_SpinMinDuration_valueChanged(double val)
{
  m_bUpdateTable = true;
}

void wdQtFileWidget::on_LineFilterByName_textChanged()
{
  m_bUpdateTable = true;
}

void wdQtFileWidget::on_ComboThread_currentIndexChanged(int state)
{
  m_bUpdateTable = true;
}