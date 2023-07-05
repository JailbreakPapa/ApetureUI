#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/ConstructionCounter.h>

#include <Foundation/Configuration/CVar.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/Reflection/Reflection.h>

static wdInt32 g_iPluginState = -1;

void OnLoadPlugin();
void OnUnloadPlugin();

wdCVarInt CVar_TestInt("test1_Int", 11, wdCVarFlags::Save, "Desc: test1_Int");
wdCVarFloat CVar_TestFloat("test1_Float", 1.1f, wdCVarFlags::RequiresRestart, "Desc: test1_Float");
wdCVarBool CVar_TestBool("test1_Bool", false, wdCVarFlags::None, "Desc: test1_Bool");
wdCVarString CVar_TestString("test1_String", "test1", wdCVarFlags::Default, "Desc: test1_String");

wdCVarInt CVar_TestInt2("test1_Int2", 21, wdCVarFlags::Default, "Desc: test1_Int2");
wdCVarFloat CVar_TestFloat2("test1_Float2", 2.1f, wdCVarFlags::Default, "Desc: test1_Float2");
wdCVarBool CVar_TestBool2("test1_Bool2", true, wdCVarFlags::Default, "Desc: test1_Bool2");
wdCVarString CVar_TestString2("test1_String2", "test1b", wdCVarFlags::Default, "Desc: test1_String2");

WD_PLUGIN_ON_LOADED()
{
  OnLoadPlugin();
}

WD_PLUGIN_ON_UNLOADED()
{
  OnUnloadPlugin();
}

void OnLoadPlugin()
{
  WD_TEST_BOOL_MSG(g_iPluginState == -1, "Plugin is in an invalid state.");
  g_iPluginState = 1;

  wdCVarInt* pCVar = (wdCVarInt*)wdCVar::FindCVarByName("TestPlugin1InitCount");

  if (pCVar)
    *pCVar = *pCVar + 1;

  wdCVarBool* pCVarPlugin2Inited = (wdCVarBool*)wdCVar::FindCVarByName("test2_Inited");
  if (pCVarPlugin2Inited)
  {
    WD_TEST_BOOL(*pCVarPlugin2Inited == false); // Although Plugin2 is present, it should not yet have been initialized
  }
}

void OnUnloadPlugin()
{
  WD_TEST_BOOL_MSG(g_iPluginState == 1, "Plugin is in an invalid state.");
  g_iPluginState = 2;

  wdCVarInt* pCVar = (wdCVarInt*)wdCVar::FindCVarByName("TestPlugin1UninitCount");

  if (pCVar)
    *pCVar = *pCVar + 1;
}

// clang-format off
WD_BEGIN_SUBSYSTEM_DECLARATION(PluginGroup_Plugin1, TestSubSystem1)

  //BEGIN_SUBSYSTEM_DEPENDENCIES
  //  "PluginGroup_Plugin1"
  //END_SUBSYSTEM_DEPENDENCIES

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

struct wdTestStruct2
{
  float m_fFloat2;

  wdTestStruct2() { m_fFloat2 = 42.0f; }
};

WD_DECLARE_REFLECTABLE_TYPE(WD_NO_LINKAGE, wdTestStruct2);

// clang-format off
WD_BEGIN_STATIC_REFLECTED_TYPE(wdTestStruct2, wdNoBase, 1, wdRTTIDefaultAllocator<wdTestStruct2>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Float2", m_fFloat2),
  }
  WD_END_PROPERTIES;
}
WD_END_STATIC_REFLECTED_TYPE;
// clang-format on
