#pragma once

#ifdef WD_USE_QT

#  include <QStyledItemDelegate>
#  include <TestFramework/Framework/Qt/qtTestFramework.h>
#  include <TestFramework/TestFrameworkDLL.h>

class wdQtTestFramework;

/// \brief Delegate for wdQtTestModel which shows bars for the test durations.
class WD_TEST_DLL wdQtTestDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  wdQtTestDelegate(QObject* pParent);
  virtual ~wdQtTestDelegate();

public: // QStyledItemDelegate interface
  virtual void paint(QPainter* pPainter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif

