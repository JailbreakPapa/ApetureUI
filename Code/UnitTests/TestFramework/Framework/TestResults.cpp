#include <TestFramework/TestFrameworkPCH.h>

#include <Foundation/Types/ScopeExit.h>
#include <TestFramework/Framework/TestResults.h>

////////////////////////////////////////////////////////////////////////
// wdTestOutput public functions
////////////////////////////////////////////////////////////////////////

const char* const wdTestOutput::s_Names[] = {
  "StartOutput", "BeginBlock", "EndBlock", "ImportantInfo", "Details", "Success", "Message", "Warning", "Error", "Duration", "FinalResult"};

const char* wdTestOutput::ToString(Enum type)
{
  return s_Names[type];
}

wdTestOutput::Enum wdTestOutput::FromString(const char* szName)
{
  for (wdUInt32 i = 0; i < AllOutputTypes; ++i)
  {
    if (strcmp(szName, s_Names[i]) == 0)
      return (wdTestOutput::Enum)i;
  }
  return InvalidType;
}


////////////////////////////////////////////////////////////////////////
// wdTestResultData public functions
////////////////////////////////////////////////////////////////////////

void wdTestResultData::Reset()
{
  m_bExecuted = false;
  m_bSuccess = false;
  m_iTestAsserts = 0;
  m_fTestDuration = 0.0;
  m_iFirstOutput = -1;
  m_iLastOutput = -1;
}

void wdTestResultData::AddOutput(wdInt32 iOutputIndex)
{
  if (m_iFirstOutput == -1)
  {
    m_iFirstOutput = iOutputIndex;
    m_iLastOutput = iOutputIndex;
  }
  else
  {
    m_iLastOutput = iOutputIndex;
  }
}


////////////////////////////////////////////////////////////////////////
// wdTestResultData public functions
////////////////////////////////////////////////////////////////////////

wdTestConfiguration::wdTestConfiguration()
  : m_uiInstalledMainMemory(0)
  , m_uiMemoryPageSize(0)
  , m_uiCPUCoreCount(0)
  , m_b64BitOS(false)
  , m_b64BitApplication(false)
  , m_iDateTime(0)
  , m_iRCSRevision(-1)
{
}


////////////////////////////////////////////////////////////////////////
// wdTestFrameworkResult public functions
////////////////////////////////////////////////////////////////////////

void wdTestFrameworkResult::Clear()
{
  m_Tests.clear();
  m_Errors.clear();
  m_TestOutput.clear();
}

void wdTestFrameworkResult::SetupTests(const std::deque<wdTestEntry>& tests, const wdTestConfiguration& config)
{
  m_Config = config;
  Clear();

  const wdUInt32 uiTestCount = (wdUInt32)tests.size();
  for (wdUInt32 uiTestIdx = 0; uiTestIdx < uiTestCount; ++uiTestIdx)
  {
    m_Tests.push_back(wdTestResult(tests[uiTestIdx].m_szTestName));

    const wdUInt32 uiSubTestCount = (wdUInt32)tests[uiTestIdx].m_SubTests.size();
    for (wdUInt32 uiSubTestIdx = 0; uiSubTestIdx < uiSubTestCount; ++uiSubTestIdx)
    {
      m_Tests[uiTestIdx].m_SubTests.push_back(wdSubTestResult(tests[uiTestIdx].m_SubTests[uiSubTestIdx].m_szSubTestName));
    }
  }
}

void ::wdTestFrameworkResult::Reset()
{
  const wdUInt32 uiTestCount = (wdUInt32)m_Tests.size();
  for (wdUInt32 uiTestIdx = 0; uiTestIdx < uiTestCount; ++uiTestIdx)
  {
    m_Tests[uiTestIdx].Reset();
  }
  m_Errors.clear();
  m_TestOutput.clear();
}

