#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Configuration/CVar.h>

wdCVarInt CVar_TestPlugin1InitializedCount("TestPlugin1InitCount", 0, wdCVarFlags::None, "How often Plugin1 has been initialized.");
wdCVarInt CVar_TestPlugin1UninitializedCount("TestPlugin1UninitCount", 0, wdCVarFlags::None, "How often Plugin1 has been uninitialized.");
wdCVarInt CVar_TestPlugin1Reloaded("TestPlugin1Reloaded", 0, wdCVarFlags::None, "How often Plugin1 has been reloaded (counts init AND de-init).");

wdCVarInt CVar_TestPlugin2InitializedCount("TestPlugin2InitCount", 0, wdCVarFlags::None, "How often Plugin2 has been initialized.");
wdCVarInt CVar_TestPlugin2UninitializedCount("TestPlugin2UninitCount", 0, wdCVarFlags::None, "How often Plugin2 has been uninitialized.");
wdCVarInt CVar_TestPlugin2Reloaded("TestPlugin2Reloaded", 0, wdCVarFlags::None, "How often Plugin2 has been reloaded (counts init AND de-init).");
wdCVarBool CVar_TestPlugin2FoundDependencies("TestPlugin2FoundDependencies", false, wdCVarFlags::None, "Whether Plugin2 found all its dependencies (other plugins).");

WD_CREATE_SIMPLE_TEST(Configuration, Plugin)
{
  CVar_TestPlugin1InitializedCount = 0;
  CVar_TestPlugin1UninitializedCount = 0;
  CVar_TestPlugin1Reloaded = 0;
  CVar_TestPlugin2InitializedCount = 0;
  CVar_TestPlugin2UninitializedCount = 0;
  CVar_TestPlugin2Reloaded = 0;
  CVar_TestPlugin2FoundDependencies = false;

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "LoadPlugin")
  {
    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin2) == WD_SUCCESS);
    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin2, wdPluginLoadFlags::PluginIsOptional) == WD_SUCCESS); // loading already loaded plugin is always a success

    WD_TEST_INT(CVar_TestPlugin1InitializedCount, 1);
    WD_TEST_INT(CVar_TestPlugin2InitializedCount, 1);

    WD_TEST_INT(CVar_TestPlugin1UninitializedCount, 0);
    WD_TEST_INT(CVar_TestPlugin2UninitializedCount, 0);

    WD_TEST_INT(CVar_TestPlugin1Reloaded, 0);
    WD_TEST_INT(CVar_TestPlugin2Reloaded, 0);

    WD_TEST_BOOL(CVar_TestPlugin2FoundDependencies);

    // this will fail the FoundationTests, as it logs an error
    // WD_TEST_BOOL(wdPlugin::LoadPlugin("Test") == WD_FAILURE); // plugin does not exist
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "UnloadPlugin")
  {
    CVar_TestPlugin2FoundDependencies = false;
    wdPlugin::UnloadAllPlugins();

    WD_TEST_INT(CVar_TestPlugin1InitializedCount, 1);
    WD_TEST_INT(CVar_TestPlugin2InitializedCount, 1);

    WD_TEST_INT(CVar_TestPlugin1UninitializedCount, 1);
    WD_TEST_INT(CVar_TestPlugin2UninitializedCount, 1);

    WD_TEST_INT(CVar_TestPlugin1Reloaded, 0);
    WD_TEST_INT(CVar_TestPlugin2Reloaded, 0);

    WD_TEST_BOOL(CVar_TestPlugin2FoundDependencies);
    WD_TEST_BOOL(wdPlugin::LoadPlugin("Test", wdPluginLoadFlags::PluginIsOptional) == WD_FAILURE); // plugin does not exist
  }

#endif
}
