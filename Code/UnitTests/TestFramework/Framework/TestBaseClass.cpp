#include <TestFramework/TestFrameworkPCH.h>

#include <TestFramework/Framework/TestFramework.h>

WD_ENUMERABLE_CLASS_IMPLEMENTATION(wdTestBaseClass);

const char* wdTestBaseClass::GetSubTestName(wdInt32 iIdentifier) const
{
  if (iIdentifier < 0 || static_cast<std::size_t>(iIdentifier) > m_Entries.size())
  {
    wdLog::Error("Tried to access retrieve sub-test name using invalid identifier.");
    return "";
  }

  return m_Entries[iIdentifier].m_szName;
}

void wdTestBaseClass::UpdateConfiguration(wdTestConfiguration& ref_config) const
{
  // If the configuration hasn't been set yet this is the first instance of wdTestBaseClass being called
  // to fill in the configuration and we thus have to do so.
  // Derived classes can have more information (e.g.GPU info) and there is no way to know which instance
  // of wdTestBaseClass may have additional information so we ask all of them and each one early outs
  // if the information it knows about is already present.
  if (ref_config.m_uiInstalledMainMemory == 0)
  {
    const wdSystemInformation& pSysInfo = wdSystemInformation::Get();
    ref_config.m_uiInstalledMainMemory = pSysInfo.GetInstalledMainMemory();
    ref_config.m_uiMemoryPageSize = pSysInfo.GetMemoryPageSize();
    ref_config.m_uiCPUCoreCount = pSysInfo.GetCPUCoreCount();
    ref_config.m_sPlatformName = pSysInfo.GetPlatformName();
    ref_config.m_b64BitOS = pSysInfo.Is64BitOS();
    ref_config.m_b64BitApplication = WD_ENABLED(WD_PLATFORM_64BIT);
    ref_config.m_sBuildConfiguration = pSysInfo.GetBuildConfiguration();
    ref_config.m_iDateTime = wdTimestamp::CurrentTimestamp().GetInt64(wdSIUnitOfTime::Second);
    ref_config.m_iRCSRevision = wdTestFramework::GetInstance()->GetSettings().m_iRevision;
    ref_config.m_sHostName = pSysInfo.GetHostName();
  }
}

void wdTestBaseClass::MapImageNumberToString(
  const char* szTestName, const char* szSubTestName, wdUInt32 uiImageNumber, wdStringBuilder& out_sString) const
{
  out_sString.Format("{0}_{1}_{2}", szTestName, szSubTestName, wdArgI(uiImageNumber, 3, true));
  out_sString.ReplaceAll(" ", "_");
}

void wdTestBaseClass::ClearSubTests()
{
  m_Entries.clear();
}

void wdTestBaseClass::AddSubTest(const char* szName, wdInt32 iIdentifier)
{
  WD_ASSERT_DEV(szName != nullptr, "Sub test name must not be nullptr");

  TestEntry e;
  e.m_szName = szName;
  e.m_iIdentifier = iIdentifier;

  m_Entries.push_back(e);
}

wdResult wdTestBaseClass::DoTestInitialization()
{
  try
  {
    if (InitializeTest() == WD_FAILURE)
    {
      wdTestFramework::Output(wdTestOutput::Error, "Test Initialization failed.");
      return WD_FAILURE;
    }
  }
  catch (...)
  {
    wdTestFramework::Output(wdTestOutput::Error, "Exception during test initialization.");
    return WD_FAILURE;
  }

  return WD_SUCCESS;
}

void wdTestBaseClass::DoTestDeInitialization()
{
  try

  {
    if (DeInitializeTest() == WD_FAILURE)
      wdTestFramework::Output(wdTestOutput::Error, "Test DeInitialization failed.");
  }
  catch (...)
  {
    wdTestFramework::Output(wdTestOutput::Error, "Exception during test de-initialization.");
  }
}

wdResult wdTestBaseClass::DoSubTestInitialization(wdInt32 iIdentifier)
{
  try
  {
    if (InitializeSubTest(iIdentifier) == WD_FAILURE)
    {
      wdTestFramework::Output(wdTestOutput::Error, "Sub-Test Initialization failed, skipping Test.");
      return WD_FAILURE;
    }
  }
  catch (...)
  {
    wdTestFramework::Output(wdTestOutput::Error, "Exception during sub-test initialization.");
    return WD_FAILURE;
  }

  return WD_SUCCESS;
}

void wdTestBaseClass::DoSubTestDeInitialization(wdInt32 iIdentifier)
{
  try
  {
    if (DeInitializeSubTest(iIdentifier) == WD_FAILURE)
      wdTestFramework::Output(wdTestOutput::Error, "Sub-Test De-Initialization failed.");
  }
  catch (...)
  {
    wdTestFramework::Output(wdTestOutput::Error, "Exception during sub-test de-initialization.");
  }
}

wdTestAppRun wdTestBaseClass::DoSubTestRun(wdInt32 iIdentifier, double& fDuration, wdUInt32 uiInvocationCount)
{
  fDuration = 0.0;

  wdTestAppRun ret = wdTestAppRun::Quit;

  try
  {
    wdTime StartTime = wdTime::Now();

    ret = RunSubTest(iIdentifier, uiInvocationCount);

    fDuration = (wdTime::Now() - StartTime).GetMilliseconds();
  }
  catch (...)
  {
    wdInt32 iEntry = -1;

    for (wdInt32 i = 0; i < (wdInt32)m_Entries.size(); ++i)
    {
      if (m_Entries[i].m_iIdentifier == iIdentifier)
      {
        iEntry = i;
        break;
      }
    }

    if (iEntry >= 0)
      wdTestFramework::Output(wdTestOutput::Error, "Exception during sub-test '%s'.", m_Entries[iEntry].m_szName);
    else
      wdTestFramework::Output(wdTestOutput::Error, "Exception during unknown sub-test.");
  }

  return ret;
}


WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_TestBaseClass);
