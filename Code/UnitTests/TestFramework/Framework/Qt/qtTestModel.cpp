#include <TestFramework/TestFrameworkPCH.h>

#ifdef WD_USE_QT

#  include <QApplication>
#  include <QPalette>
#  include <TestFramework/Framework/Qt/qtTestModel.h>

////////////////////////////////////////////////////////////////////////
// wdQtTestModelEntry public functions
////////////////////////////////////////////////////////////////////////

wdQtTestModelEntry::wdQtTestModelEntry(const wdTestFrameworkResult* pResult, wdInt32 iTestIndex, wdInt32 iSubTestIndex)
  : m_pResult(pResult)
  , m_iTestIndex(iTestIndex)
  , m_iSubTestIndex(iSubTestIndex)
  , m_pParentEntry(nullptr)
  , m_uiIndexInParent(0)
{
}

wdQtTestModelEntry::~wdQtTestModelEntry()
{
  ClearEntries();
}

void wdQtTestModelEntry::ClearEntries()
{
  for (wdInt32 i = (wdInt32)m_SubEntries.size() - 1; i >= 0; --i)
  {
    delete m_SubEntries[i];
  }
  m_SubEntries.clear();
}
wdUInt32 wdQtTestModelEntry::GetNumSubEntries() const

{
  return (wdInt32)m_SubEntries.size();
}

wdQtTestModelEntry* wdQtTestModelEntry::GetSubEntry(wdUInt32 uiIndex) const
{
  if (uiIndex >= GetNumSubEntries())
    return nullptr;

  return m_SubEntries[uiIndex];
}

void wdQtTestModelEntry::AddSubEntry(wdQtTestModelEntry* pEntry)
{
  pEntry->m_pParentEntry = this;
  pEntry->m_uiIndexInParent = (wdUInt32)m_SubEntries.size();
  m_SubEntries.push_back(pEntry);
}

wdQtTestModelEntry::wdTestModelEntryType wdQtTestModelEntry::GetNodeType() const
{
  return (m_iTestIndex == -1) ? RootNode : ((m_iSubTestIndex == -1) ? TestNode : SubTestNode);
}

const wdTestResultData* wdQtTestModelEntry::GetTestResult() const
{
  switch (GetNodeType())
  {
    case wdQtTestModelEntry::TestNode:
    case wdQtTestModelEntry::SubTestNode:
      return &m_pResult->GetTestResultData(m_iTestIndex, m_iSubTestIndex);
    default:
      return nullptr;
  }
}

static QColor ToneColor(const QColor& inputColor, const QColor& toneColor)
{
  qreal fHue = toneColor.hueF();
  qreal fSaturation = 1.0f;
  qreal fLightness = inputColor.lightnessF();
  fLightness = wdMath::Clamp(fLightness, 0.20, 0.80);
  return QColor::fromHslF(fHue, fSaturation, fLightness);
}

////////////////////////////////////////////////////////////////////////
// wdQtTestModel public functions
////////////////////////////////////////////////////////////////////////

wdQtTestModel::wdQtTestModel(QObject* pParent, wdQtTestFramework* pTestFramework)
  : QAbstractItemModel(pParent)
  , m_pTestFramework(pTestFramework)
  , m_Root(nullptr)
{
  QPalette palette = QApplication::palette();
  m_pResult = &pTestFramework->GetTestResult();

  // Derive state colors from the current active palette.
  m_SucessColor = ToneColor(palette.text().color(), QColor(Qt::green)).toRgb();
  m_FailedColor = ToneColor(palette.text().color(), QColor(Qt::red)).toRgb();

  m_TestColor = ToneColor(palette.base().color(), QColor(Qt::cyan)).toRgb();
  m_SubTestColor = ToneColor(palette.base().color(), QColor(Qt::blue)).toRgb();

  m_TestIcon = QIcon(":/Icons/Icons/pie.png");
  m_TestIconOff = QIcon(":/Icons/Icons/pie_off.png");

  UpdateModel();
}

wdQtTestModel::~wdQtTestModel()
{
  m_Root.ClearEntries();
}

void wdQtTestModel::Reset()
{
  beginResetModel();
  endResetModel();
}

void wdQtTestModel::InvalidateAll()
{
  dataChanged(QModelIndex(), QModelIndex());
}

