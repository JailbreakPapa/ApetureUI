#pragma once

#ifdef WD_USE_QT

#  include <QAbstractItemModel>
#  include <QDockWidget>
#  include <TestFramework/TestFrameworkDLL.h>
#  include <TestFramework/ui_qtLogMessageDock.h>
#  include <vector>

class wdQtTestFramework;
struct wdTestResultData;
class wdQtLogMessageModel;
class wdTestFrameworkResult;

/// \brief Dock widget that lists the output of a given wdResult struct.
class WD_TEST_DLL wdQtLogMessageDock : public QDockWidget, public Ui_qtLogMessageDock
{
  Q_OBJECT
public:
  wdQtLogMessageDock(QObject* pParent, const wdTestFrameworkResult* pResult);
  virtual ~wdQtLogMessageDock();

public Q_SLOTS:
  void resetModel();
  void currentTestResultChanged(const wdTestResultData* pTestResult);
  void currentTestSelectionChanged(const wdTestResultData* pTestResult);

private:
  wdQtLogMessageModel* m_pModel;
};

/// \brief Model used by wdQtLogMessageDock to list the output entries in wdResult.
class WD_TEST_DLL wdQtLogMessageModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  wdQtLogMessageModel(QObject* pParent, const wdTestFrameworkResult* pResult);
  virtual ~wdQtLogMessageModel();

  void resetModel();
  QModelIndex GetFirstIndexOfTestSelection();
  QModelIndex GetLastIndexOfTestSelection();

public Q_SLOTS:
  void currentTestResultChanged(const wdTestResultData* pTestResult);
  void currentTestSelectionChanged(const wdTestResultData* pTestResult);

public: // QAbstractItemModel interface
  virtual QVariant data(const QModelIndex& index, int iRole) const override;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
  virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;
  virtual QModelIndex index(int iRow, int iColumn, const QModelIndex& parent = QModelIndex()) const override;
  virtual QModelIndex parent(const QModelIndex& index) const override;
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

private:
  void UpdateVisibleEntries();

private:
  const wdTestResultData* m_pCurrentTestSelection;
  const wdTestFrameworkResult* m_pTestResult;
  std::vector<wdUInt32> m_VisibleEntries;
  std::vector<wdUInt8> m_VisibleEntriesIndention;
};

#endif

