#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/CodeUtils/Preprocessor.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>

WD_CREATE_SIMPLE_TEST_GROUP(CodeUtils);

wdResult FileLocator(const char* szCurAbsoluteFile, const char* szIncludeFile, wdPreprocessor::IncludeType incType, wdStringBuilder& out_sAbsoluteFilePath)
{
  wdStringBuilder& s = out_sAbsoluteFilePath;

  if (incType == wdPreprocessor::RelativeInclude)
  {
    s = szCurAbsoluteFile;
    s.PathParentDirectory();
    s.AppendPath(szIncludeFile);
    s.MakeCleanPath();
  }
  else if (incType == wdPreprocessor::GlobalInclude)
  {
    s = "Preprocessor";
    s.AppendPath(szIncludeFile);
    s.MakeCleanPath();
  }
  else
    s = szIncludeFile;

  return WD_SUCCESS;
}

class Logger : public wdLogInterface
{
public:
  virtual void HandleLogMessage(const wdLoggingEventData& le) override { m_sOutput.AppendFormat("Log: '{0}'\r\n", le.m_sText); }

  void EventHandler(const wdPreprocessor::ProcessingEvent& ed)
  {
    switch (ed.m_Type)
    {
      case wdPreprocessor::ProcessingEvent::Error:
      case wdPreprocessor::ProcessingEvent::Warning:
        m_EventStack.PushBack(ed);
        break;
      case wdPreprocessor::ProcessingEvent::BeginExpansion:
        m_EventStack.PushBack(ed);
        return;
      case wdPreprocessor::ProcessingEvent::EndExpansion:
        m_EventStack.PopBack();
        return;
      default:
        return;
    }

    for (wdUInt32 i = 0; i < m_EventStack.GetCount(); ++i)
    {
      const wdPreprocessor::ProcessingEvent& event = m_EventStack[i];

      if (event.m_pToken != nullptr)
        m_sOutput.AppendFormat("{0}: Line {1} [{2}]: ", event.m_pToken->m_File.GetString(), event.m_pToken->m_uiLine, event.m_pToken->m_uiColumn);

      switch (event.m_Type)
      {
        case wdPreprocessor::ProcessingEvent::Error:
          m_sOutput.Append("Error: ");
          break;
        case wdPreprocessor::ProcessingEvent::Warning:
          m_sOutput.Append("Warning: ");
          break;
        case wdPreprocessor::ProcessingEvent::BeginExpansion:
          m_sOutput.AppendFormat("In Macro: '{0}'", wdString(event.m_pToken->m_DataView));
          break;
        case wdPreprocessor::ProcessingEvent::EndExpansion:
          break;

        default:
          break;
      }

      m_sOutput.AppendFormat("{0}\r\n", event.m_szInfo);
    }

    m_EventStack.PopBack();
  }

  wdDeque<wdPreprocessor::ProcessingEvent> m_EventStack;
  wdStringBuilder m_sOutput;
};

