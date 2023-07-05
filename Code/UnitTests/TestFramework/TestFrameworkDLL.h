#pragma once

#include <Foundation/Basics.h>

// Configure the DLL Import/Export Define
#if WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_TESTFRAMEWORK_LIB
#    define WD_TEST_DLL WD_DECL_EXPORT
#  else
#    define WD_TEST_DLL WD_DECL_IMPORT
#  endif
#else
#  define WD_TEST_DLL
#endif

enum class wdTestAppRun
{
  Continue,
  Quit
};
