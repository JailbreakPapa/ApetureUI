#pragma once

#include <TestFramework/Framework/Declarations.h>
#include <TestFramework/Framework/SimpleTest.h>
#include <TestFramework/Framework/TestBaseClass.h>
#include <TestFramework/Framework/TestResults.h>
#include <TestFramework/TestFrameworkDLL.h>

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/String.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>

class wdCommandLineUtils;

// Disable C++/CX adds.
#pragma warning(disable : 4447)

class WD_TEST_DLL wdTestFramework
{
public:
  wdTestFramework(const char* szTestName, const char* szAbsTestOutputDir, const char* szRelTestDataDir, int iArgc, const char** pArgv);
  virtual ~wdTestFramework();

  typedef void (*OutputHandler)(wdTestOutput::Enum Type, const char* szMsg);

  // Test management
  void CreateOutputFolder();
  void UpdateReferenceImages();
  const char* GetTestName() const;
  const char* GetAbsOutputPath() const;
  const char* GetRelTestDataPath() const;
  const char* GetAbsTestOrderFilePath() const;
  const char* GetAbsTestSettingsFilePath() const;
  void RegisterOutputHandler(OutputHandler handler);
  void GatherAllTests();
  void LoadTestOrder();
  void ApplyTestOrderFromCommandLine(const wdCommandLineUtils& cmd);
  void LoadTestSettings();
  void AutoSaveTestOrder();
  void SaveTestOrder(const char* const szFilePath);
  void SaveTestSettings(const char* const szFilePath);
  void SetAllTestsEnabledStatus(bool bEnable);
  void SetAllFailedTestsEnabledStatus();
  // Each function on a test must not take longer than the given time or the test process will be terminated.
  void SetTestTimeout(wdUInt32 uiTestTimeoutMS);
  wdUInt32 GetTestTimeout() const;
  void GetTestSettingsFromCommandLine(const wdCommandLineUtils& cmd);

  // Test execution
  void ResetTests();
  wdTestAppRun RunTestExecutionLoop();

  void StartTests();
  void ExecuteNextTest();
  void EndTests();
  void AbortTests();

  // Test queries
  wdUInt32 GetTestCount() const;
  wdUInt32 GetTestEnabledCount() const;
  wdUInt32 GetSubTestEnabledCount(wdUInt32 uiTestIndex) const;
  const std::string& IsTestAvailable(wdUInt32 uiTestIndex) const;
  bool IsTestEnabled(wdUInt32 uiTestIndex) const;
  bool IsSubTestEnabled(wdUInt32 uiTestIndex, wdUInt32 uiSubTestIndex) const;
  void SetTestEnabled(wdUInt32 uiTestIndex, bool bEnabled);
  void SetSubTestEnabled(wdUInt32 uiTestIndex, wdUInt32 uiSubTestIndex, bool bEnabled);

  wdInt32 GetCurrentTestIndex() const { return m_iCurrentTestIndex; }
  wdInt32 GetCurrentSubTestIndex() const { return m_iCurrentSubTestIndex; }
  wdTestEntry* GetTest(wdUInt32 uiTestIndex);
  const wdTestEntry* GetTest(wdUInt32 uiTestIndex) const;
  bool GetTestsRunning() const { return m_bTestsRunning; }

  const wdTestEntry* GetCurrentTest() const;
  const wdSubTestEntry* GetCurrentSubTest() const;

  // Global settings
  TestSettings GetSettings() const;
  void SetSettings(const TestSettings& settings);

  // Test results
  wdTestFrameworkResult& GetTestResult();
  wdInt32 GetTotalErrorCount() const;
  wdInt32 GetTestsPassedCount() const;
  wdInt32 GetTestsFailedCount() const;
  double GetTotalTestDuration() const;

  // Image comparison
  void ScheduleImageComparison(wdUInt32 uiImageNumber, wdUInt32 uiMaxError);
  void ScheduleDepthImageComparison(wdUInt32 uiImageNumber, wdUInt32 uiMaxError);
  bool IsImageComparisonScheduled() const { return m_bImageComparisonScheduled; }
  bool IsDepthImageComparisonScheduled() const { return m_bDepthImageComparisonScheduled; }
  void GenerateComparisonImageName(wdUInt32 uiImageNumber, wdStringBuilder& ref_sImgName);
  void GetCurrentComparisonImageName(wdStringBuilder& ref_sImgName);
  void SetImageReferenceFolderName(const char* szFolderName);
  void SetImageReferenceOverrideFolderName(const char* szFolderName);

