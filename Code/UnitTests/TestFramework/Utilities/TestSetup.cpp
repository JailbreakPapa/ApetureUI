#include <TestFramework/TestFrameworkPCH.h>

#include <TestFramework/Utilities/TestSetup.h>

#include <TestFramework/Utilities/ConsoleOutput.h>
#include <TestFramework/Utilities/HTMLOutput.h>

#include <Foundation/System/CrashHandler.h>
#include <Foundation/System/SystemInformation.h>

#ifdef WD_USE_QT
#  include <TestFramework/Framework/Qt/qtTestFramework.h>
#  include <TestFramework/Framework/Qt/qtTestGUI.h>
#elif WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
#  include <TestFramework/Framework/Uwp/uwpTestFramework.h>
#endif

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
#  include <conio.h>
#endif

int wdTestSetup::s_iArgc = 0;
const char** wdTestSetup::s_pArgv = nullptr;

wdTestFramework* wdTestSetup::InitTestFramework(const char* szTestName, const char* szNiceTestName, int iArgc, const char** pArgv)
{
  s_iArgc = iArgc;
  s_pArgv = pArgv;

#if WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
  if (FAILED(RoInitialize(RO_INIT_MULTITHREADED)))
  {
    std::cout << "Failed to init WinRT." << std::endl;
  }
#endif

  // without a proper file system the current working directory is pretty much useless
  std::string sTestFolder = std::string(wdOSFile::GetUserDataFolder());
  if (*sTestFolder.rbegin() != '/')
    sTestFolder.append("/");
  sTestFolder.append("wdEngine Tests/");
  sTestFolder.append(szTestName);

  std::string sTestDataSubFolder = "Data/UnitTests/";
  sTestDataSubFolder.append(szTestName);

#ifdef WD_USE_QT
  wdTestFramework* pTestFramework = new wdQtTestFramework(szNiceTestName, sTestFolder.c_str(), sTestDataSubFolder.c_str(), iArgc, pArgv);
#elif WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
  // Command line args in UWP are handled differently and can't be retrieved from the main function.
  wdTestFramework* pTestFramework = new wdUwpTestFramework(szNiceTestName, sTestFolder.c_str(), sTestDataSubFolder.c_str(), 0, nullptr);
#else
  wdTestFramework* pTestFramework = new wdTestFramework(szNiceTestName, sTestFolder.c_str(), sTestDataSubFolder.c_str(), iArgc, pArgv);
#endif

  // Register some output handlers to forward all the messages to the console and to an HTML file
  pTestFramework->RegisterOutputHandler(OutputToConsole);
  pTestFramework->RegisterOutputHandler(wdOutputToHTML::OutputToHTML);

  wdCrashHandler_WriteMiniDump::g_Instance.SetDumpFilePath(pTestFramework->GetAbsOutputPath(), szTestName);
  wdCrashHandler::SetCrashHandler(&wdCrashHandler_WriteMiniDump::g_Instance);

  return pTestFramework;
}

wdTestAppRun wdTestSetup::RunTests()
{
  wdTestFramework* pTestFramework = wdTestFramework::GetInstance();

  // Todo: Incorporate all the below in a virtual call of testFramework?
#ifdef WD_USE_QT
  TestSettings settings = pTestFramework->GetSettings();
  if (settings.m_bNoGUI)
  {
    return pTestFramework->RunTestExecutionLoop();
  }

  // Setup Qt Application

  int argc = s_iArgc;
  char** argv = const_cast<char**>(s_pArgv);

  if (qApp != nullptr)
  {
    bool ok = false;
    int iCount = qApp->property("Shared").toInt(&ok);
    WD_ASSERT_DEV(ok, "Existing QApplication was not constructed by WD!");
    qApp->setProperty("Shared", QVariant::fromValue(iCount + 1));
  }
  else
  {
    new QApplication(argc, argv);
    qApp->setProperty("Shared", QVariant::fromValue((int)1));
    qApp->setApplicationName(pTestFramework->GetTestName());
    wdQtTestGUI::SetDarkTheme();
  }

  // Create main window
  {
    wdQtTestGUI mainWindow(*static_cast<wdQtTestFramework*>(pTestFramework));
    mainWindow.show();

    qApp->exec();
  }
  {
    const int iCount = qApp->property("Shared").toInt();
    if (iCount == 1)
    {
      delete qApp;
    }
    else
    {
      qApp->setProperty("Shared", QVariant::fromValue(iCount - 1));
    }
  }

  return wdTestAppRun::Quit;
#elif WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
  static_cast<wdUwpTestFramework*>(pTestFramework)->Run();
  return wdTestAppRun::Quit;
#else
  // Run all the tests with the given order
  return pTestFramework->RunTestExecutionLoop();
#endif
}

void wdTestSetup::DeInitTestFramework(bool bSilent /*= false*/)
{
  wdTestFramework* pTestFramework = wdTestFramework::GetInstance();

  wdStartup::ShutdownCoreSystems();

  // In the UWP case we never initialized this thread for wd, so we can't do log output now.
#if WD_DISABLED(WD_PLATFORM_WINDOWS_UWP)
  if (!bSilent)
  {
    wdGlobalLog::AddLogWriter(wdLogWriter::Console::LogMessageHandler);
    wdStringUtils::PrintStringLengthStatistics();
  }
#endif

  TestSettings settings = pTestFramework->GetSettings();
  if (settings.m_bKeepConsoleOpen && !bSilent)
  {
    if (wdSystemInformation::IsDebuggerAttached())
    {
      std::cout << "Press the any key to continue...\n";
      fflush(stdin);
      [[maybe_unused]] int c = getchar();
    }
  }

  // This is needed as at least windows can't be bothered to write anything
  // to the output streams at all if it's not enough or the app is too fast.
  fflush(stdout);
  fflush(stderr);
  delete pTestFramework;
}

wdInt32 wdTestSetup::GetFailedTestCount()
{
  return wdTestFramework::GetInstance()->GetTestsFailedCount();
}


WD_STATICLINK_FILE(TestFramework, TestFramework_Utilities_TestSetup);
