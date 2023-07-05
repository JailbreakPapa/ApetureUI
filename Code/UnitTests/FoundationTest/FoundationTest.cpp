#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Threading/ThreadUtils.h>
#include <Foundation/Utilities/CommandLineUtils.h>
#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/TestSetup.h>
#include <iostream>

wdInt32 wdConstructionCounter::s_iConstructions = 0;
wdInt32 wdConstructionCounter::s_iDestructions = 0;
wdInt32 wdConstructionCounter::s_iConstructionsLast = 0;
wdInt32 wdConstructionCounter::s_iDestructionsLast = 0;

wdInt32 wdConstructionCounterRelocatable::s_iConstructions = 0;
wdInt32 wdConstructionCounterRelocatable::s_iDestructions = 0;
wdInt32 wdConstructionCounterRelocatable::s_iConstructionsLast = 0;
wdInt32 wdConstructionCounterRelocatable::s_iDestructionsLast = 0;

WD_TESTFRAMEWORK_ENTRY_POINT_BEGIN("FoundationTest", "Foundation Tests")
{
  wdCommandLineUtils cmd;
  cmd.SetCommandLine(argc, (const char**)argv, wdCommandLineUtils::PreferOsArgs);

  // if the -cmd switch is set, FoundationTest.exe will execute a couple of simple operations and then close
  // this is used to test process launching (e.g. wdProcess)
  if (cmd.GetBoolOption("-cmd"))
  {
    // print something to stdout
    const char* szStdOut = cmd.GetStringOption("-stdout");
    if (!wdStringUtils::IsNullOrEmpty(szStdOut))
    {
      std::cout << szStdOut;
    }

    const char* szStdErr = cmd.GetStringOption("-stderr");
    if (!wdStringUtils::IsNullOrEmpty(szStdErr))
    {
      std::cerr << szStdErr;
    }

    // wait a little
    wdThreadUtils::Sleep(wdTime::Milliseconds(cmd.GetIntOption("-sleep")));

    // shutdown with exit code
    wdTestSetup::DeInitTestFramework(true);
    return cmd.GetIntOption("-exitcode");
  }
}
WD_TESTFRAMEWORK_ENTRY_POINT_END()
