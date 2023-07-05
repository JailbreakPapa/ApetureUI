#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/HTMLWriter.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/Threading/Thread.h>
#include <TestFramework/Utilities/TestLogInterface.h>

WD_CREATE_SIMPLE_TEST_GROUP(Logging);

namespace
{

  class LogTestLogInterface : public wdLogInterface
  {
  public:
    virtual void HandleLogMessage(const wdLoggingEventData& le) override
    {
      switch (le.m_EventType)
      {
        case wdLogMsgType::Flush:
          m_Result.Append("[Flush]\n");
          return;
        case wdLogMsgType::BeginGroup:
          m_Result.Append(">", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::EndGroup:
          m_Result.Append("<", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::ErrorMsg:
          m_Result.Append("E:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::SeriousWarningMsg:
          m_Result.Append("SW:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::WarningMsg:
          m_Result.Append("W:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::SuccessMsg:
          m_Result.Append("S:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::InfoMsg:
          m_Result.Append("I:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::DevMsg:
          m_Result.Append("E:", le.m_sTag, " ", le.m_sText, "\n");
          break;
        case wdLogMsgType::DebugMsg:
          m_Result.Append("D:", le.m_sTag, " ", le.m_sText, "\n");
          break;

        default:
          WD_REPORT_FAILURE("Invalid msg type");
          break;
      }
    }

    wdStringBuilder m_Result;
  };

} // namespace

WD_CREATE_SIMPLE_TEST(Logging, Log)
{
  LogTestLogInterface log;
  LogTestLogInterface log2;
  wdLogSystemScope logScope(&log);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Output")
  {
    WD_LOG_BLOCK("Verse 1", "Portal: Still Alive");

    wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::All);

    wdLog::Success("{0}", "This was a triumph.");
    wdLog::Info("{0}", "I'm making a note here:");
    wdLog::Error("{0}", "Huge Success");
    wdLog::Info("{0}", "It's hard to overstate my satisfaction.");
    wdLog::Dev("{0}", "Aperture Science. We do what we must, because we can,");
    wdLog::Dev("{0}", "For the good of all of us, except the ones who are dead.");
    wdLog::Flush();
    wdLog::Flush(); // second flush should be ignored

    {
      WD_LOG_BLOCK("Verse 2");

      wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::DevMsg);

      wdLog::Dev("But there's no sense crying over every mistake.");
      wdLog::Debug("You just keep on trying 'till you run out of cake.");
      wdLog::Info("And the science gets done, and you make a neat gun");
      wdLog::Error("for the people who are still alive.");
    }

    {
      WD_LOG_BLOCK("Verse 3");

      wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::InfoMsg);

      wdLog::Info("I'm not even angry.");
      wdLog::Debug("I'm being so sincere right now.");
      wdLog::Dev("Even though you broke my heart and killed me.");
      wdLog::Info("And tore me to pieces,");
      wdLog::Dev("and threw every piece into a fire.");
      wdLog::Info("As they burned it hurt because I was so happy for you.");
      wdLog::Error("Now these points of data make a beautiful line");
      wdLog::Dev("and we're off the beta, we're releasing on time.");
      wdLog::Flush();
      wdLog::Flush();

      {
        WD_LOG_BLOCK("Verse 4");

        wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::SuccessMsg);

        wdLog::Info("So I'm glad I got burned,");
        wdLog::Debug("think of all the things we learned");
        wdLog::Debug("for the people who are still alive.");

        {
          wdLogSystemScope logScope2(&log2);
          WD_LOG_BLOCK("Interlude");
          wdLog::Info("Well here we are again. It's always such a pleasure.");
          wdLog::Error("Remember when you tried to kill me twice?");
        }

        {
          WD_LOG_BLOCK("Verse 5");

          wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::WarningMsg);

          wdLog::Debug("Go ahead and leave me.");
          wdLog::Info("I think I prefer to stay inside.");
          wdLog::Dev("Maybe you'll find someone else, to help you.");
          wdLog::Dev("Maybe Black Mesa.");
          wdLog::Info("That was a joke. Haha. Fat chance.");
          wdLog::Warning("Anyway, this cake is great.");
          wdLog::Success("It's so delicious and moist.");
          wdLog::Dev("Look at me still talking when there's science to do.");
          wdLog::Error("When I look up there it makes me glad I'm not you.");
          wdLog::Info("I've experiments to run,");
          wdLog::SeriousWarning("there is research to be done on the people who are still alive.");
        }
      }
    }
  }

  {
    WD_LOG_BLOCK("Verse 6", "Last One");

    wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::ErrorMsg);

    wdLog::Dev("And believe me I am still alive.");
    wdLog::Info("I'm doing science and I'm still alive.");
    wdLog::Success("I feel fantastic and I'm still alive.");
    wdLog::Warning("While you're dying I'll be still alive.");
    wdLog::Error("And when you're dead I will be, still alive.");
    wdLog::Debug("Still alive, still alive.");
  }