  /// \brief Writes an Html file that contains test information and an image diff view for failed image comparisons.
  void WriteImageDiffHtml(const char* szFileName, wdImage& ref_referenceImgRgb, wdImage& ref_referenceImgAlpha, wdImage& ref_capturedImgRgb,
    wdImage& ref_capturedImgAlpha, wdImage& ref_diffImgRgb, wdImage& ref_diffImgAlpha, wdUInt32 uiError, wdUInt32 uiThreshold, wdUInt8 uiMinDiffRgb,
    wdUInt8 uiMaxDiffRgb, wdUInt8 uiMinDiffAlpha, wdUInt8 uiMaxDiffAlpha);

  bool PerformImageComparison(wdStringBuilder sImgName, const wdImage& img, wdUInt32 uiMaxError, char* szErrorMsg);
  bool CompareImages(wdUInt32 uiImageNumber, wdUInt32 uiMaxError, char* szErrorMsg, bool bIsDepthImage = false);

  /// \brief A function to be called to add extra info to image diff output, that is not available from here.
  /// E.g. device specific info like driver version.
  typedef std::function<wdDynamicArray<std::pair<wdString, wdString>>()> ImageDiffExtraInfoCallback;
  void SetImageDiffExtraInfoCallback(ImageDiffExtraInfoCallback provider);

  typedef std::function<void(bool)> ImageComparisonCallback; /// \brief A function to be called after every image comparison with a bool
                                                             /// indicating if the images matched or not.
  void SetImageComparisonCallback(const ImageComparisonCallback& callback);

  static wdResult CaptureRegressionStat(wdStringView sTestName, wdStringView sName, wdStringView sUnit, float value, wdInt32 iTestId = -1);

protected:
  void Initialize();
  void DeInitialize();

  /// \brief Will be called for test failures to record the location of the failure and forward the error to OutputImpl.
  virtual void ErrorImpl(const char* szError, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg);
  /// \brief Receives wdLog messages (via LogWriter) as well as test-framework internal logging. Any wdTestOutput::Error will
  /// cause the test to fail.
  virtual void OutputImpl(wdTestOutput::Enum Type, const char* szMsg);
  virtual void TestResultImpl(wdInt32 iSubTestIndex, bool bSuccess, double fDuration);
  void FlushAsserts();
  void TimeoutThread();
  void UpdateTestTimeout();

  // ignore this for now
public:
  static const char* s_szTestBlockName;
  static int s_iAssertCounter;
  static bool s_bCallstackOnAssert;
  static wdLog::TimestampMode s_LogTimestampMode;

  // static functions
public:
  static WD_ALWAYS_INLINE wdTestFramework* GetInstance() { return s_pInstance; }

  /// \brief Returns whether to asset on test failure.
  static bool GetAssertOnTestFail();

  static void Output(wdTestOutput::Enum type, const char* szMsg, ...);
  static void OutputArgs(wdTestOutput::Enum type, const char* szMsg, va_list szArgs);
  static void Error(const char* szError, const char* szFile, wdInt32 iLine, const char* szFunction, wdStringView sMsg, ...);
  static void Error(const char* szError, const char* szFile, wdInt32 iLine, const char* szFunction, wdStringView sMsg, va_list szArgs);
  static void TestResult(wdInt32 iSubTestIndex, bool bSuccess, double fDuration);