bool wdTestFrameworkResult::WriteJsonToFile(const char* szFileName) const
{
  wdStartup::StartupCoreSystems();
  WD_SCOPE_EXIT(wdStartup::ShutdownCoreSystems());

  {
    wdStringBuilder jsonFilename;
    if (wdPathUtils::IsAbsolutePath(szFileName))
    {
      // Make sure we can access raw absolute file paths
      if (wdFileSystem::AddDataDirectory("", "jsonoutput", ":", wdFileSystem::AllowWrites).Failed())
        return false;

      jsonFilename = szFileName;
    }
    else
    {
      // If this is a relative path, we use the wdtest/ data directory to make sure that this works properly with the fileserver.
      if (wdFileSystem::AddDataDirectory(">wdtest/", "jsonoutput", ":", wdFileSystem::AllowWrites).Failed())
        return false;

      jsonFilename = ":";
      jsonFilename.AppendPath(szFileName);
    }

    wdFileWriter file;
    if (file.Open(jsonFilename).Failed())
    {
      return false;
    }
    wdStandardJSONWriter js;
    js.SetOutputStream(&file);

    js.BeginObject();
    {
      js.BeginObject("configuration");
      {
        js.AddVariableUInt64("m_uiInstalledMainMemory", m_Config.m_uiInstalledMainMemory);
        js.AddVariableUInt32("m_uiMemoryPageSize", m_Config.m_uiMemoryPageSize);
        js.AddVariableUInt32("m_uiCPUCoreCount", m_Config.m_uiCPUCoreCount);
        js.AddVariableBool("m_b64BitOS", m_Config.m_b64BitOS);
        js.AddVariableBool("m_b64BitApplication", m_Config.m_b64BitApplication);
        js.AddVariableString("m_sPlatformName", m_Config.m_sPlatformName.c_str());
        js.AddVariableString("m_sBuildConfiguration", m_Config.m_sBuildConfiguration.c_str());
        js.AddVariableInt64("m_iDateTime", m_Config.m_iDateTime);
        js.AddVariableInt32("m_iRCSRevision", m_Config.m_iRCSRevision);
        js.AddVariableString("m_sHostName", m_Config.m_sHostName.c_str());
      }
      js.EndObject();

      // Output Messages
      js.BeginArray("messages");
      {
        wdUInt32 uiMessages = GetOutputMessageCount();
        for (wdUInt32 uiMessageIdx = 0; uiMessageIdx < uiMessages; ++uiMessageIdx)
        {
          const wdTestOutputMessage* pMessage = GetOutputMessage(uiMessageIdx);
          js.BeginObject();
          {
            js.AddVariableString("m_Type", wdTestOutput::ToString(pMessage->m_Type));
            js.AddVariableString("m_sMessage", pMessage->m_sMessage.c_str());
            if (pMessage->m_iErrorIndex != -1)
              js.AddVariableInt32("m_iErrorIndex", pMessage->m_iErrorIndex);
          }
          js.EndObject();
        }
      }
      js.EndArray();

      // Error Messages
      js.BeginArray("errors");
      {
        wdUInt32 uiMessages = GetErrorMessageCount();
        for (wdUInt32 uiMessageIdx = 0; uiMessageIdx < uiMessages; ++uiMessageIdx)
        {
          const wdTestErrorMessage* pMessage = GetErrorMessage(uiMessageIdx);
          js.BeginObject();
          {
            js.AddVariableString("m_sError", pMessage->m_sError.c_str());
            js.AddVariableString("m_sBlock", pMessage->m_sBlock.c_str());
            js.AddVariableString("m_sFile", pMessage->m_sFile.c_str());
            js.AddVariableString("m_sFunction", pMessage->m_sFunction.c_str());
            js.AddVariableInt32("m_iLine", pMessage->m_iLine);
            js.AddVariableString("m_sMessage", pMessage->m_sMessage.c_str());
          }
          js.EndObject();
        }
      }
      js.EndArray();

      // Tests
      js.BeginArray("tests");
      {
        wdUInt32 uiTests = GetTestCount();
        for (wdUInt32 uiTestIdx = 0; uiTestIdx < uiTests; ++uiTestIdx)
        {
          const wdTestResultData& testResult = GetTestResultData(uiTestIdx, -1);
          js.BeginObject();
          {
            js.AddVariableString("m_sName", testResult.m_sName.c_str());
            js.AddVariableBool("m_bExecuted", testResult.m_bExecuted);
            js.AddVariableBool("m_bSuccess", testResult.m_bSuccess);
            js.AddVariableInt32("m_iTestAsserts", testResult.m_iTestAsserts);
            js.AddVariableDouble("m_fTestDuration", testResult.m_fTestDuration);
            js.AddVariableInt32("m_iFirstOutput", testResult.m_iFirstOutput);
            js.AddVariableInt32("m_iLastOutput", testResult.m_iLastOutput);

            // Sub Tests
            js.BeginArray("subTests");
            {
              wdUInt32 uiSubTests = GetSubTestCount(uiTestIdx);
              for (wdUInt32 uiSubTestIdx = 0; uiSubTestIdx < uiSubTests; ++uiSubTestIdx)
              {
                const wdTestResultData& subTestResult = GetTestResultData(uiTestIdx, uiSubTestIdx);
                js.BeginObject();
                {
                  js.AddVariableString("m_sName", subTestResult.m_sName.c_str());
                  js.AddVariableBool("m_bExecuted", subTestResult.m_bExecuted);
                  js.AddVariableBool("m_bSuccess", subTestResult.m_bSuccess);
                  js.AddVariableInt32("m_iTestAsserts", subTestResult.m_iTestAsserts);
                  js.AddVariableDouble("m_fTestDuration", subTestResult.m_fTestDuration);
                  js.AddVariableInt32("m_iFirstOutput", subTestResult.m_iFirstOutput);
                  js.AddVariableInt32("m_iLastOutput", subTestResult.m_iLastOutput);
                }
                js.EndObject();
              }
            }
            js.EndArray(); // subTests
          }
          js.EndObject();
        }
      }
      js.EndArray(); // tests
    }
    js.EndObject();
  }

  return true;
}

