#include <TestFramework/TestFrameworkPCH.h>

#ifdef WD_USE_QT

#  include <QStringBuilder>
#  include <TestFramework/Framework/Qt/qtLogMessageDock.h>
#  include <TestFramework/Framework/TestFramework.h>

////////////////////////////////////////////////////////////////////////
// wdQtLogMessageDock public functions
////////////////////////////////////////////////////////////////////////

wdQtLogMessageDock::wdQtLogMessageDock(QObject* pParent, const wdTestFrameworkResult* pResult)
{
  setupUi(this);
  m_pModel = new wdQtLogMessageModel(this, pResult);
  ListView->setModel(m_pModel);
}

wdQtLogMessageDock::~wdQtLogMessageDock()
{
  ListView->setModel(nullptr);
  delete m_pModel;
  m_pModel = nullptr;
}

void wdQtLogMessageDock::resetModel()
{
  m_pModel->resetModel();
}

void wdQtLogMessageDock::currentTestResultChanged(const wdTestResultData* pTestResult)
{
  m_pModel->currentTestResultChanged(pTestResult);
  ListView->scrollToBottom();
}

void wdQtLogMessageDock::currentTestSelectionChanged(const wdTestResultData* pTestResult)
{
  m_pModel->currentTestSelectionChanged(pTestResult);
  ListView->scrollTo(m_pModel->GetLastIndexOfTestSelection(), QAbstractItemView::EnsureVisible);
  ListView->scrollTo(m_pModel->GetFirstIndexOfTestSelection(), QAbstractItemView::EnsureVisible);
}

////////////////////////////////////////////////////////////////////////
// wdQtLogMessageModel public functions
////////////////////////////////////////////////////////////////////////

wdQtLogMessageModel::wdQtLogMessageModel(QObject* pParent, const wdTestFrameworkResult* pResult)
  : QAbstractItemModel(pParent)
  , m_pTestResult(pResult)
{
}

wdQtLogMessageModel::~wdQtLogMessageModel() {}

void wdQtLogMessageModel::resetModel()
{
  beginResetModel();
  currentTestResultChanged(nullptr);
  endResetModel();
}

QModelIndex wdQtLogMessageModel::GetFirstIndexOfTestSelection()
{
  if (m_pCurrentTestSelection == nullptr || m_pCurrentTestSelection->m_iFirstOutput == -1)
    return QModelIndex();

  wdInt32 iEntries = (wdInt32)m_VisibleEntries.size();
  for (int i = 0; i < iEntries; ++i)
  {
    if ((wdInt32)m_VisibleEntries[i] >= m_pCurrentTestSelection->m_iFirstOutput)
      return index(i, 0);
  }
  return index(rowCount() - 1, 0);
}

QModelIndex wdQtLogMessageModel::GetLastIndexOfTestSelection()
{
  if (m_pCurrentTestSelection == nullptr || m_pCurrentTestSelection->m_iLastOutput == -1)
    return QModelIndex();

  wdInt32 iEntries = (wdInt32)m_VisibleEntries.size();
  for (int i = 0; i < iEntries; ++i)
  {
    if ((wdInt32)m_VisibleEntries[i] >= m_pCurrentTestSelection->m_iLastOutput)
      return index(i, 0);
  }
  return index(rowCount() - 1, 0);
}

void wdQtLogMessageModel::currentTestResultChanged(const wdTestResultData* pTestResult)
{
  UpdateVisibleEntries();
  currentTestSelectionChanged(pTestResult);
}

void wdQtLogMessageModel::currentTestSelectionChanged(const wdTestResultData* pTestResult)
{
  m_pCurrentTestSelection = pTestResult;
  if (m_pCurrentTestSelection != nullptr)
  {
    dataChanged(index(m_pCurrentTestSelection->m_iFirstOutput, 0), index(m_pCurrentTestSelection->m_iLastOutput, 0));
  }
}


////////////////////////////////////////////////////////////////////////
// wdQtLogMessageModel QAbstractItemModel functions
////////////////////////////////////////////////////////////////////////

