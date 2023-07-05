#pragma once

#include <TestFramework/Framework/Declarations.h>
#include <TestFramework/TestFrameworkDLL.h>
#include <deque>
#include <string>
#include <vector>

struct wdTestOutput
{
  /// \brief Defines the type of output message for wdTestOutputMessage.
  enum Enum
  {
    InvalidType = -1,
    StartOutput = 0,
    BeginBlock,
    EndBlock,
    ImportantInfo,
    Details,
    Success,
    Message,
    Warning,
    Error,
    ImageDiffFile,
    Duration,
    FinalResult,
    AllOutputTypes
  };

  static const char* const s_Names[];
  static const char* ToString(Enum type);
  static Enum FromString(const char* szName);
};

/// \brief A message of type wdTestOutput::Enum, stored in wdResult.
struct wdTestErrorMessage
{
  wdTestErrorMessage()
    : m_iLine(-1)
  {
  }

  std::string m_sError;
  std::string m_sBlock;
  std::string m_sFile;
  wdInt32 m_iLine;
  std::string m_sFunction;
  std::string m_sMessage;
};

/// \brief A message of type wdTestOutput::Enum, stored in wdResult.
struct wdTestOutputMessage
{
  wdTestOutputMessage()
    : m_Type(wdTestOutput::ImportantInfo)
    , m_iErrorIndex(-1)
  {
  }

  wdTestOutput::Enum m_Type;
  std::string m_sMessage;
  wdInt32 m_iErrorIndex;
};

struct wdTestResultQuery
{
  /// \brief Defines what information should be accumulated over the sub-tests in wdTestEntry::GetSubTestCount.
  enum Enum
  {
    Count,
    Executed,
    Success,
    Errors,
  };
};

/// \brief Stores the results of a test run. Used by both wdTestEntry and wdSubTestEntry.
struct wdTestResultData
{
  wdTestResultData()
    : m_bExecuted(false)
    , m_bSuccess(false)
    , m_iTestAsserts(0)
    , m_fTestDuration(0.0)
    , m_iFirstOutput(-1)
    , m_iLastOutput(-1)
  {
  }
  void Reset();
  void AddOutput(wdInt32 iOutputIndex);

  std::string m_sName;
  bool m_bExecuted;       ///< Whether the test was executed. If false, the test was either deactivated or the test process crashed before
                          ///< executing it.
  bool m_bSuccess;        ///< Whether the test succeeded or not.
  int m_iTestAsserts;     ///< Asserts that were checked. For tests this includes the count of all of their sub-tests as well.
  double m_fTestDuration; ///< Duration of the test/sub-test. For tests, this includes the duration of all their sub-tests as well.
  wdInt32 m_iFirstOutput; ///< First output message. For tests, this range includes all messages of their sub-tests as well.
  wdInt32 m_iLastOutput;  ///< Last output message. For tests, this range includes all messages of their sub-tests as well.
};

struct wdTestConfiguration
{
  wdTestConfiguration();

  wdUInt64 m_uiInstalledMainMemory;
  wdUInt32 m_uiMemoryPageSize;
  wdUInt32 m_uiCPUCoreCount;
  bool m_b64BitOS;
  bool m_b64BitApplication;
  std::string m_sPlatformName;
  std::string m_sBuildConfiguration; ///< Debug, Release, etc
  wdInt64 m_iDateTime;               ///< in seconds since Linux epoch
  wdInt32 m_iRCSRevision;
  std::string m_sHostName;
};

class wdTestFrameworkResult
{
public:
  wdTestFrameworkResult() {}

  // Manage tests
  void Clear();
  void SetupTests(const std::deque<wdTestEntry>& tests, const wdTestConfiguration& config);
  void Reset();
  bool WriteJsonToFile(const char* szFileName) const;

  // Result access
  wdUInt32 GetTestCount(wdTestResultQuery::Enum countQuery = wdTestResultQuery::Count) const;
  wdUInt32 GetSubTestCount(wdUInt32 uiTestIndex, wdTestResultQuery::Enum countQuery = wdTestResultQuery::Count) const;
  wdInt32 GetTestIndexByName(const char* szTestName) const;
  wdInt32 GetSubTestIndexByName(wdUInt32 uiTestIndex, const char* szSubTestName) const;
  double GetTotalTestDuration() const;
  const wdTestResultData& GetTestResultData(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex) const;

  // Test output
  void TestOutput(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, wdTestOutput::Enum type, const char* szMsg);
  void TestError(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, const char* szError, const char* szBlock, const char* szFile, wdInt32 iLine,
    const char* szFunction, const char* szMsg);
  void TestResult(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, bool bSuccess, double fDuration);
  void AddAsserts(wdUInt32 uiTestIndex, wdInt32 iSubTestIndex, int iCount);

  // Messages / Errors
  wdUInt32 GetOutputMessageCount(wdInt32 iTestIndex = -1, wdInt32 iSubTestIndex = -1, wdTestOutput::Enum type = wdTestOutput::AllOutputTypes) const;
  const wdTestOutputMessage* GetOutputMessage(wdUInt32 uiOutputMessageIdx) const;

  wdUInt32 GetErrorMessageCount(wdInt32 iTestIndex = -1, wdInt32 iSubTestIndex = -1) const;
  const wdTestErrorMessage* GetErrorMessage(wdUInt32 uiErrorMessageIdx) const;

private:
  struct wdSubTestResult
  {
    wdSubTestResult() {}
    wdSubTestResult(const char* szName) { m_Result.m_sName = szName; }

    wdTestResultData m_Result;
  };

  struct wdTestResult
  {
    wdTestResult() {}
    wdTestResult(const char* szName) { m_Result.m_sName = szName; }

    void Reset();

    wdTestResultData m_Result;
    std::deque<wdSubTestResult> m_SubTests;
  };

private:
  wdTestConfiguration m_Config;
  std::deque<wdTestResult> m_Tests;
  std::deque<wdTestErrorMessage> m_Errors;
  std::deque<wdTestOutputMessage> m_TestOutput;
};