  // static members
private:
  static wdTestFramework* s_pInstance;

private:
  std::string m_sTestName;                ///< The name of the tests being done
  std::string m_sAbsTestOutputDir;        ///< Absolute path to the output folder where results and temp data is stored
  std::string m_sRelTestDataDir;          ///< Relative path from the SDK to where the unit test data is located
  std::string m_sAbsTestOrderFilePath;    ///< Absolute path to the test order file
  std::string m_sAbsTestSettingsFilePath; ///< Absolute path to the test settings file
  wdInt32 m_iErrorCount = 0;
  wdInt32 m_iTestsFailed = 0;
  wdInt32 m_iTestsPassed = 0;
  TestSettings m_Settings;
  std::recursive_mutex m_OutputMutex;
  std::deque<OutputHandler> m_OutputHandlers;
  std::deque<wdTestEntry> m_TestEntries;
  wdTestFrameworkResult m_Result;
  wdAssertHandler m_PreviousAssertHandler = nullptr;
  ImageDiffExtraInfoCallback m_ImageDiffExtraInfoCallback;
  ImageComparisonCallback m_ImageComparisonCallback;

  std::mutex m_TimeoutLock;
  wdUInt32 m_uiTimeoutMS = 5 * 60 * 1000; // 5 min default timeout
  bool m_bUseTimeout = false;
  bool m_bArm = false;
  std::condition_variable m_TimeoutCV;
  std::thread m_TimeoutThread;

  wdInt32 m_iExecutingTest = 0;
  wdInt32 m_iExecutingSubTest = 0;
  bool m_bSubTestInitialized = false;
  bool m_bAbortTests = false;
  wdUInt8 m_uiPassesLeft = 0;
  double m_fTotalTestDuration = 0.0;
  double m_fTotalSubTestDuration = 0.0;
  wdInt32 m_iErrorCountBeforeTest = 0;
  wdUInt32 m_uiSubTestInvocationCount = 0;

  bool m_bIsInitialized = false;

  // image comparisons
  bool m_bImageComparisonScheduled = false;
  wdUInt32 m_uiMaxImageComparisonError = 0;
  wdUInt32 m_uiComparisonImageNumber = 0;

  bool m_bDepthImageComparisonScheduled = false;
  wdUInt32 m_uiMaxDepthImageComparisonError = 0;
  wdUInt32 m_uiComparisonDepthImageNumber = 0;

  std::string m_sImageReferenceFolderName = "Images_Reference";
  std::string m_sImageReferenceOverrideFolderName;

protected:
  wdInt32 m_iCurrentTestIndex = -1;
  wdInt32 m_iCurrentSubTestIndex = -1;
  bool m_bTestsRunning = false;
};

#ifdef WD_NV_OPTIMUS
#  undef WD_NV_OPTIMUS
#endif

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/MinWindows.h>
#  define WD_NV_OPTIMUS                                                          \
    extern "C"                                                                   \
    {                                                                            \
      _declspec(dllexport) wdMinWindows::DWORD NvOptimusEnablement = 0x00000001; \
      _declspec(dllexport) wdMinWindows::DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; \
    }
#else
#  define WD_NV_OPTIMUS
#endif

#if WD_ENABLED(WD_PLATFORM_ANDROID)
#  include <Foundation/Basics/Platform/Android/AndroidUtils.h>
#  include <android/log.h>
#  include <android/native_activity.h>
#  include <android_native_app_glue.h>


