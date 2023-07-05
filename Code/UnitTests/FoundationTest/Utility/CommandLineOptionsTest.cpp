#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Utilities/CommandLineOptions.h>

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

WD_CREATE_SIMPLE_TEST(Utility, CommandLineOptions)
{
  wdCommandLineOptionDoc optDoc("__test", "-argDoc", "<doc>", "Doc argument", "no value");

  wdCommandLineOptionBool optBool1("__test", "-bool1", "bool argument 1", false);
  wdCommandLineOptionBool optBool2("__test", "-bool2", "bool argument 2", true);

  wdCommandLineOptionInt optInt1("__test", "-int1", "int argument 1", 1);
  wdCommandLineOptionInt optInt2("__test", "-int2", "int argument 2", 0, 4, 8);
  wdCommandLineOptionInt optInt3("__test", "-int3", "int argument 3", 6, -8, 8);

  wdCommandLineOptionFloat optFloat1("__test", "-float1", "float argument 1", 1);
  wdCommandLineOptionFloat optFloat2("__test", "-float2", "float argument 2", 0, 4, 8);
  wdCommandLineOptionFloat optFloat3("__test", "-float3", "float argument 3", 6, -8, 8);

  wdCommandLineOptionString optString1("__test", "-string1", "string argument 1", "default string");

  wdCommandLineOptionPath optPath1("__test", "-path1", "path argument 1", "default path");

  wdCommandLineOptionEnum optEnum1("__test", "-enum1", "enum argument 1", "A | B = 2 | C | D | E = 7", 3);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionBool")
  {
    wdCommandLineUtils cmd;
    cmd.InjectCustomArgument("-bool1");
    cmd.InjectCustomArgument("on");

    WD_TEST_BOOL(optBool1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd) == true);
    WD_TEST_BOOL(optBool2.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd) == true);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionInt")
  {
    wdCommandLineUtils cmd;
    cmd.InjectCustomArgument("-int1");
    cmd.InjectCustomArgument("3");

    cmd.InjectCustomArgument("-int2");
    cmd.InjectCustomArgument("10");

    cmd.InjectCustomArgument("-int3");
    cmd.InjectCustomArgument("-2");

    WD_TEST_INT(optInt1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 3);
    WD_TEST_INT(optInt2.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 0);
    WD_TEST_INT(optInt3.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), -2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionFloat")
  {
    wdCommandLineUtils cmd;
    cmd.InjectCustomArgument("-float1");
    cmd.InjectCustomArgument("3");

    cmd.InjectCustomArgument("-float2");
    cmd.InjectCustomArgument("10");

    cmd.InjectCustomArgument("-float3");
    cmd.InjectCustomArgument("-2");

    WD_TEST_FLOAT(optFloat1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 3, 0.001f);
    WD_TEST_FLOAT(optFloat2.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 0, 0.001f);
    WD_TEST_FLOAT(optFloat3.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), -2, 0.001f);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionString")
  {
    wdCommandLineUtils cmd;
    cmd.InjectCustomArgument("-string1");
    cmd.InjectCustomArgument("hello");

    WD_TEST_STRING(optString1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), "hello");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionPath")
  {
    wdCommandLineUtils cmd;
    cmd.InjectCustomArgument("-path1");
    cmd.InjectCustomArgument("C:/test");

    const wdString path = optPath1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd);

#if WD_ENABLED(WD_PLATFORM_WINDOWS)
    WD_TEST_STRING(path, "C:/test");
#else
    WD_TEST_BOOL(path.EndsWith("C:/test"));
#endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "wdCommandLineOptionEnum")
  {
    {
      wdCommandLineUtils cmd;
      cmd.InjectCustomArgument("-enum1");
      cmd.InjectCustomArgument("A");

      WD_TEST_INT(optEnum1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 0);
    }

    {
      wdCommandLineUtils cmd;
      cmd.InjectCustomArgument("-enum1");
      cmd.InjectCustomArgument("B");

      WD_TEST_INT(optEnum1.GetOptionValue(wdCommandLineOption::LogMode::Never, &cmd), 2);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LogAvailableOptions")
  {
    wdCommandLineUtils cmd;

    wdStringBuilder result;

    WD_TEST_BOOL(wdCommandLineOption::LogAvailableOptionsToBuffer(result, wdCommandLineOption::LogAvailableModes::Always, "__test", &cmd));

    WD_TEST_STRING(result, "\
\n\
-argDoc <doc> = no value\n\
    Doc argument\n\
\n\
-bool1 <bool> = false\n\
    bool argument 1\n\
\n\
-bool2 <bool> = true\n\
    bool argument 2\n\
\n\
-int1 <int> = 1\n\
    int argument 1\n\
\n\
-int2 <int> [4 .. 8] = 0\n\
    int argument 2\n\
\n\
-int3 <int> [-8 .. 8] = 6\n\
    int argument 3\n\
\n\
-float1 <float> = 1\n\
    float argument 1\n\
\n\
-float2 <float> [4 .. 8] = 0\n\
    float argument 2\n\
\n\
-float3 <float> [-8 .. 8] = 6\n\
    float argument 3\n\
\n\
-string1 <string> = default string\n\
    string argument 1\n\
\n\
-path1 <path> = default path\n\
    path argument 1\n\
\n\
-enum1 <A | B | C | D | E> = C\n\
    enum argument 1\n\
\n\
\n\
");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsHelpRequested")
  {
    wdCommandLineUtils cmd;

    WD_TEST_BOOL(!wdCommandLineOption::IsHelpRequested(&cmd));

    cmd.InjectCustomArgument("-help");

    WD_TEST_BOOL(wdCommandLineOption::IsHelpRequested(&cmd));
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "RequireOptions")
  {
    wdCommandLineUtils cmd;
    wdString missing;

    WD_TEST_BOOL(wdCommandLineOption::RequireOptions("-opt1 ; -opt2", &missing, &cmd).Failed());
    WD_TEST_STRING(missing, "-opt1");

    cmd.InjectCustomArgument("-opt1");

    WD_TEST_BOOL(wdCommandLineOption::RequireOptions("-opt1 ; -opt2", &missing, &cmd).Failed());
    WD_TEST_STRING(missing, "-opt2");

    cmd.InjectCustomArgument("-opt2");

    WD_TEST_BOOL(wdCommandLineOption::RequireOptions("-opt1 ; -opt2", &missing, &cmd).Succeeded());
    WD_TEST_STRING(missing, "");
  }
}
