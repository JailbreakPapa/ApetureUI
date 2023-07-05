#include <TestFramework/TestFrameworkPCH.h>

#include <Foundation/Profiling/Profiling.h>
#include <TestFramework/Framework/TestFramework.h>

WD_ENUMERABLE_CLASS_IMPLEMENTATION(wdRegisterSimpleTestHelper);

void wdSimpleTestGroup::AddSimpleTest(const char* szName, SimpleTestFunc testFunc)
{
  SimpleTestEntry e;
  e.m_szName = szName;
  e.m_Func = testFunc;

  for (wdUInt32 i = 0; i < m_SimpleTests.size(); ++i)
  {
    if ((strcmp(m_SimpleTests[i].m_szName, e.m_szName) == 0) && (m_SimpleTests[i].m_Func == e.m_Func))
      return;
  }

  m_SimpleTests.push_back(e);
}

void wdSimpleTestGroup::SetupSubTests()
{
  for (wdUInt32 i = 0; i < m_SimpleTests.size(); ++i)
  {
    AddSubTest(m_SimpleTests[i].m_szName, i);
  }
}

wdTestAppRun wdSimpleTestGroup::RunSubTest(wdInt32 iIdentifier, wdUInt32 uiInvocationCount)
{
  // until the block name is properly set, use the test name instead
  wdTestFramework::s_szTestBlockName = m_SimpleTests[iIdentifier].m_szName;

  WD_PROFILE_SCOPE(m_SimpleTests[iIdentifier].m_szName);
  m_SimpleTests[iIdentifier].m_Func();

  wdTestFramework::s_szTestBlockName = "";
  return wdTestAppRun::Quit;
}

wdResult wdSimpleTestGroup::InitializeSubTest(wdInt32 iIdentifier)
{
  // initialize everything up to 'core'
  wdStartup::StartupCoreSystems();
  return WD_SUCCESS;
}

wdResult wdSimpleTestGroup::DeInitializeSubTest(wdInt32 iIdentifier)
{
  // shut down completely
  wdStartup::ShutdownCoreSystems();
  wdMemoryTracker::DumpMemoryLeaks();
  return WD_SUCCESS;
}


WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_SimpleTest);
