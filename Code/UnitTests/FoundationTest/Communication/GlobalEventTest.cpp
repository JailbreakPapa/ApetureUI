#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Communication/GlobalEvent.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/Log.h>

static wdInt32 iTestData1 = 0;
static wdInt32 iTestData2 = 0;

// The following event handlers are automatically registered, nothing else needs to be done here

WD_ON_GLOBAL_EVENT(TestGlobalEvent1)
{
  iTestData1 += param0.Get<wdInt32>();
}

WD_ON_GLOBAL_EVENT(TestGlobalEvent2)
{
  iTestData2 += param0.Get<wdInt32>();
}

WD_ON_GLOBAL_EVENT_ONCE(TestGlobalEvent3)
{
  // this handler will be executed only once, even if the event is broadcast multiple times
  iTestData2 += 42;
}

static bool g_bFirstRun = true;

WD_CREATE_SIMPLE_TEST(Communication, GlobalEvent)
{
  iTestData1 = 0;
  iTestData2 = 0;

  WD_TEST_INT(iTestData1, 0);
  WD_TEST_INT(iTestData2, 0);

  wdGlobalEvent::Broadcast("TestGlobalEvent1", 1);

  WD_TEST_INT(iTestData1, 1);
  WD_TEST_INT(iTestData2, 0);

  wdGlobalEvent::Broadcast("TestGlobalEvent1", 2);

  WD_TEST_INT(iTestData1, 3);
  WD_TEST_INT(iTestData2, 0);

  wdGlobalEvent::Broadcast("TestGlobalEvent1", 3);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 0);

  wdGlobalEvent::Broadcast("TestGlobalEvent2", 4);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 4);

  wdGlobalEvent::Broadcast("TestGlobalEvent3", 4);

  WD_TEST_INT(iTestData1, 6);

  if (g_bFirstRun)
  {
    g_bFirstRun = false;
    WD_TEST_INT(iTestData2, 46);
  }
  else
  {
    WD_TEST_INT(iTestData2, 4);
    iTestData2 += 42;
  }

  wdGlobalEvent::Broadcast("TestGlobalEvent2", 5);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 51);

  wdGlobalEvent::Broadcast("TestGlobalEvent3", 4);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 51);

  wdGlobalEvent::Broadcast("TestGlobalEvent2", 6);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 57);

  wdGlobalEvent::Broadcast("TestGlobalEvent3", 4);

  WD_TEST_INT(iTestData1, 6);
  WD_TEST_INT(iTestData2, 57);

  wdGlobalLog::AddLogWriter(wdLogWriter::Console::LogMessageHandler);

  wdGlobalEvent::PrintGlobalEventStatistics();

  wdGlobalLog::RemoveLogWriter(wdLogWriter::Console::LogMessageHandler);
}