wdUInt32 wdTestFrameworkResult::GetTestCount(wdTestResultQuery::Enum countQuery) const
{
  wdUInt32 uiAccumulator = 0;
  const wdUInt32 uiTests = (wdUInt32)m_Tests.size();

  if (countQuery == wdTestResultQuery::Count)
    return uiTests;

  if (countQuery == wdTestResultQuery::Errors)
    return (wdUInt32)m_Errors.size();

  for (wdUInt32 uiTest = 0; uiTest < uiTests; ++uiTest)
  {
    switch (countQuery)
    {
      case wdTestResultQuery::Executed:
        uiAccumulator += m_Tests[uiTest].m_Result.m_bExecuted ? 1 : 0;
        break;
      case wdTestResultQuery::Success:
        uiAccumulator += m_Tests[uiTest].m_Result.m_bSuccess ? 1 : 0;
        break;
      default:
        break;
    }
  }
  return uiAccumulator;
}

wdUInt32 wdTestFrameworkResult::GetSubTestCount(wdUInt32 uiTestIndex, wdTestResultQuery::Enum countQuery) const
{
  if (uiTestIndex >= (wdUInt32)m_Tests.size())
    return 0;

  const wdTestResult& test = m_Tests[uiTestIndex];
  wdUInt32 uiAccumulator = 0;
  const wdUInt32 uiSubTests = (wdUInt32)test.m_SubTests.size();

  if (countQuery == wdTestResultQuery::Count)
    return uiSubTests;

  if (countQuery == wdTestResultQuery::Errors)
  {
    for (wdInt32 iOutputIdx = test.m_Result.m_iFirstOutput; iOutputIdx <= test.m_Result.m_iLastOutput && iOutputIdx != -1; ++iOutputIdx)
    {
      if (m_TestOutput[iOutputIdx].m_Type == wdTestOutput::Error)
        uiAccumulator++;
    }
    return uiAccumulator;
  }

  for (wdUInt32 uiSubTest = 0; uiSubTest < uiSubTests; ++uiSubTest)
  {
    switch (countQuery)
    {
      case wdTestResultQuery::Executed:
        uiAccumulator += test.m_SubTests[uiSubTest].m_Result.m_bExecuted ? 1 : 0;
        break;
      case wdTestResultQuery::Success:
        uiAccumulator += test.m_SubTests[uiSubTest].m_Result.m_bSuccess ? 1 : 0;
        break;
      default:
        break;
    }
  }
  return uiAccumulator;
}

