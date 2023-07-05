#include <TestFramework/TestFrameworkPCH.h>

#ifdef WD_USE_QT
#  include <TestFramework/Framework/Qt/qtTestFramework.h>

////////////////////////////////////////////////////////////////////////
// wdQtTestFramework public functions
////////////////////////////////////////////////////////////////////////

wdQtTestFramework::wdQtTestFramework(const char* szTestName, const char* szAbsTestDir, const char* szRelTestDataDir, int iArgc, const char** pArgv)
  : wdTestFramework(szTestName, szAbsTestDir, szRelTestDataDir, iArgc, pArgv)
{
  Q_INIT_RESOURCE(resources);
  Initialize();
}

wdQtTestFramework::~wdQtTestFramework() {}


////////////////////////////////////////////////////////////////////////
// wdQtTestFramework protected functions
////////////////////////////////////////////////////////////////////////

void wdQtTestFramework::OutputImpl(wdTestOutput::Enum Type, const char* szMsg)
{
  wdTestFramework::OutputImpl(Type, szMsg);
}

void wdQtTestFramework::TestResultImpl(wdInt32 iSubTestIndex, bool bSuccess, double fDuration)
{
  wdTestFramework::TestResultImpl(iSubTestIndex, bSuccess, fDuration);
  Q_EMIT TestResultReceived(m_iCurrentTestIndex, iSubTestIndex);
}

#endif

WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_Qt_qtTestFramework);