  /// \todo This test will fail if WD_COMPILE_FOR_DEVELOPMENT is disabled.
  /// We also currently don't test wdLog::Debug, because our build machines compile in release and then the text below would need to be
  /// different.

  const char* szResult = log.m_Result;
  const char* szExpected = "\
>Portal: Still Alive Verse 1\n\
S: This was a triumph.\n\
I: I'm making a note here:\n\
E: Huge Success\n\
I: It's hard to overstate my satisfaction.\n\
E: Aperture Science. We do what we must, because we can,\n\
E: For the good of all of us, except the ones who are dead.\n\
[Flush]\n\
> Verse 2\n\
E: But there's no sense crying over every mistake.\n\
I: And the science gets done, and you make a neat gun\n\
E: for the people who are still alive.\n\
< Verse 2\n\
> Verse 3\n\
I: I'm not even angry.\n\
I: And tore me to pieces,\n\
I: As they burned it hurt because I was so happy for you.\n\
E: Now these points of data make a beautiful line\n\
[Flush]\n\
> Verse 4\n\
> Verse 5\n\
W: Anyway, this cake is great.\n\
E: When I look up there it makes me glad I'm not you.\n\
SW: there is research to be done on the people who are still alive.\n\
< Verse 5\n\
< Verse 4\n\
< Verse 3\n\
<Portal: Still Alive Verse 1\n\
>Last One Verse 6\n\
E: And when you're dead I will be, still alive.\n\
<Last One Verse 6\n\
";

  WD_TEST_STRING(szResult, szExpected);

  const char* szResult2 = log2.m_Result;
  const char* szExpected2 = "\
> Interlude\n\
I: Well here we are again. It's always such a pleasure.\n\
E: Remember when you tried to kill me twice?\n\
< Interlude\n\
";

  WD_TEST_STRING(szResult2, szExpected2);
}

WD_CREATE_SIMPLE_TEST(Logging, GlobalTestLog)
{
  wdLog::GetThreadLocalLogSystem()->SetLogLevel(wdLogMsgType::All);

  {
    wdTestLogInterface log;
    wdTestLogSystemScope scope(&log, true);

    log.ExpectMessage("managed to break", wdLogMsgType::ErrorMsg);
    log.ExpectMessage("my heart", wdLogMsgType::WarningMsg);
    log.ExpectMessage("see you", wdLogMsgType::WarningMsg, 10);

    {
      class LogThread : public wdThread
      {
      public:
        virtual wdUInt32 Run() override
        {
          wdLog::Warning("I see you!");
          wdLog::Debug("Test debug");
          return 0;
        }
      };

      LogThread thread[10];

      for (wdUInt32 i = 0; i < 10; ++i)
      {
        thread[i].Start();
      }

      wdLog::Error("The only thing you managed to break so far");
      wdLog::Warning("is my heart");

      for (wdUInt32 i = 0; i < 10; ++i)
      {
        thread[i].Join();
      }
    }
  }
}
