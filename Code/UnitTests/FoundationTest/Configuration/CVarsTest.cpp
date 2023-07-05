#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Configuration/CVar.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Utilities/CommandLineUtils.h>

WD_CREATE_SIMPLE_TEST_GROUP(Configuration);

#define wdCVarValueDefault wdCVarValue::Default
#define wdCVarValueStored wdCVarValue::Stored
#define wdCVarValueRestart wdCVarValue::Restart

// Interestingly using 'wdCVarValue::Default' directly inside a macro does not work. (?!)
#define CHECK_CVAR(var, Current, Default, Stored, Restart)      \
  WD_TEST_BOOL(var != nullptr);                                 \
  if (var != nullptr)                                           \
  {                                                             \
    WD_TEST_BOOL(var->GetValue() == Current);                   \
    WD_TEST_BOOL(var->GetValue(wdCVarValueDefault) == Default); \
    WD_TEST_BOOL(var->GetValue(wdCVarValueStored) == Stored);   \
    WD_TEST_BOOL(var->GetValue(wdCVarValueRestart) == Restart); \
  }

static wdInt32 iChangedValue = 0;
static wdInt32 iChangedRestart = 0;

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

static void ChangedCVar(const wdCVarEvent& e)
{
  switch (e.m_EventType)
  {
    case wdCVarEvent::ValueChanged:
      ++iChangedValue;
      break;
    case wdCVarEvent::RestartValueChanged:
      ++iChangedRestart;
      break;
    default:
      break;
  }
}

#endif

WD_CREATE_SIMPLE_TEST(Configuration, CVars)
{
  iChangedValue = 0;
  iChangedRestart = 0;

  // setup the filesystem
  // we need it to test the storing of cvars (during plugin reloading)

  wdStringBuilder sOutputFolder1 = wdTestFramework::GetInstance()->GetAbsOutputPath();

  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(sOutputFolder1.GetData(), "test", "output", wdFileSystem::AllowWrites) == WD_SUCCESS);

  // Delete all cvar setting files
  {
    wdStringBuilder sConfigFile;

    sConfigFile = ":output/CVars/CVars_" wdFoundationTest_Plugin1 ".cfg";

    wdFileSystem::DeleteFile(sConfigFile.GetData());

    sConfigFile = ":output/CVars/CVars_" wdFoundationTest_Plugin2 ".cfg";

    wdFileSystem::DeleteFile(sConfigFile.GetData());
  }

  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("-test1_Int2");
  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("102");

  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("-test1_Float2");
  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("102.2");

  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("-test1_Bool2");
  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("false");

  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("-test1_String2");
  wdCommandLineUtils::GetGlobalInstance()->InjectCustomArgument("test1c");

  wdCVar::SetStorageFolder(":output/CVars");
  wdCVar::LoadCVars(); // should do nothing (no settings files available)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "No Plugin Loaded")
  {
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String") == nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_String") == nullptr);
  }

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Plugin1 Loaded")
  {
    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin1) == WD_SUCCESS);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String") != nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int2") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float2") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool2") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String2") != nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_String") == nullptr);

    wdPlugin::UnloadAllPlugins();
  }

#endif

  WD_TEST_BLOCK(wdTestBlock::Enabled, "No Plugin Loaded (2)")
  {
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String") == nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_String") == nullptr);
  }

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Plugin2 Loaded")
  {
    // Plugin2 should automatically load Plugin1 with it

    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin2) == WD_SUCCESS);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String") != nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Int") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Float") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Bool") != nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_String") != nullptr);

    wdPlugin::UnloadAllPlugins();
  }

#endif

  WD_TEST_BLOCK(wdTestBlock::Enabled, "No Plugin Loaded (2)")
  {
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test1_String") == nullptr);

    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Int") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Float") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_Bool") == nullptr);
    WD_TEST_BOOL(wdCVar::FindCVarByName("test2_String") == nullptr);
  }

