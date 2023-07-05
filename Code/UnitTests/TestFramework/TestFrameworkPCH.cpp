#include <TestFramework/TestFrameworkPCH.h>

WD_STATICLINK_LIBRARY(TestFramework)
{
  if (bReturn)
    return;

  WD_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtLogMessageDock);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestDelegate);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestFramework);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestGUI);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestModel);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_SimpleTest);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_TestBaseClass);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_TestFramework);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_TestResults);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_uwp_uwpTestApplication);
  WD_STATICLINK_REFERENCE(TestFramework_Framework_uwp_uwpTestFramework);
  WD_STATICLINK_REFERENCE(TestFramework_Utilities_TestLogInterface);
  WD_STATICLINK_REFERENCE(TestFramework_Utilities_TestOrder);
  WD_STATICLINK_REFERENCE(TestFramework_Utilities_TestSetup);
}
