#pragma once

#ifdef WD_USE_QT

#  include <QAbstractItemModel>
#  include <QColor>
#  include <QIcon>
#  include <TestFramework/Framework/Qt/qtTestFramework.h>
#  include <TestFramework/TestFrameworkDLL.h>

class wdQtTestFramework;

/// \brief Helper class that stores the test hierarchy used in wdQtTestModel.
class wdQtTestModelEntry
{
public:
  wdQtTestModelEntry(const wdTestFrameworkResult* pResult, wdInt32 iTestIndex = -1, wdInt32 iSubTestIndex = -1);
  ~wdQtTestModelEntry();

private:
  wdQtTestModelEntry(wdQtTestModelEntry&);
  void operator=(wdQtTestModelEntry&);

public:
  enum wdTestModelEntryType
  {
    RootNode,
    TestNode,
    SubTestNode
  };

  void ClearEntries();
  wdUInt32 GetNumSubEntries() const;
  wdQtTestModelEntry* GetSubEntry(wdUInt32 uiIndex) const;
  void AddSubEntry(wdQtTestModelEntry* pEntry);
  wdQtTestModelEntry* GetParentEntry() const { return m_pParentEntry; }
  wdUInt32 GetIndexInParent() const { return m_uiIndexInParent; }
  wdTestModelEntryType GetNodeType() const;
  const wdTestResultData* GetTestResult() const;
  wdInt32 GetTestIndex() const { return m_iTestIndex; }
  wdInt32 GetSubTestIndex() const { return m_iSubTestIndex; }

private:
  const wdTestFrameworkResult* m_pResult;
  wdInt32 m_iTestIndex;
  wdInt32 m_iSubTestIndex;

  wdQtTestModelEntry* m_pParentEntry;
  wdUInt32 m_uiIndexInParent;
  std::deque<wdQtTestModelEntry*> m_SubEntries;
};

/// \brief A Model that lists all unit tests and sub-tests in a tree.
class WD_TEST_DLL wdQtTestModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  wdQtTestModel(QObject* pParent, wdQtTestFramework* pTestFramework);
  virtual ~wdQtTestModel();

  void Reset();
  void InvalidateAll();
  void TestDataChanged(wdInt32 iTestIndex, wdInt32 iSubTestIndex);

  struct UserRoles
  {
    enum Enum
    {
      Duration = Qt::UserRole,
      DurationColor = Qt::UserRole + 1,
    };
  };

  struct Columns
  {
    enum Enum
    {
      Name = 0,
      Status,
      Duration,
      Errors,
      Asserts,
      Progress,
      ColumnCount,
    };
  };

public: // QAbstractItemModel interface
  virtual QVariant data(const QModelIndex& index, int iRole) const override;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
  virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;
  virtual QModelIndex index(int iRow, int iColumn, const QModelIndex& parent = QModelIndex()) const override;
  virtual QModelIndex parent(const QModelIndex& index) const override;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual bool setData(const QModelIndex& index, const QVariant& value, int iRole = Qt::EditRole) override;

public Q_SLOTS:
  void UpdateModel();

private:
  wdQtTestFramework* m_pTestFramework;
  wdTestFrameworkResult* m_pResult;
  wdQtTestModelEntry m_Root;
  QColor m_SucessColor;
  QColor m_FailedColor;
  QColor m_TestColor;
  QColor m_SubTestColor;
  QIcon m_TestIcon;
  QIcon m_TestIconOff;
};

#endif