void wdQtTestModel::TestDataChanged(wdInt32 iTestIndex, wdInt32 iSubTestIndex)
{
  QModelIndex TestModelIndex = index(iTestIndex, 0);
  // Invalidate whole test row
  Q_EMIT dataChanged(TestModelIndex, index(iTestIndex, columnCount() - 1));

  // Invalidate all sub-tests
  const wdQtTestModelEntry* pEntry = (wdQtTestModelEntry*)TestModelIndex.internalPointer();
  wdInt32 iChildren = (wdInt32)pEntry->GetNumSubEntries();
  Q_EMIT dataChanged(index(0, 0, TestModelIndex), index(iChildren - 1, columnCount() - 1, TestModelIndex));
}


////////////////////////////////////////////////////////////////////////
// wdQtTestModel QAbstractItemModel functions
////////////////////////////////////////////////////////////////////////

QVariant wdQtTestModel::data(const QModelIndex& index, int iRole) const
{
  if (!index.isValid())
    return QVariant();

  const wdQtTestModelEntry* pEntry = (wdQtTestModelEntry*)index.internalPointer();
  const wdQtTestModelEntry* pParentEntry = pEntry->GetParentEntry();
  const wdQtTestModelEntry::wdTestModelEntryType entryType = pEntry->GetNodeType();

  bool bTestEnabled = true;
  bool bParentEnabled = true;
  bool bIsSubTest = entryType == wdQtTestModelEntry::SubTestNode;
  const std::string& testUnavailableReason = m_pTestFramework->IsTestAvailable(bIsSubTest ? pParentEntry->GetTestIndex() : pEntry->GetTestIndex());

  if (bIsSubTest)
  {
    bTestEnabled = m_pTestFramework->IsSubTestEnabled(pEntry->GetTestIndex(), pEntry->GetSubTestIndex());
    bParentEnabled = m_pTestFramework->IsTestEnabled(pParentEntry->GetTestIndex());
  }
  else
  {
    bTestEnabled = m_pTestFramework->IsTestEnabled(pEntry->GetTestIndex());
  }

  const wdTestResultData& TestResult = *pEntry->GetTestResult();

  // Name
  if (index.column() == Columns::Name)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        return QString(TestResult.m_sName.c_str());
      }
      case Qt::CheckStateRole:
      {
        return bTestEnabled ? Qt::Checked : Qt::Unchecked;
      }
      case Qt::DecorationRole:
      {
        return (bTestEnabled && bParentEnabled) ? m_TestIcon : m_TestIconOff;
      }
      case Qt::ForegroundRole:
      {
        if (!testUnavailableReason.empty())
        {
          QPalette palette = QApplication::palette();
          return palette.color(QPalette::Disabled, QPalette::Text);
        }
      }
      case Qt::ToolTipRole:
      {
        if (!testUnavailableReason.empty())
        {
          return QString("Test not available: %1").arg(testUnavailableReason.c_str());
        }
      }
      default:
        return QVariant();
    }
  }
  // Status
  else if (index.column() == Columns::Status)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        if (!testUnavailableReason.empty())
        {
          return QString("Test not available: %1").arg(testUnavailableReason.c_str());
        }
        else if (bTestEnabled && bParentEnabled)
        {
          if (bIsSubTest)
          {
            return QString("Enabled");
          }
          else
          {
            // Count sub-test status
            const wdUInt32 iSubTests = m_pResult->GetSubTestCount(pEntry->GetTestIndex());
            const wdUInt32 iEnabled = m_pTestFramework->GetSubTestEnabledCount(pEntry->GetTestIndex());

            if (iEnabled == iSubTests)
            {
              return QString("All Enabled");
            }

            return QString("%1 / %2 Enabled").arg(iEnabled).arg(iSubTests);
          }
        }
        else
        {
          return QString("Disabled");
        }
      }
      case Qt::TextAlignmentRole:
      {
        return Qt::AlignRight;
      }
      default:
        return QVariant();
    }
  }
  // Duration
  else if (index.column() == Columns::Duration)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        return QLocale(QLocale::English).toString(TestResult.m_fTestDuration, 'f', 4);
      }
      case Qt::TextAlignmentRole:
      {
        return Qt::AlignRight;
      }
      /*case Qt::BackgroundRole:
        {
          QPalette palette = QApplication::palette();
          return palette.alternateBase().color();
        }*/
      case UserRoles::Duration:
      {
        if (bIsSubTest && TestResult.m_bExecuted)
        {
          return TestResult.m_fTestDuration / pParentEntry->GetTestResult()->m_fTestDuration;
        }
        else if (TestResult.m_bExecuted)
        {
          return TestResult.m_fTestDuration / m_pTestFramework->GetTotalTestDuration();
        }
        return QVariant();
      }
      case UserRoles::DurationColor:
      {
        if (TestResult.m_bExecuted)
        {
          return (bIsSubTest ? m_SubTestColor : m_TestColor);
        }
        return QVariant();
      }
      default:
        return QVariant();
    }
  }
  // Errors
  else if (index.column() == Columns::Errors)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        return QString("%1 / %2")
          .arg(m_pResult->GetErrorMessageCount(pEntry->GetTestIndex(), pEntry->GetSubTestIndex()))
          .arg(m_pResult->GetOutputMessageCount(pEntry->GetTestIndex(), pEntry->GetSubTestIndex()));
      }
      case Qt::BackgroundRole:
      {
        QPalette palette = QApplication::palette();
        return palette.alternateBase().color();
      }
      case Qt::ForegroundRole:
      {
        if (TestResult.m_bExecuted)
        {
          return (m_pResult->GetErrorMessageCount(pEntry->GetTestIndex(), pEntry->GetSubTestIndex()) == 0) ? m_SucessColor : m_FailedColor;
        }
        return QVariant();
      }
      case Qt::TextAlignmentRole:
      {
        return Qt::AlignRight;
      }

      default:
        return QVariant();
    }
  }
  // Assert Count
  else if (index.column() == Columns::Asserts)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        return QString("%1").arg(TestResult.m_iTestAsserts);
      }
      case Qt::BackgroundRole:
      {
        QPalette palette = QApplication::palette();
        return palette.alternateBase().color();
      }
      case Qt::TextAlignmentRole:
      {
        return Qt::AlignRight;
      }

      default:
        return QVariant();
    }
  }
  // Progress
  else if (index.column() == Columns::Progress)
  {
    switch (iRole)
    {
      case Qt::DisplayRole:
      {
        if (!testUnavailableReason.empty())
        {
          return QString("Test not available: %1").arg(testUnavailableReason.c_str());
        }
        else if (bTestEnabled && bParentEnabled)
        {
          if (bIsSubTest)
          {
            if (TestResult.m_bExecuted)
            {
              return (TestResult.m_bSuccess) ? QString("Passed") : QString("Failed");
            }
            else
            {
              return QString("Pending");
            }
          }
          else
          {
            // Count sub-test status

            const wdUInt32 iEnabled = m_pTestFramework->GetSubTestEnabledCount(pEntry->GetTestIndex());
            const wdUInt32 iExecuted = m_pResult->GetSubTestCount(pEntry->GetTestIndex(), wdTestResultQuery::Executed);
            const wdUInt32 iSucceeded = m_pResult->GetSubTestCount(pEntry->GetTestIndex(), wdTestResultQuery::Success);

            if (TestResult.m_bExecuted && iExecuted == iEnabled)
            {
              return (TestResult.m_bSuccess && iExecuted == iSucceeded) ? QString("Passed") : QString("Failed");
            }
            else
            {
              return QString("%1 / %2 Executed").arg(iExecuted).arg(iEnabled);
            }
          }
        }
        else
        {
          return QString("Disabled");
        }
      }
      case Qt::BackgroundRole:
      {
        QPalette palette = QApplication::palette();
        return palette.alternateBase().color();
      }
      case Qt::ForegroundRole:
      {
        if (!testUnavailableReason.empty())
        {
          QPalette palette = QApplication::palette();
          return palette.color(QPalette::Disabled, QPalette::Text);
        }
        else if (TestResult.m_bExecuted)
        {
          return TestResult.m_bSuccess ? m_SucessColor : m_FailedColor;
        }
        return QVariant();
      }
      case Qt::TextAlignmentRole:
      {
        return Qt::AlignRight;
      }

      default:
        return QVariant();
    }
  }

  return QVariant();
}

