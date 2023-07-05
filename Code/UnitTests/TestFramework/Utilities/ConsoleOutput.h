#pragma once

#include <TestFramework/Framework/TestFramework.h>

#include <Foundation/Basics/Platform/Win/IncludeWindows.h>

#if WD_ENABLED(WD_PLATFORM_ANDROID)
#  include <android/log.h>
#endif
#if WD_ENABLED(WD_PLATFORM_WINDOWS)
#  include <Foundation/Logging/ETWWriter.h>
inline void SetConsoleColorInl(WORD ui)
{
#  if WD_DISABLED(WD_PLATFORM_WINDOWS_UWP)
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ui);
#  endif
}
#else
inline void SetConsoleColorInl(wdUInt8 ui) {}
#endif

inline void OutputToConsole(wdTestOutput::Enum type, const char* szMsg)
{
  static wdInt32 iIndentation = 0;
  static bool bAnyError = false;

  switch (type)
  {
    case wdTestOutput::StartOutput:
      break;
    case wdTestOutput::BeginBlock:
      iIndentation += 2;
      break;
    case wdTestOutput::EndBlock:
      iIndentation -= 2;
      break;
    case wdTestOutput::Details:
      SetConsoleColorInl(0x07);
      break;
    case wdTestOutput::ImportantInfo:
      SetConsoleColorInl(0x07);
      break;
    case wdTestOutput::Success:
      SetConsoleColorInl(0x0A);
      break;
    case wdTestOutput::Message:
      SetConsoleColorInl(0x0E);
      break;
    case wdTestOutput::Warning:
      SetConsoleColorInl(0x0C);
      break;
    case wdTestOutput::Error:
      SetConsoleColorInl(0x0C);
      bAnyError = true;
      break;
    case wdTestOutput::Duration:
    case wdTestOutput::ImageDiffFile:
    case wdTestOutput::InvalidType:
    case wdTestOutput::AllOutputTypes:
      return;

    case wdTestOutput::FinalResult:
      if (bAnyError)
        SetConsoleColorInl(0x0C);
      else
        SetConsoleColorInl(0x0A);

      // reset it for the next test round
      bAnyError = false;
      break;

    default:
      WD_ASSERT_NOT_IMPLEMENTED;
      break;
  }

  printf("%*s%s\n", iIndentation, "", szMsg);
  SetConsoleColorInl(0x07);

#if WD_ENABLED(WD_PLATFORM_WINDOWS_UWP)
  wdLogMsgType::Enum logType = wdLogMsgType::None;
  switch (Type)
  {
    case wdTestOutput::StartOutput:
    case wdTestOutput::InvalidType:
    case wdTestOutput::AllOutputTypes:
      logType = wdLogMsgType::None;
      break;
    case wdTestOutput::BeginBlock:
      logType = wdLogMsgType::BeginGroup;
      break;
    case wdTestOutput::EndBlock:
      logType = wdLogMsgType::EndGroup;
      break;
    case wdTestOutput::ImportantInfo:
    case wdTestOutput::Details:
    case wdTestOutput::Message:
    case wdTestOutput::Duration:
    case wdTestOutput::FinalResult:
      logType = wdLogMsgType::InfoMsg;
      break;
    case wdTestOutput::Success:
      logType = wdLogMsgType::SuccessMsg;
      break;
    case wdTestOutput::Warning:
      logType = wdLogMsgType::WarningMsg;
      break;
    case wdTestOutput::Error:
      logType = wdLogMsgType::ErrorMsg;
      break;
    case wdTestOutput::ImageDiffFile:
      logType = wdLogMsgType::DevMsg;
      break;
    default:
      break;
  }
  if (logType != wdLogMsgType::None)
  {
    wdLogWriter::ETW::LogMessage(wdLogMsgType::InfoMsg, iIndentation, szMsg);
  }
#endif
#if WD_ENABLED(WD_PLATFORM_WINDOWS)
  char sz[4096];
  wdStringUtils::snprintf(sz, 4096, "%*s%s\n", iIndentation, "", szMsg);
  OutputDebugStringW(wdStringWChar(sz).GetData());
#endif
#if WD_ENABLED(WD_PLATFORM_ANDROID)
  __android_log_print(ANDROID_LOG_DEBUG, "wdEngine", "%*s%s\n", iIndentation, "", szMsg);
#endif

  if (type >= wdTestOutput::Error)
  {
    fflush(stdout);
  }
}