wdInt32 wdTestFrameworkResult::GetTestIndexByName(const char* szTestName) const
{
  wdInt32 iTestCount = (wdInt32)GetTestCount();
  for (wdInt32 i = 0; i < iTestCount; ++i)
  {
    if (m_Tests[i].m_Result.m_sName.compare(szTestName) == 0)
      return i;
  }
  return -1;
}

wdInt32 wdTestFrameworkResult::GetSubTestIndexByName(wdUInt32 uiTestIndex, const char* szSubTestName) const
{
  if (uiTestIndex >= GetTestCount())
    return -1;

  wdInt32 iSubTestCount = (wdInt32)GetSubTestCount(uiTestIndex);
  for (wdInt32 i = 0; i < iSubTestCount; ++i)
  {
    if (m_Tests[uiTestIndex].m_SubTests[i].m_Result.m_sName.compare(szSubTestName) == 0)
      return i;
  }
  return -1;
}

double wdTestFrameworkResult::GetTotalTestDuration() const
{
  double fTotalTestDuration = 0.0;
  const wdUInt32 uiTests = (wdUInt32)m_Tests.size();
  for (wdUInt32 uiTest = 0; uiTest < uiTests; ++uiTest)
  {
    fTotalTestDuration += m_Tests[uiTest].m_Result.m_fTestDuration;
  }
  return fTotalTestDuration;
}

const wdTestResultData& wdTestFrameworkResult::GetTestResultData(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex) const
{
  return (iSubTestIndex == -1) ? m_Tests[uiTestIndex].m_Result : m_Tests[uiTestIndex].m_SubTests[iSubTestIndex].m_Result;
}

void wdTestFrameworkResult::TestOutput(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, wdTestOutput::Enum type, const char* szMsg)
{
  if (uiTestIndex != -1)
  {
    m_Tests[uiTestIndex].m_Result.AddOutput((wdInt32)m_TestOutput.size());
    if (iSubTestIndex != -1)
    {
      m_Tests[uiTestIndex].m_SubTests[iSubTestIndex].m_Result.AddOutput((wdInt32)m_TestOutput.size());
    }
  }

  m_TestOutput.push_back(wdTestOutputMessage());
  wdTestOutputMessage& outputMessage = *m_TestOutput.rbegin();
  outputMessage.m_Type = type;
  outputMessage.m_sMessage.assign(szMsg);
}

void wdTestFrameworkResult::TestError(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, const char* szError, const char* szBlock, const char* szFile,
  wdInt32 iLine, const char* szFunction, const char* szMsg)
{
  // In case there is no message set, we use the error as the message.
  TestOutput(uiTestIndex, iSubTestIndex, wdTestOutput::Error, szError);
  m_TestOutput.rbegin()->m_iErrorIndex = (wdInt32)m_Errors.size();

  m_Errors.push_back(wdTestErrorMessage());
  wdTestErrorMessage& errorMessage = *m_Errors.rbegin();
  errorMessage.m_sError.assign(szError);
  errorMessage.m_sBlock.assign(szBlock);
  errorMessage.m_sFile.assign(szFile);
  errorMessage.m_iLine = iLine;
  errorMessage.m_sFunction.assign(szFunction);
  errorMessage.m_sMessage.assign(szMsg);
}