Qt::ItemFlags wdQtTestModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::ItemFlags();

  wdQtTestModelEntry* pEntry = (wdQtTestModelEntry*)index.internalPointer();
  if (pEntry == &m_Root)
    return Qt::ItemFlags();

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QVariant wdQtTestModel::headerData(int iSection, Qt::Orientation orientation, int iRole) const
{
  if (orientation == Qt::Horizontal && iRole == Qt::DisplayRole)
  {
    switch (iSection)
    {
      case Columns::Name:
        return QString("Name");
      case Columns::Status:
        return QString("Status");
      case Columns::Duration:
        return QString("Duration (ms)");
      case Columns::Errors:
        return QString("Errors / Output");
      case Columns::Asserts:
        return QString("Checks");
      case Columns::Progress:
        return QString("Progress");
    }
  }
  return QVariant();
}

QModelIndex wdQtTestModel::index(int iRow, int iColumn, const QModelIndex& parent) const
{
  if (!hasIndex(iRow, iColumn, parent))
    return QModelIndex();

  const wdQtTestModelEntry* pParent = nullptr;

  if (!parent.isValid())
    pParent = &m_Root;
  else
    pParent = static_cast<wdQtTestModelEntry*>(parent.internalPointer());

  wdQtTestModelEntry* pEntry = pParent->GetSubEntry(iRow);
  return pEntry ? createIndex(iRow, iColumn, pEntry) : QModelIndex();
}

