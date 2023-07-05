#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/System/Process.h>
#include <Foundation/Utilities/CommandLineUtils.h>

WD_CREATE_SIMPLE_TEST_GROUP(System);

#if WD_ENABLED(WD_SUPPORTS_PROCESSES)

WD_CREATE_SIMPLE_TEST(System, Process)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Command Line")
  {
    wdProcessOptions proc;
    proc.m_Arguments.PushBack("-bla");
    proc.m_Arguments.PushBack("blub blub");
    proc.m_Arguments.PushBack("\"di dub\"");
    proc.AddArgument(" -test ");
    proc.AddArgument("-hmpf {}", 27);
    proc.AddCommandLine("-a b   -c  d  -e \"f g h\" ");

    wdStringBuilder cmdLine;
    proc.BuildCommandLineString(cmdLine);

    WD_TEST_STRING(cmdLine, "-bla \"blub blub\" \"di dub\" -test \"-hmpf 27\" -a b -c d -e \"f g h\"");
  }

  static const char* g_szTestMsg = "Tell me more!\nAnother line\n520CharactersInOneLineAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA_END\nThat's all";
  static const char* g_szTestMsgLine0 = "Tell me more!\n";
  static const char* g_szTestMsgLine1 = "Another line\n";
  static const char* g_szTestMsgLine2 = "520CharactersInOneLineAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA_END\n";
  static const char* g_szTestMsgLine3 = "That's all";


  // we can launch FoundationTest with the -cmd parameter to execute a couple of useful things to test launching process
  const wdStringBuilder pathToSelf = wdCommandLineUtils::GetGlobalInstance()->GetParameter(0);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Execute")
  {
    wdProcessOptions opt;
    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-sleep");
    opt.m_Arguments.PushBack("500");

    wdInt32 exitCode = -1;

    if (!WD_TEST_BOOL_MSG(wdProcess::Execute(opt, &exitCode).Succeeded(), "Failed to start process."))
      return;

    WD_TEST_INT(exitCode, 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Launch / WaitToFinish")
  {
    wdProcessOptions opt;
    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-sleep");
    opt.m_Arguments.PushBack("500");

    wdProcess proc;
    WD_TEST_BOOL(proc.GetState() == wdProcessState::NotStarted);

    if (!WD_TEST_BOOL_MSG(proc.Launch(opt).Succeeded(), "Failed to start process."))
      return;

    WD_TEST_BOOL(proc.GetState() == wdProcessState::Running);
    WD_TEST_BOOL(proc.WaitToFinish(wdTime::Seconds(5)).Succeeded());
    WD_TEST_BOOL(proc.GetState() == wdProcessState::Finished);
    WD_TEST_INT(proc.GetExitCode(), 0);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Launch / Terminate")
  {
    wdProcessOptions opt;
    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-sleep");
    opt.m_Arguments.PushBack("10000");
    opt.m_Arguments.PushBack("-exitcode");
    opt.m_Arguments.PushBack("0");

    wdProcess proc;
    WD_TEST_BOOL(proc.GetState() == wdProcessState::NotStarted);

    if (!WD_TEST_BOOL_MSG(proc.Launch(opt).Succeeded(), "Failed to start process."))
      return;

    WD_TEST_BOOL(proc.GetState() == wdProcessState::Running);
    WD_TEST_BOOL(proc.Terminate().Succeeded());
    WD_TEST_BOOL(proc.GetState() == wdProcessState::Finished);
    WD_TEST_INT(proc.GetExitCode(), -1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Launch / Detach")
  {
    wdTime tTerminate;

    {
      wdProcessOptions opt;
      opt.m_sProcess = pathToSelf;
      opt.m_Arguments.PushBack("-cmd");
      opt.m_Arguments.PushBack("-sleep");
      opt.m_Arguments.PushBack("10000");

      wdProcess proc;
      if (!WD_TEST_BOOL_MSG(proc.Launch(opt).Succeeded(), "Failed to start process."))
        return;

      proc.Detach();

      tTerminate = wdTime::Now();
    }

    const wdTime tDiff = wdTime::Now() - tTerminate;
    WD_TEST_BOOL_MSG(tDiff < wdTime::Seconds(1.0), "Destruction of wdProcess should be instant after Detach() was used.");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STDOUT")
  {
    wdDynamicArray<wdStringBuilder> lines;
    wdStringBuilder out;
    wdProcessOptions opt;
    opt.m_onStdOut = [&](wdStringView sView) {
      out.Append(sView);
      lines.PushBack(sView);
    };

    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-stdout");
    opt.m_Arguments.PushBack(g_szTestMsg);

    if (!WD_TEST_BOOL_MSG(wdProcess::Execute(opt).Succeeded(), "Failed to start process."))
      return;

    if (WD_TEST_BOOL(lines.GetCount() == 4))
    {
      lines[0].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[0], g_szTestMsgLine0);
      lines[1].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[1], g_szTestMsgLine1);
      lines[2].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[2], g_szTestMsgLine2);
      lines[3].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[3], g_szTestMsgLine3);
    }

    out.ReplaceAll("\r\n", "\n");
    WD_TEST_STRING(out, g_szTestMsg);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STDERROR")
  {
    wdStringBuilder err;
    wdProcessOptions opt;
    opt.m_onStdError = [&err](wdStringView sView) { err.Append(sView); };

    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-stderr");
    opt.m_Arguments.PushBack("NOT A VALID COMMAND");
    opt.m_Arguments.PushBack("-exitcode");
    opt.m_Arguments.PushBack("1");

    wdInt32 exitCode = 0;

    if (!WD_TEST_BOOL_MSG(wdProcess::Execute(opt, &exitCode).Succeeded(), "Failed to start process."))
      return;

    WD_TEST_BOOL_MSG(!err.IsEmpty(), "Error stream should contain something.");
    WD_TEST_INT(exitCode, 1);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "STDOUT_STDERROR")
  {
    wdDynamicArray<wdStringBuilder> lines;
    wdStringBuilder out;
    wdStringBuilder err;
    wdProcessOptions opt;
    opt.m_onStdOut = [&](wdStringView sView) {
      out.Append(sView);
      lines.PushBack(sView);
    };
    opt.m_onStdError = [&err](wdStringView sView) { err.Append(sView); };
    opt.m_sProcess = pathToSelf;
    opt.m_Arguments.PushBack("-cmd");
    opt.m_Arguments.PushBack("-stdout");
    opt.m_Arguments.PushBack(g_szTestMsg);

    if (!WD_TEST_BOOL_MSG(wdProcess::Execute(opt).Succeeded(), "Failed to start process."))
      return;

    if (WD_TEST_BOOL(lines.GetCount() == 4))
    {
      lines[0].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[0], g_szTestMsgLine0);
      lines[1].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[1], g_szTestMsgLine1);
      lines[2].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[2], g_szTestMsgLine2);
      lines[3].ReplaceAll("\r\n", "\n");
      WD_TEST_STRING(lines[3], g_szTestMsgLine3);
    }

    out.ReplaceAll("\r\n", "\n");
    WD_TEST_STRING(out, g_szTestMsg);
    WD_TEST_BOOL_MSG(err.IsEmpty(), "Error stream should be empty.");
  }
}
#endif