void wdTestFrameworkResult::TestResult(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, bool bSuccess, double fDuration)
{
  wdTestResultData& Result = (iSubTestIndex == -1) ? m_Tests[uiTestIndex].m_Result : m_Tests[uiTestIndex].m_SubTests[iSubTestIndex].m_Result;

  Result.m_bExecuted = true;
  Result.m_bSuccess = bSuccess;
  Result.m_fTestDuration = fDuration;

  // Accumulate sub-test duration onto test duration to get duration feedback while the sub-tests are running.
  // Final time will be set again once the entire test finishes and currently these times are identical as
  // init and de-init times aren't measured at the moment due to missing timer when engine is shut down.
  if (iSubTestIndex != -1)
  {
    m_Tests[uiTestIndex].m_Result.m_fTestDuration += fDuration;
  }
}

void wdTestFrameworkResult::AddAsserts(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, int iCount)
{
  if (uiTestIndex != -1)
  {
    m_Tests[uiTestIndex].m_Result.m_iTestAsserts += iCount;
  }

  if (iSubTestIndex != -1)
  {
    m_Tests[uiTestIndex].m_SubTests[iSubTestIndex].m_Result.m_iTestAsserts += iCount;
  }
}

wdUInt32 wdTestFrameworkResult::GetOutputMessageCount(wdInt32 iTestIndex, wdInt32 iSubTestIndex, wdTestOutput::Enum type) const
{
  if (iTestIndex == -1 && type == wdTestOutput::AllOutputTypes)
    return (wdUInt32)m_TestOutput.size();

  wdInt32 iStartIdx = 0;
  wdInt32 iEndIdx = (wdInt32)m_TestOutput.size() - 1;

  if (iTestIndex != -1)
  {
    const wdTestResultData& result = GetTestResultData(iTestIndex, iSubTestIndex);
    iStartIdx = result.m_iFirstOutput;
    iEndIdx = result.m_iLastOutput;

    // If no messages have been output (yet) for the given test we early-out here.
    if (iStartIdx == -1)
      return 0;

    // If all message types should be counted we can simply return the range.
    if (type == wdTestOutput::AllOutputTypes)
      return iEndIdx - iStartIdx + 1;
  }

  wdUInt32 uiAccumulator = 0;
  for (wdInt32 uiOutputMessageIdx = iStartIdx; uiOutputMessageIdx <= iEndIdx; ++uiOutputMessageIdx)
  {
    if (m_TestOutput[uiOutputMessageIdx].m_Type == type)
      uiAccumulator++;
  }
  return uiAccumulator;
}

const wdTestOutputMessage* wdTestFrameworkResult::GetOutputMessage(wdUInt32 uiOutputMessageIdx) const
{
  return &m_TestOutput[uiOutputMessageIdx];
}

wdUInt32 wdTestFrameworkResult::GetErrorMessageCount(wdInt32 iTestIndex, wdInt32 iSubTestIndex) const
{
  // If no test is given we can simply return the total error count.
  if (iTestIndex == -1)
  {
    return (wdUInt32)m_Errors.size();
  }

  return GetOutputMessageCount(iTestIndex, iSubTestIndex, wdTestOutput::Error);
}

const wdTestErrorMessage* wdTestFrameworkResult::GetErrorMessage(wdUInt32 uiErrorMessageIdx) const
{
  return &m_Errors[uiErrorMessageIdx];
}


////////////////////////////////////////////////////////////////////////
// wdTestFrameworkResult public functions
////////////////////////////////////////////////////////////////////////

void wdTestFrameworkResult::wdTestResult::Reset()
{
  m_Result.Reset();
  const wdUInt32 uiSubTestCount = (wdUInt32)m_SubTests.size();
  for (wdUInt32 uiSubTest = 0; uiSubTest < uiSubTestCount; ++uiSubTest)
  {
    m_SubTests[uiSubTest].m_Result.Reset();
  }
}



WD_STATICLINK_FILE(TestFramework, TestFramework_Framework_TestResults);