#if WD_ENABLED(WD_SUPPORTS_DYNAMIC_PLUGINS) && WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Default Value Test")
  {
    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin2) == WD_SUCCESS);

    // CVars from Plugin 1
    {
      wdCVarInt* pInt = (wdCVarInt*)wdCVar::FindCVarByName("test1_Int");
      CHECK_CVAR(pInt, 11, 11, 11, 11);

      if (pInt)
      {
        WD_TEST_BOOL(pInt->GetType() == wdCVarType::Int);
        WD_TEST_BOOL(pInt->GetName() == "test1_Int");
        WD_TEST_BOOL(pInt->GetDescription() == "Desc: test1_Int");

        pInt->m_CVarEvents.AddEventHandler(ChangedCVar);

        *pInt = 12;
        CHECK_CVAR(pInt, 12, 11, 11, 12);
        WD_TEST_INT(iChangedValue, 1);
        WD_TEST_INT(iChangedRestart, 0);

        // no change
        *pInt = 12;
        WD_TEST_INT(iChangedValue, 1);
        WD_TEST_INT(iChangedRestart, 0);
      }

      wdCVarFloat* pFloat = (wdCVarFloat*)wdCVar::FindCVarByName("test1_Float");
      CHECK_CVAR(pFloat, 1.1f, 1.1f, 1.1f, 1.1f);

      if (pFloat)
      {
        WD_TEST_BOOL(pFloat->GetType() == wdCVarType::Float);
        WD_TEST_BOOL(pFloat->GetName() == "test1_Float");
        WD_TEST_BOOL(pFloat->GetDescription() == "Desc: test1_Float");

        pFloat->m_CVarEvents.AddEventHandler(ChangedCVar);

        *pFloat = 1.2f;
        CHECK_CVAR(pFloat, 1.1f, 1.1f, 1.1f, 1.2f);

        WD_TEST_INT(iChangedValue, 1);
        WD_TEST_INT(iChangedRestart, 1);

        // no change
        *pFloat = 1.2f;
        WD_TEST_INT(iChangedValue, 1);
        WD_TEST_INT(iChangedRestart, 1);

        pFloat->SetToRestartValue();
        CHECK_CVAR(pFloat, 1.2f, 1.1f, 1.1f, 1.2f);

        WD_TEST_INT(iChangedValue, 2);
        WD_TEST_INT(iChangedRestart, 1);
      }

      wdCVarBool* pBool = (wdCVarBool*)wdCVar::FindCVarByName("test1_Bool");
      CHECK_CVAR(pBool, false, false, false, false);

      if (pBool)
      {
        WD_TEST_BOOL(pBool->GetType() == wdCVarType::Bool);
        WD_TEST_BOOL(pBool->GetName() == "test1_Bool");
        WD_TEST_BOOL(pBool->GetDescription() == "Desc: test1_Bool");

        *pBool = true;
        CHECK_CVAR(pBool, true, false, false, true);
      }

      wdCVarString* pString = (wdCVarString*)wdCVar::FindCVarByName("test1_String");
      CHECK_CVAR(pString, "test1", "test1", "test1", "test1");

      if (pString)
      {
        WD_TEST_BOOL(pString->GetType() == wdCVarType::String);
        WD_TEST_BOOL(pString->GetName() == "test1_String");
        WD_TEST_BOOL(pString->GetDescription() == "Desc: test1_String");

        *pString = "test1_value2";
        CHECK_CVAR(pString, "test1_value2", "test1", "test1", "test1_value2");
      }
    }

    // CVars from Plugin 2
    {
      wdCVarInt* pInt = (wdCVarInt*)wdCVar::FindCVarByName("test2_Int");
      CHECK_CVAR(pInt, 22, 22, 22, 22);

      if (pInt)
      {
        pInt->m_CVarEvents.AddEventHandler(ChangedCVar);

        *pInt = 23;
        CHECK_CVAR(pInt, 23, 22, 22, 23);
        WD_TEST_INT(iChangedValue, 3);
        WD_TEST_INT(iChangedRestart, 1);
      }

      wdCVarFloat* pFloat = (wdCVarFloat*)wdCVar::FindCVarByName("test2_Float");
      CHECK_CVAR(pFloat, 2.2f, 2.2f, 2.2f, 2.2f);

      if (pFloat)
      {
        *pFloat = 2.3f;
        CHECK_CVAR(pFloat, 2.3f, 2.2f, 2.2f, 2.3f);
      }

      wdCVarBool* pBool = (wdCVarBool*)wdCVar::FindCVarByName("test2_Bool");
      CHECK_CVAR(pBool, true, true, true, true);

      if (pBool)
      {
        *pBool = false;
        CHECK_CVAR(pBool, false, true, true, false);
      }

      wdCVarString* pString = (wdCVarString*)wdCVar::FindCVarByName("test2_String");
      CHECK_CVAR(pString, "test2", "test2", "test2", "test2");

      if (pString)
      {
        *pString = "test2_value2";
        CHECK_CVAR(pString, "test2", "test2", "test2", "test2_value2");

        pString->SetToRestartValue();
        CHECK_CVAR(pString, "test2_value2", "test2", "test2", "test2_value2");
      }
    }

    wdPlugin::UnloadAllPlugins();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Loaded Value Test")
  {
    WD_TEST_BOOL(wdPlugin::LoadPlugin(wdFoundationTest_Plugin2) == WD_SUCCESS);

    // CVars from Plugin 1
    {
      wdCVarInt* pInt = (wdCVarInt*)wdCVar::FindCVarByName("test1_Int");
      CHECK_CVAR(pInt, 12, 11, 12, 12);

      wdCVarFloat* pFloat = (wdCVarFloat*)wdCVar::FindCVarByName("test1_Float");
      CHECK_CVAR(pFloat, 1.2f, 1.1f, 1.2f, 1.2f);

      wdCVarBool* pBool = (wdCVarBool*)wdCVar::FindCVarByName("test1_Bool");
      CHECK_CVAR(pBool, false, false, false, false);

      wdCVarString* pString = (wdCVarString*)wdCVar::FindCVarByName("test1_String");
      CHECK_CVAR(pString, "test1", "test1", "test1", "test1");
    }

    // CVars from Plugin 1, overridden by command line
    {
      wdCVarInt* pInt = (wdCVarInt*)wdCVar::FindCVarByName("test1_Int2");
      CHECK_CVAR(pInt, 102, 21, 102, 102);

      wdCVarFloat* pFloat = (wdCVarFloat*)wdCVar::FindCVarByName("test1_Float2");
      CHECK_CVAR(pFloat, 102.2f, 2.1f, 102.2f, 102.2f);

      wdCVarBool* pBool = (wdCVarBool*)wdCVar::FindCVarByName("test1_Bool2");
      CHECK_CVAR(pBool, false, true, false, false);

      wdCVarString* pString = (wdCVarString*)wdCVar::FindCVarByName("test1_String2");
      CHECK_CVAR(pString, "test1c", "test1b", "test1c", "test1c");
    }

    // CVars from Plugin 2
    {
      wdCVarInt* pInt = (wdCVarInt*)wdCVar::FindCVarByName("test2_Int");
      CHECK_CVAR(pInt, 22, 22, 22, 22);

      wdCVarFloat* pFloat = (wdCVarFloat*)wdCVar::FindCVarByName("test2_Float");
      CHECK_CVAR(pFloat, 2.2f, 2.2f, 2.2f, 2.2f);

      wdCVarBool* pBool = (wdCVarBool*)wdCVar::FindCVarByName("test2_Bool");
      CHECK_CVAR(pBool, false, true, false, false);

      wdCVarString* pString = (wdCVarString*)wdCVar::FindCVarByName("test2_String");
      CHECK_CVAR(pString, "test2_value2", "test2", "test2_value2", "test2_value2");
    }

    wdPlugin::UnloadAllPlugins();
  }

#endif

  wdFileSystem::ClearAllDataDirectories();
}