QModelIndex wdQtTestModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return QModelIndex();

  wdQtTestModelEntry* pChild = static_cast<wdQtTestModelEntry*>(index.internalPointer());
  wdQtTestModelEntry* pParent = pChild->GetParentEntry();

  if (pParent == &m_Root)
    return QModelIndex();

  return createIndex(pParent->GetIndexInParent(), 0, pParent);
}

int wdQtTestModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() > 0)
    return 0;

  const wdQtTestModelEntry* pParent = nullptr;

  if (!parent.isValid())
    pParent = &m_Root;
  else
    pParent = static_cast<wdQtTestModelEntry*>(parent.internalPointer());

  return pParent->GetNumSubEntries();
}

int wdQtTestModel::columnCount(const QModelIndex& parent) const
{
  return Columns::ColumnCount;
}

bool wdQtTestModel::setData(const QModelIndex& index, const QVariant& value, int iRole)
{
  wdQtTestModelEntry* pEntry = static_cast<wdQtTestModelEntry*>(index.internalPointer());
  if (pEntry == nullptr || index.column() != Columns::Name || iRole != Qt::CheckStateRole)
    return false;

  if (pEntry->GetNodeType() == wdQtTestModelEntry::TestNode)
  {
    m_pTestFramework->SetTestEnabled(pEntry->GetTestIndex(), value.toBool());
    TestDataChanged(pEntry->GetIndexInParent(), -1);

    // if a test gets enabled in the UI, and all sub-tests are currently disabled,
    // enable all sub-tests as well
    // if some set of sub-tests is already enabled and some are disabled,
    // do not mess with the user's choice of enabled tests
    bool bEnableSubTests = value.toBool();
    for (wdUInt32 subIdx = 0; subIdx < pEntry->GetNumSubEntries(); ++subIdx)
    {
      if (m_pTestFramework->IsSubTestEnabled(pEntry->GetTestIndex(), subIdx))
      {
        bEnableSubTests = false;
        break;
      }
    }

    if (bEnableSubTests)
    {
      for (wdUInt32 subIdx = 0; subIdx < pEntry->GetNumSubEntries(); ++subIdx)
      {
        m_pTestFramework->SetSubTestEnabled(pEntry->GetTestIndex(), subIdx, true);
        TestDataChanged(pEntry->GetIndexInParent(), subIdx);
      }
    }
  }
  else
  {
    m_pTestFramework->SetSubTestEnabled(pEntry->GetTestIndex(), pEntry->GetSubTestIndex(), value.toBool());
    TestDataChanged(pEntry->GetParentEntry()->GetIndexInParent(), pEntry->GetIndexInParent());
  }

  return true;
}


////////////////////////////////////////////////////////////////////////
// wdQtTestModel public slots
////////////////////////////////////////////////////////////////////////

void wdQtTestModel::UpdateModel()
{
  m_Root.ClearEntries();
  if (m_pResult == nullptr)
    return;

  const wdUInt32 uiTestCount = m_pResult->GetTestCount();
  for (wdUInt32 uiTestIndex = 0; uiTestIndex < uiTestCount; ++uiTestIndex)
  {
    wdQtTestModelEntry* pTestModelEntry = new wdQtTestModelEntry(m_pResult, uiTestIndex);
    m_Root.AddSubEntry(pTestModelEntry);

    const wdUInt32 uiSubTestCount = m_pResult->GetSubTestCount(uiTestIndex);
    for (wdUInt32 uiSubTestIndex = 0; uiSubTestIndex < uiSubTestCount; ++uiSubTestIndex)
    {
      wdQtTestModelEntry* pSubTestModelEntry = new wdQtTestModelEntry(m_pResult, uiTestIndex, uiSubTestIndex);
      pTestModelEntry->AddSubEntry(pSubTestModelEntry);
    }
  }
  // reset();
}


#endif

WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_Qt_qtTestModel);