#  define WD_TESTFRAMEWORK_ENTRY_POINT_BEGIN(szTestName, szNiceTestName)                                                   \
    int wdAndroidMain(int argc, char** argv);                                                                              \
    extern "C" void android_main(struct android_app* app)                                                                  \
    {                                                                                                                      \
      wdAndroidUtils::SetAndroidApp(app);                                                                                  \
      /* TODO: do something with the return value of wdAndroidMain?  */                                                    \
      /* TODO: can we get somehow get the command line arguments to the android app? Is there even something like that? */ \
      int iReturnCode = wdAndroidMain(0, nullptr);                                                                         \
      __android_log_print(ANDROID_LOG_ERROR, "wdEngine", "Test framework exited with return code: '%d'", iReturnCode);     \
    }                                                                                                                      \
                                                                                                                           \
    int wdAndroidMain(int argc, char** argv)                                                                               \
    {                                                                                                                      \
      wdTestSetup::InitTestFramework(szTestName, szNiceTestName, 0, nullptr);                                              \
      /* Execute custom init code here by using the BEGIN/END macros directly */

#else
/// \brief Macro to define the application entry point for all test applications
#  define WD_TESTFRAMEWORK_ENTRY_POINT_BEGIN(szTestName, szNiceTestName)                    \
    /* Enables that on machines with multiple GPUs the NVIDIA GPU is preferred */           \
    WD_NV_OPTIMUS                                                                           \
    WD_APPLICATION_ENTRY_POINT_CODE_INJECTION                                               \
    int main(int argc, char** argv)                                                         \
    {                                                                                       \
      wdTestSetup::InitTestFramework(szTestName, szNiceTestName, argc, (const char**)argv); \
      /* Execute custom init code here by using the BEGIN/END macros directly */

#endif

#if WD_ENABLED(WD_PLATFORM_ANDROID)
#  define WD_TESTFRAMEWORK_ENTRY_POINT_END()                                       \
    /* TODO: This is too big for a macro now */                                    \
    auto app = wdAndroidUtils::GetAndroidApp();                                    \
    bool bRun = true;                                                              \
    while (true)                                                                   \
    {                                                                              \
      struct android_poll_source* source = nullptr;                                \
      int ident = 0;                                                               \
      int events = 0;                                                              \
      while ((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0) \
      {                                                                            \
        if (source != nullptr)                                                     \
          source->process(app, source);                                            \
      }                                                                            \
      if (bRun && wdTestSetup::RunTests() != wdTestAppRun::Continue)               \
      {                                                                            \
        bRun = false;                                                              \
        ANativeActivity_finish(app->activity);                                     \
      }                                                                            \
      if (app->destroyRequested)                                                   \
      {                                                                            \
        const wdInt32 iFailedTests = wdTestSetup::GetFailedTestCount();            \
        wdTestSetup::DeInitTestFramework();                                        \
        return iFailedTests;                                                       \
      }                                                                            \
    }                                                                              \
    }

#else
#  define WD_TESTFRAMEWORK_ENTRY_POINT_END()                        \
    while (wdTestSetup::RunTests() == wdTestAppRun::Continue)       \
    {                                                               \
    }                                                               \
    const wdInt32 iFailedTests = wdTestSetup::GetFailedTestCount(); \
    wdTestSetup::DeInitTestFramework();                             \
    return iFailedTests;                                            \
    }

#endif


#define WD_TESTFRAMEWORK_ENTRY_POINT(szTestName, szNiceTestName)             \
  WD_TESTFRAMEWORK_ENTRY_POINT_BEGIN(szTestName, szNiceTestName)             \
  /* Execute custom init code here by using the BEGIN/END macros directly */ \
  WD_TESTFRAMEWORK_ENTRY_POINT_END()

/// \brief Enum for usage in WD_TEST_BLOCK to enable or disable the block.
struct wdTestBlock
{
  /// \brief Enum for usage in WD_TEST_BLOCK to enable or disable the block.
  enum Enum
  {
    Enabled,           ///< The test block is enabled.
    Disabled,          ///< The test block will be skipped. The test framework will print a warning message, that some block is deactivated.
    DisabledNoWarning, ///< The test block will be skipped, but no warning printed. Used to deactivate 'on demand/optional' tests.
  };
};

#define safeprintf wdStringUtils::snprintf

/// \brief Starts a small test block inside a larger test.
///
/// First parameter allows to quickly disable a block depending on a condition (e.g. platform).
/// Second parameter just gives it a name for better error reporting.
/// Also skipped tests are highlighted in the output, such that people can quickly see when a test is currently deactivated.
#define WD_TEST_BLOCK(enable, name)                                                  \
  wdTestFramework::s_szTestBlockName = name;                                         \
  if (enable == wdTestBlock::Disabled)                                               \
  {                                                                                  \
    wdTestFramework::s_szTestBlockName = "";                                         \
    wdTestFramework::Output(wdTestOutput::Warning, "Skipped Test Block '%s'", name); \
  }                                                                                  \
  else if (enable == wdTestBlock::DisabledNoWarning)                                 \
  {                                                                                  \
    wdTestFramework::s_szTestBlockName = "";                                         \
  }                                                                                  \
  else


/// \brief Will trigger a debug break, if the test framework is configured to do so on test failure
#define WD_TEST_DEBUG_BREAK                   \
  if (wdTestFramework::GetAssertOnTestFail()) \
  WD_DEBUG_BREAK

#define WD_TEST_FAILURE(erroroutput, msg, ...)                                                                   \
  {                                                                                                              \
    wdTestFramework::Error(erroroutput, WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__); \
    WD_TEST_DEBUG_BREAK                                                                                          \
  }

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestBool(
  bool bCondition, const char* szErrorText, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Tests for a boolean condition, does not output an extra message.
#define WD_TEST_BOOL(condition) WD_TEST_BOOL_MSG(condition, "")

/// \brief Tests for a boolean condition, outputs a custom message on failure.
#define WD_TEST_BOOL_MSG(condition, msg, ...) \
  wdTestBool(condition, "Test failed: " WD_STRINGIZE(condition), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestResult(
  wdResult condition, const char* szErrorText, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Tests for a boolean condition, does not output an extra message.
#define WD_TEST_RESULT(condition) WD_TEST_RESULT_MSG(condition, "")

/// \brief Tests for a boolean condition, outputs a custom message on failure.
#define WD_TEST_RESULT_MSG(condition, msg, ...) \
  wdTestResult(condition, "Test failed: " WD_STRINGIZE(condition), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestResult(
  wdResult condition, const char* szErrorText, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Tests for a boolean condition, does not output an extra message.
#define WD_TEST_RESULT(condition) WD_TEST_RESULT_MSG(condition, "")

/// \brief Tests for a boolean condition, outputs a custom message on failure.
#define WD_TEST_RESULT_MSG(condition, msg, ...) \
  wdTestResult(condition, "Test failed: " WD_STRINGIZE(condition), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/// \brief Tests for a wdStatus condition, outputs wdStatus message on failure
#define WD_TEST_STATUS(condition)                 \
  auto WD_CONCAT(l_, WD_SOURCE_LINE) = condition; \
  wdTestResult(WD_CONCAT(l_, WD_SOURCE_LINE).m_Result, "Test failed: " WD_STRINGIZE(condition), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, WD_CONCAT(l_, WD_SOURCE_LINE).m_sMessage)

inline double ToFloat(int f)
{
  return static_cast<double>(f);
}

inline double ToFloat(float f)
{
  return static_cast<double>(f);
}

inline double ToFloat(double f)
{
  return static_cast<double>(f);
}

WD_TEST_DLL bool wdTestDouble(double f1, double f2, double fEps, const char* szF1, const char* szF2, const char* szFile, wdInt32 iLine,
  const char* szFunction, const char* szMsg, ...);

/// \brief Tests two floats for equality, within a given epsilon. On failure both actual and expected values are output.
#define WD_TEST_FLOAT(f1, f2, epsilon) WD_TEST_FLOAT_MSG(f1, f2, epsilon, "")

/// \brief Tests two floats for equality, within a given epsilon. On failure both actual and expected values are output, also a custom
/// message is printed.
#define WD_TEST_FLOAT_MSG(f1, f2, epsilon, msg, ...)                                                                                               \
  wdTestDouble(ToFloat(f1), ToFloat(f2), ToFloat(epsilon), WD_STRINGIZE(f1), WD_STRINGIZE(f2), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, \
    msg, ##__VA_ARGS__)


//////////////////////////////////////////////////////////////////////////

/// \brief Tests two doubles for equality, within a given epsilon. On failure both actual and expected values are output.
#define WD_TEST_DOUBLE(f1, f2, epsilon) WD_TEST_DOUBLE_MSG(f1, f2, epsilon, "")

/// \brief Tests two doubles for equality, within a given epsilon. On failure both actual and expected values are output, also a custom
/// message is printed.
#define WD_TEST_DOUBLE_MSG(f1, f2, epsilon, msg, ...)                                                                                              \
  wdTestDouble(ToFloat(f1), ToFloat(f2), ToFloat(epsilon), WD_STRINGIZE(f1), WD_STRINGIZE(f2), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, \
    msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestInt(
  wdInt64 i1, wdInt64 i2, const char* szI1, const char* szI2, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Tests two ints for equality. On failure both actual and expected values are output.
#define WD_TEST_INT(i1, i2) WD_TEST_INT_MSG(i1, i2, "")

/// \brief Tests two ints for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_INT_MSG(i1, i2, msg, ...) \
  wdTestInt(i1, i2, WD_STRINGIZE(i1), WD_STRINGIZE(i2), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestString(std::string s1, std::string s2, const char* szString1, const char* szString2, const char* szFile, wdInt32 iLine,
  const char* szFunction, const char* szMsg, ...);

/// \brief Tests two strings for equality. On failure both actual and expected values are output.
#define WD_TEST_STRING(i1, i2) WD_TEST_STRING_MSG(i1, i2, "")

/// \brief Tests two strings for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_STRING_MSG(s1, s2, msg, ...)                                                                                                   \
  wdTestString(static_cast<const char*>(s1), static_cast<const char*>(s2), WD_STRINGIZE(s1), WD_STRINGIZE(s2), WD_SOURCE_FILE, WD_SOURCE_LINE, \
    WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestWString(std::wstring s1, std::wstring s2, const char* szString1, const char* szString2, const char* szFile, wdInt32 iLine,
  const char* szFunction, const char* szMsg, ...);

/// \brief Tests two strings for equality. On failure both actual and expected values are output.
#define WD_TEST_WSTRING(i1, i2) WD_TEST_WSTRING_MSG(i1, i2, "")

/// \brief Tests two strings for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_WSTRING_MSG(s1, s2, msg, ...)                                                                                         \
  wdTestWString(static_cast<const wchar_t*>(s1), static_cast<const wchar_t*>(s2), WD_STRINGIZE(s1), WD_STRINGIZE(s2), WD_SOURCE_FILE, \
    WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/// \brief Tests two strings for equality. On failure both actual and expected values are output. Does not embed the original expression to
/// work around issues with the current code page and unicode literals.
#define WD_TEST_STRING_UNICODE(i1, i2) WD_TEST_STRING_UNICODE_MSG(i1, i2, "")

/// \brief Tests two strings for equality. On failure both actual and expected values are output, also a custom message is printed. Does not
/// embed the original expression to work around issues with the current code page and unicode literals.
#define WD_TEST_STRING_UNICODE_MSG(s1, s2, msg, ...) \
  wdTestString(                                      \
    static_cast<const char*>(s1), static_cast<const char*>(s2), "", "", WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestVector(
  wdVec4d v1, wdVec4d v2, double fEps, const char* szCondition, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Tests two wdVec2's for equality, using some epsilon. On failure both actual and expected values are output.
#define WD_TEST_VEC2(i1, i2, epsilon) WD_TEST_VEC2_MSG(i1, i2, epsilon, "")

/// \brief Tests two wdVec2's for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_VEC2_MSG(r1, r2, epsilon, msg, ...)                                                                                \
  wdTestVector(wdVec4d(ToFloat((r1).x), ToFloat((r1).y), 0, 0), wdVec4d(ToFloat((r2).x), ToFloat((r2).y), 0, 0), ToFloat(epsilon), \
    WD_STRINGIZE(r1) " == " WD_STRINGIZE(r2), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/// \brief Tests two wdVec3's for equality, using some epsilon. On failure both actual and expected values are output.
#define WD_TEST_VEC3(i1, i2, epsilon) WD_TEST_VEC3_MSG(i1, i2, epsilon, "")

/// \brief Tests two wdVec3's for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_VEC3_MSG(r1, r2, epsilon, msg, ...)                                                                                          \
  wdTestVector(wdVec4d(ToFloat((r1).x), ToFloat((r1).y), ToFloat((r1).z), 0), wdVec4d(ToFloat((r2).x), ToFloat((r2).y), ToFloat((r2).z), 0), \
    ToFloat(epsilon), WD_STRINGIZE(r1) " == " WD_STRINGIZE(r2), WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

/// \brief Tests two wdVec4's for equality, using some epsilon. On failure both actual and expected values are output.
#define WD_TEST_VEC4(i1, i2, epsilon) WD_TEST_VEC4_MSG(i1, i2, epsilon, "")

/// \brief Tests two wdVec4's for equality. On failure both actual and expected values are output, also a custom message is printed.
#define WD_TEST_VEC4_MSG(r1, r2, epsilon, msg, ...)                                                                                          \
  wdTestVector(wdVec4d(ToFloat((r1).x), ToFloat((r1).y), ToFloat((r1).z), ToFloat((r1).w)),                                                  \
    wdVec4d(ToFloat((r2).x), ToFloat((r2).y), ToFloat((r2).z), ToFloat((r2).w)), ToFloat(epsilon), WD_STRINGIZE(r1) " == " WD_STRINGIZE(r2), \
    WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestFiles(
  const char* szFile1, const char* szFile2, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

#define WD_TEST_FILES(szFile1, szFile2, msg, ...) \
  wdTestFiles(szFile1, szFile2, WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

WD_TEST_DLL bool wdTestTextFiles(
  const char* szFile1, const char* szFile2, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

#define WD_TEST_TEXT_FILES(szFile1, szFile2, msg, ...) \
  wdTestTextFiles(szFile1, szFile2, WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

WD_TEST_DLL bool wdTestImage(
  wdUInt32 uiImageNumber, wdUInt32 uiMaxError, bool bIsDepthImage, const char* szFile, wdInt32 iLine, const char* szFunction, const char* szMsg, ...);

/// \brief Same as WD_TEST_IMAGE_MSG but uses an empty error message.
#define WD_TEST_IMAGE(ImageNumber, MaxError) WD_TEST_IMAGE_MSG(ImageNumber, MaxError, "")

/// \brief Same as WD_TEST_DEPTH_IMAGE_MSG but uses an empty error message.
#define WD_TEST_DEPTH_IMAGE(ImageNumber, MaxError) WD_TEST_DEPTH_IMAGE_MSG(ImageNumber, MaxError, "")

/// \brief Executes an image comparison right now.
///
/// The reference image is read from disk.
/// The path to the reference image is constructed from the test and sub-test name and the 'ImageNumber'.
/// One can, for instance, use the 'invocation count' that is passed to wdTestBaseClass::RunSubTest() as the ImageNumber,
/// but any other integer is fine as well.
///
/// The current image to compare is taken from wdTestBaseClass::GetImage().
/// Rendering tests typically override this function to return the result of the currently rendered frame.
///
/// 'MaxError' specifies the maximum mean-square error that is still considered acceptable
/// between the reference image and the current image.
///
/// Use the * DEPTH * variant if a depth buffer comparison should be requested.
///
/// \note Some tests need to know at the start, whether an image comparison will be done at the end, so they
/// can capture the image first. For such use cases, use WD_SCHEDULE_IMAGE_TEST at the start of a sub-test instead.
#define WD_TEST_IMAGE_MSG(ImageNumber, MaxError, msg, ...) \
  wdTestImage(ImageNumber, MaxError, false, WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

#define WD_TEST_DEPTH_IMAGE_MSG(ImageNumber, MaxError, msg, ...) \
  wdTestImage(ImageNumber, MaxError, true, WD_SOURCE_FILE, WD_SOURCE_LINE, WD_SOURCE_FUNCTION, msg, ##__VA_ARGS__)

/// \brief Schedules an WD_TEST_IMAGE to be executed after the current sub-test execution finishes.
///
/// Call this at the beginning of a sub-test, to automatically execute an image comparison when it is finished.
/// Calling wdTestFramework::IsImageComparisonScheduled() will now return true.
///
/// To support image comparisons, tests derived from wdTestBaseClass need to provide the current image through wdTestBaseClass::GetImage().
/// To support 'scheduled' image comparisons, the class should poll wdTestFramework::IsImageComparisonScheduled() every step and capture the
/// image when needed.
///
/// Use the * DEPTH * variant if a depth buffer comparison is intended.
///
/// \note Scheduling image comparisons is an optimization to only capture data when necessary, instead of capturing it every single frame.
#define WD_SCHEDULE_IMAGE_TEST(ImageNumber, MaxError) wdTestFramework::GetInstance()->ScheduleImageComparison(ImageNumber, MaxError);

#define WD_SCHEDULE_DEPTH_IMAGE_TEST(ImageNumber, MaxError) wdTestFramework::GetInstance()->ScheduleDepthImageComparison(ImageNumber, MaxError);
