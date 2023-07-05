#include <FoundationTest/FoundationTestPCH.h>

#if WD_ENABLED(WD_SUPPORTS_PROCESSES)

#  include <Foundation/System/ProcessGroup.h>
#  include <Foundation/Utilities/CommandLineUtils.h>

WD_CREATE_SIMPLE_TEST(System, ProcessGroup)
{
  // we can launch FoundationTest with the -cmd parameter to execute a couple of useful things to test launching process
  const wdStringBuilder pathToSelf = wdCommandLineUtils::GetGlobalInstance()->GetParameter(0);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "WaitToFinish")
  {
    wdProcessGroup pgroup;
    wdStringBuilder out;

    wdMutex mutex;

    for (wdUInt32 i = 0; i < 8; ++i)
    {
      wdProcessOptions opt;
      opt.m_sProcess = pathToSelf;
      opt.m_onStdOut = [&out, &mutex](wdStringView sView) {
        WD_LOCK(mutex);
        out.Append(sView);
      };

      opt.m_Arguments.PushBack("-cmd");
      opt.m_Arguments.PushBack("-sleep");
      opt.m_Arguments.PushBack("1000");
      opt.m_Arguments.PushBack("-stdout");
      opt.m_Arguments.PushBack("Na");

      WD_TEST_BOOL(pgroup.Launch(opt).Succeeded());
    }

    // in a debugger with child debugging enabled etc. even 10 seconds can lead to timeouts due to long delays in the IDE
    WD_TEST_BOOL(pgroup.WaitToFinish(wdTime::Seconds(60)).Succeeded());
    WD_TEST_STRING(out, "NaNaNaNaNaNaNaNa"); // BATMAN!
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "TerminateAll")
  {
    wdProcessGroup pgroup;

    wdHybridArray<wdProcess, 8> procs;

    for (wdUInt32 i = 0; i < 8; ++i)
    {
      wdProcessOptions opt;
      opt.m_sProcess = pathToSelf;

      opt.m_Arguments.PushBack("-cmd");
      opt.m_Arguments.PushBack("-sleep");
      opt.m_Arguments.PushBack("60000");

      WD_TEST_BOOL(pgroup.Launch(opt).Succeeded());
    }

    const wdTime tStart = wdTime::Now();
    WD_TEST_BOOL(pgroup.TerminateAll().Succeeded());
    const wdTime tDiff = wdTime::Now() - tStart;

    WD_TEST_BOOL(tDiff < wdTime::Seconds(10));
  }
}
#endif