WD_CREATE_SIMPLE_TEST(CodeUtils, Preprocessor)
{
  wdStringBuilder sReadDir(">sdk/", wdTestFramework::GetInstance()->GetRelTestDataPath());
  wdStringBuilder sWriteDir = wdTestFramework::GetInstance()->GetAbsOutputPath();

  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sReadDir, "PreprocessorTest") == WD_SUCCESS);
  WD_TEST_BOOL_MSG(wdFileSystem::AddDataDirectory(sWriteDir, "PreprocessorTest", "output", wdFileSystem::AllowWrites) == WD_SUCCESS, "Failed to mount data dir '%s'", sWriteDir.GetData());

  wdTokenizedFileCache SharedCache;

  /// \todo Add tests for the following:
  /*
    macro expansion
    macro expansion in #if
    custom defines from outside
    stringify invalid token
    concatenate invalid tokens, tokens that yield valid macro
    broken function macros (missing parenthesis etc.)
    errors after #line directive
    errors in
      #line directive
      #define
      #ifdef
      etc.

    Done:
    #pragma once
    #include "" and <>
    #define defined / __LINE__ / __FILE__
    #line, __LINE__, __FILE__
    stringification with comments, newlines and spaces
    concatenation (maybe more?)
    bad #include
    comments
    newlines in some weird places
    too few, too many parameters
    __VA_ARGS__
    expansion that needs several iterations
    stringification of strings and special characters (\n)
    commas in macro parameters
    #undef
    unlocateable include file
    pass through #pragma
    pass through #line
    invalid #if, #else, #elif, #endif nesting
    #ifdef, #if etc.
    mathematical expressions
    boolean expressions
    bitwise expressions
    expand to self (with, without parameters, with parameters to expand)
    incorrect expressions
    Strings with line breaks in them
    Redefining without undefining a macro
    */

  {
    struct PPTestSettings
    {
      PPTestSettings(const char* szFileName, bool bPassThroughLines = false, bool bPassThroughPragmas = false, bool bPassThroughUnknownCommands = false)
        : m_szFileName(szFileName)
        , m_bPassThroughLines(bPassThroughLines)
        , m_bPassThroughPragmas(bPassThroughPragmas)
        , m_bPassThroughUnknownCommands(bPassThroughUnknownCommands)
      {
      }

      const char* m_szFileName;
      bool m_bPassThroughLines;
      bool m_bPassThroughPragmas;
      bool m_bPassThroughUnknownCommands;
    };

    PPTestSettings TestSettings[] = {
      PPTestSettings("IncludeViaMacro"),
      PPTestSettings("PragmaOnce"),
      PPTestSettings("LinePragmaPassThrough", true, true),
      PPTestSettings("Undef"),
      PPTestSettings("InvalidIf1"),
      PPTestSettings("Parameters"),
      PPTestSettings("LineControl"),
      PPTestSettings("LineControl2"),
      PPTestSettings("DefineFile"),
      PPTestSettings("DefineLine"),
      PPTestSettings("DefineDefined"),
      PPTestSettings("Stringify"),
      PPTestSettings("BuildFlags"),
      PPTestSettings("Empty"),
      PPTestSettings("Test1"),
      PPTestSettings("FailedInclude"), /// \todo Better error message
      PPTestSettings("PassThroughUnknown", false, false, true),
      PPTestSettings("IncorrectNesting1"),
      PPTestSettings("IncorrectNesting2"),
      PPTestSettings("IncorrectNesting3"),
      PPTestSettings("IncorrectNesting4"),
      PPTestSettings("IncorrectNesting5"),
      PPTestSettings("IncorrectNesting6"),
      PPTestSettings("IncorrectNesting7"),
      PPTestSettings("Error"),
      PPTestSettings("Warning"),
      PPTestSettings("Expressions"),
      PPTestSettings("ExpressionsBit"),
      PPTestSettings("ExpandSelf"),
      PPTestSettings("InvalidLogic1"),
      PPTestSettings("InvalidLogic2"),
      PPTestSettings("InvalidLogic3"),
      PPTestSettings("InvalidLogic4"),
      PPTestSettings("InvalidExpandSelf1"),
      PPTestSettings("InvalidExpandSelf2"),
      PPTestSettings("ErrorNoQuotes"),
      PPTestSettings("ErrorBadQuotes"),
      PPTestSettings("ErrorBadQuotes2"),
      PPTestSettings("ErrorLineBreaks"),
      PPTestSettings("Redefine"),
      PPTestSettings("ErrorBadBrackets"),
      PPTestSettings("IfTrueFalse"),
    };

    wdStringBuilder sOutput;
    wdStringBuilder fileName;
    wdStringBuilder fileNameOut;
    wdStringBuilder fileNameExp;

    for (int i = 0; i < WD_ARRAY_SIZE(TestSettings); i++)
    {
      WD_TEST_BLOCK(wdTestBlock::Enabled, TestSettings[i].m_szFileName)
      {
        Logger log;

        wdPreprocessor pp;
        pp.SetLogInterface(&log);
        pp.SetPassThroughLine(TestSettings[i].m_bPassThroughLines);
        pp.SetPassThroughPragma(TestSettings[i].m_bPassThroughPragmas);
        pp.SetFileLocatorFunction(FileLocator);
        pp.SetCustomFileCache(&SharedCache);
        pp.m_ProcessingEvents.AddEventHandler(wdDelegate<void(const wdPreprocessor::ProcessingEvent&)>(&Logger::EventHandler, &log));
        pp.AddCustomDefine("PP_OBJ").IgnoreResult();
        pp.AddCustomDefine("PP_FUNC(a) a").IgnoreResult();
        pp.SetPassThroughUnknownCmdsCB([](const char* s) -> bool { return wdStringUtils::IsEqual(s, "version"); }); // TestSettings[i].m_bPassThroughUnknownCommands);

        {
          fileName.Format("Preprocessor/{0}.txt", TestSettings[i].m_szFileName);
          fileNameExp.Format("Preprocessor/{0} - Expected.txt", TestSettings[i].m_szFileName);
          fileNameOut.Format(":output/Preprocessor/{0} - Result.txt", TestSettings[i].m_szFileName);

          WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileName), "File does not exist: '%s'", fileName.GetData());

          wdFileWriter fout;
          WD_VERIFY(fout.Open(fileNameOut).Succeeded(), "Could not create output file '{0}'", fileNameOut);

          if (pp.Process(fileName, sOutput) == WD_SUCCESS)
          {
            wdString sError = "Processing succeeded\r\n";
            fout.WriteBytes(sError.GetData(), sError.GetElementCount()).IgnoreResult();
            fout.WriteBytes(sOutput.GetData(), sOutput.GetElementCount()).IgnoreResult();

            if (!log.m_sOutput.IsEmpty())
              fout.WriteBytes("\r\n", 2).IgnoreResult();
          }
          else
          {
            wdString sError = "Processing failed\r\n";
            fout.WriteBytes(sError.GetData(), sError.GetElementCount()).IgnoreResult();
          }

          fout.WriteBytes(log.m_sOutput.GetData(), log.m_sOutput.GetElementCount()).IgnoreResult();

          WD_TEST_BOOL_MSG(wdFileSystem::ExistsFile(fileNameOut), "Output file is missing: '%s'", fileNameOut.GetData());
        }

        WD_TEST_TEXT_FILES(fileNameOut.GetData(), fileNameExp.GetData(), "");
      }
    }
  }


  wdFileSystem::RemoveDataDirectoryGroup("PreprocessorTest");
}