QVariant wdQtLogMessageModel::data(const QModelIndex& index, int iRole) const
{
  if (!index.isValid() || m_pTestResult == nullptr || index.column() != 0)
    return QVariant();

  const wdInt32 iRow = index.row();
  if (iRow < 0 || iRow >= (wdInt32)m_VisibleEntries.size())
    return QVariant();

  const wdUInt32 uiLogIdx = m_VisibleEntries[iRow];
  const wdUInt8 uiIndention = m_VisibleEntriesIndention[iRow];
  const wdTestOutputMessage& Message = *m_pTestResult->GetOutputMessage(uiLogIdx);
  const wdTestErrorMessage* pError = (Message.m_iErrorIndex != -1) ? m_pTestResult->GetErrorMessage(Message.m_iErrorIndex) : nullptr;
  switch (iRole)
  {
    case Qt::DisplayRole:
    {
      if (pError != nullptr)
      {
        QString sBlockStart = QLatin1String("\n") % QString((uiIndention + 1) * 3, ' ');
        QString sBlockName =
          pError->m_sBlock.empty() ? QLatin1String("") : (sBlockStart % QLatin1String("Block: ") + QLatin1String(pError->m_sBlock.c_str()));
        QString sMessage =
          pError->m_sMessage.empty() ? QLatin1String("") : (sBlockStart % QLatin1String("Message: ") + QLatin1String(pError->m_sMessage.c_str()));
        QString sErrorMessage = QString(uiIndention * 3, ' ') % QString(Message.m_sMessage.c_str()) % sBlockName % sBlockStart %
                                QLatin1String("File: ") % QLatin1String(pError->m_sFile.c_str()) % sBlockStart % QLatin1String("Line: ") %
                                QString::number(pError->m_iLine) % sBlockStart % QLatin1String("Function: ") %
                                QLatin1String(pError->m_sFunction.c_str()) % sMessage;

        return sErrorMessage;
      }
      return QString(uiIndention * 3, ' ') + QString(Message.m_sMessage.c_str());
    }
    case Qt::ForegroundRole:
    {
      switch (Message.m_Type)
      {
        case wdTestOutput::BeginBlock:
        case wdTestOutput::Message:
          return QColor(Qt::yellow);
        case wdTestOutput::Error:
          return QColor(Qt::red);
        case wdTestOutput::Success:
          return QColor(Qt::green);
        case wdTestOutput::Warning:
          return QColor(qRgb(255, 100, 0));
        case wdTestOutput::StartOutput:
        case wdTestOutput::EndBlock:
        case wdTestOutput::ImportantInfo:
        case wdTestOutput::Details:
        case wdTestOutput::Duration:
        case wdTestOutput::FinalResult:
          return QVariant();
        default:
          return QVariant();
      }
    }
    case Qt::BackgroundRole:
    {
      QPalette palette = QApplication::palette();
      if (m_pCurrentTestSelection != nullptr && m_pCurrentTestSelection->m_iFirstOutput != -1)
      {
        if (m_pCurrentTestSelection->m_iFirstOutput <= (wdInt32)uiLogIdx && (wdInt32)uiLogIdx <= m_pCurrentTestSelection->m_iLastOutput)
        {
          return palette.midlight().color();
        }
      }
      return palette.base().color();
    }

    default:
      return QVariant();
  }
}

Qt::ItemFlags wdQtLogMessageModel::flags(const QModelIndex& index) const
{
  if (!index.isValid() || m_pTestResult == nullptr)
    return Qt::ItemFlags();

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant wdQtLogMessageModel::headerData(int iSection, Qt::Orientation orientation, int iRole) const
{
  if (orientation == Qt::Horizontal && iRole == Qt::DisplayRole)
  {
    switch (iSection)
    {
      case 0:
        return QString("Log Entry");
    }
  }
  return QVariant();
}

QModelIndex wdQtLogMessageModel::index(int iRow, int iColumn, const QModelIndex& parent) const
{
  if (parent.isValid() || m_pTestResult == nullptr || iColumn != 0)
    return QModelIndex();

  return createIndex(iRow, iColumn, iRow);
}

QModelIndex wdQtLogMessageModel::parent(const QModelIndex& index) const
{
  return QModelIndex();
}

int wdQtLogMessageModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid() || m_pTestResult == nullptr)
    return 0;

  return (int)m_VisibleEntries.size();
}

int wdQtLogMessageModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}


////////////////////////////////////////////////////////////////////////
// wdQtLogMessageModel private functions
////////////////////////////////////////////////////////////////////////

void wdQtLogMessageModel::UpdateVisibleEntries()
{
  m_VisibleEntries.clear();
  m_VisibleEntriesIndention.clear();
  if (m_pTestResult == nullptr)
    return;

  wdUInt8 uiIndention = 0;
  wdUInt32 uiEntries = m_pTestResult->GetOutputMessageCount();
  /// \todo filter out uninteresting messages
  for (wdUInt32 i = 0; i < uiEntries; ++i)
  {
    wdTestOutput::Enum Type = m_pTestResult->GetOutputMessage(i)->m_Type;
    if (Type == wdTestOutput::BeginBlock)
      uiIndention++;
    if (Type == wdTestOutput::EndBlock)
      uiIndention--;

    m_VisibleEntries.push_back(i);
    m_VisibleEntriesIndention.push_back(uiIndention);
  }
  beginResetModel();
  endResetModel();
}

#endif

WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_Qt_qtLogMessageDock);
