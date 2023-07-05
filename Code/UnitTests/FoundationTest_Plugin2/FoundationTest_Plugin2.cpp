#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/ConstructionCounter.h>

#include <Foundation/Configuration/CVar.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>

static wdInt32 g_iPluginState = -1;

void OnLoadPlugin();
void OnUnloadPlugin();

WD_PLUGIN_DEPENDENCY(wdFoundationTest_Plugin1);

WD_PLUGIN_ON_LOADED()
{
  OnLoadPlugin();
}

WD_PLUGIN_ON_UNLOADED()
{
  OnUnloadPlugin();
}

wdCVarInt CVar_TestInt("test2_Int", 22, wdCVarFlags::None, "Desc: test2_Int");
wdCVarFloat CVar_TestFloat("test2_Float", 2.2f, wdCVarFlags::Default, "Desc: test2_Float");
wdCVarBool CVar_TestBool("test2_Bool", true, wdCVarFlags::Save, "Desc: test2_Bool");
wdCVarString CVar_TestString("test2_String", "test2", wdCVarFlags::RequiresRestart, "Desc: test2_String");

wdCVarBool CVar_TestInited("test2_Inited", false, wdCVarFlags::None, "Desc: test2_Inited");

void OnLoadPlugin()
{
  WD_TEST_BOOL_MSG(g_iPluginState == -1, "Plugin is in an invalid state.");
  g_iPluginState = 1;

  wdCVarInt* pCVar = (wdCVarInt*)wdCVar::FindCVarByName("TestPlugin2InitCount");

  if (pCVar)
    *pCVar = *pCVar + 1;

  wdCVarBool* pCVarDep = (wdCVarBool*)wdCVar::FindCVarByName("TestPlugin2FoundDependencies");

  if (pCVarDep)
  {
    *pCVarDep = true;

    // check that all CVars from plugin1 are available (ie. plugin1 is already loaded)
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Int") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Float") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Bool") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_String") != nullptr);
  }

  CVar_TestInited = true;
}

void OnUnloadPlugin()
{
  WD_TEST_BOOL_MSG(g_iPluginState == 1, "Plugin is in an invalid state.");
  g_iPluginState = 2;

  wdCVarInt* pCVar = (wdCVarInt*)wdCVar::FindCVarByName("TestPlugin2UninitCount");

  if (pCVar)
    *pCVar = *pCVar + 1;

  wdCVarBool* pCVarDep = (wdCVarBool*)wdCVar::FindCVarByName("TestPlugin2FoundDependencies");

  if (pCVarDep)
  {
    *pCVarDep = true;

    // check that all CVars from plugin1 are STILL available (ie. plugin1 is not yet unloaded)
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Int") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Float") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_Bool") != nullptr);
    *pCVarDep = *pCVarDep && (wdCVar::FindCVarByName("test1_String") != nullptr);
  }

  CVar_TestInited = false;
}

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(PluginGroup_Plugin2, TestSubSystem2)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "PluginGroup_Plugin1"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
  }

  ON_HIGHLEVELSYSTEMS_STARTUP
  {
  }

  ON_HIGHLEVELSYSTEMS_SHUTDOWN
  {
  }

WD_END_SUBSYSTEM_DECLARATION;
// clang-format on
