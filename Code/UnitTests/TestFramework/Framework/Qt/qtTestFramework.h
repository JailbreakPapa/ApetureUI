#pragma once

#ifdef WD_USE_QT

#  include <QObject>
#  include <TestFramework/Framework/TestFramework.h>
#  include <TestFramework/TestFrameworkDLL.h>

/// \brief Derived wdTestFramework which signals the GUI to update whenever a new tests result comes in.
class WD_TEST_DLL wdQtTestFramework : public QObject, public wdTestFramework
{
  Q_OBJECT
public:
  wdQtTestFramework(const char* szTestName, const char* szAbsTestDir, const char* szRelTestDataDir, int iArgc, const char** pArgv);
  virtual ~wdQtTestFramework();

private:
  wdQtTestFramework(wdQtTestFramework&);
  void operator=(wdQtTestFramework&);

Q_SIGNALS:
  void TestResultReceived(qint32 testIndex, qint32 subTestIndex);

protected:
  virtual void OutputImpl(wdTestOutput::Enum Type, const char* szMsg) override;
  virtual void TestResultImpl(wdInt32 iSubTestIndex, bool bSuccess, double fDuration) override;
};

#endif

