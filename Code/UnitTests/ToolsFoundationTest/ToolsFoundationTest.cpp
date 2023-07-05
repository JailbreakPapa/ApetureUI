#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/TestSetup.h>

wdInt32 wdConstructionCounter::s_iConstructions = 0;
wdInt32 wdConstructionCounter::s_iDestructions = 0;
wdInt32 wdConstructionCounter::s_iConstructionsLast = 0;
wdInt32 wdConstructionCounter::s_iDestructionsLast = 0;

WD_TESTFRAMEWORK_ENTRY_POINT("ToolsFoundationTest", "Tools Foundation Tests")
