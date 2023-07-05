#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/System/EnvironmentVariableUtils.h>

#if WD_DISABLED(WD_PLATFORM_WINDOWS_UWP)

static wdUInt32 uiVersionForVariableSetting = 0;

WD_CREATE_SIMPLE_TEST(Utility, EnvironmentVariableUtils)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "GetValueString / GetValueInt")
  {
#  if WD_ENABLED(WD_PLATFORM_WINDOWS)

    // Windows will have "NUMBER_OF_PROCESSORS" and "USERNAME" set, let's see if we can get them
    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet("NUMBER_OF_PROCESSORS"));

    wdInt32 iNumProcessors = wdEnvironmentVariableUtils::GetValueInt("NUMBER_OF_PROCESSORS", -23);
    WD_TEST_BOOL(iNumProcessors > 0);

    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet("USERNAME"));
    wdString szUserName = wdEnvironmentVariableUtils::GetValueString("USERNAME");
    WD_TEST_BOOL(szUserName.GetElementCount() > 0);

#  elif WD_ENABLED(WD_PLATFORM_OSX) || WD_ENABLED(WD_PLATFORM_LINUX)

    // Mac OS & Linux will have "USER" set
    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet("USER"));
    wdString szUserName = wdEnvironmentVariableUtils::GetValueString("USER");
    WD_TEST_BOOL(szUserName.GetElementCount() > 0);

#  endif
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "IsVariableSet/SetValue")
  {
    wdStringBuilder szVarName;
    szVarName.Format("WD_THIS_SHOULDNT_EXIST_NOW_OR_THIS_TEST_WILL_FAIL_{0}", uiVersionForVariableSetting++);

    WD_TEST_BOOL(!wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    wdEnvironmentVariableUtils::SetValueString(szVarName, "NOW_IT_SHOULD_BE").IgnoreResult();
    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    WD_TEST_STRING(wdEnvironmentVariableUtils::GetValueString(szVarName), "NOW_IT_SHOULD_BE");

    // Test overwriting the same value again
    wdEnvironmentVariableUtils::SetValueString(szVarName, "NOW_IT_SHOULD_BE_SOMETHING_ELSE").IgnoreResult();
    WD_TEST_STRING(wdEnvironmentVariableUtils::GetValueString(szVarName), "NOW_IT_SHOULD_BE_SOMETHING_ELSE");
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Variable with very long value")
  {
    // The Windows implementation has a 64 wchar_t buffer for example. Let's try setting a really
    // long variable and getting it back
    const char* szLongVariable =
      "SOME REALLY LONG VALUE, LETS TEST SOME LIMITS WE MIGHT HIT - 012456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz";

    wdStringBuilder szVarName;
    szVarName.Format("WD_LONG_VARIABLE_TEST_{0}", uiVersionForVariableSetting++);

    WD_TEST_BOOL(!wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    wdEnvironmentVariableUtils::SetValueString(szVarName, szLongVariable).IgnoreResult();
    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    WD_TEST_STRING(wdEnvironmentVariableUtils::GetValueString(szVarName), szLongVariable);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Unsetting variables")
  {
    const char* szVarName = "WD_TEST_HELLO_WORLD";
    WD_TEST_BOOL(!wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    wdEnvironmentVariableUtils::SetValueString(szVarName, "TEST").IgnoreResult();

    WD_TEST_BOOL(wdEnvironmentVariableUtils::IsVariableSet(szVarName));

    wdEnvironmentVariableUtils::UnsetVariable(szVarName).IgnoreResult();
    WD_TEST_BOOL(!wdEnvironmentVariableUtils::IsVariableSet(szVarName));
  }
}

#endif
