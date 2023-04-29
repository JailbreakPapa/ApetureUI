#pragma once

#include <Foundation/Basics.h>

// Configure the DLL Import/Export Define
#if WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_CORE_LIB
#    define WD_CORE_DLL WD_DECL_EXPORT
#    define WD_CORE_DLL_FRIEND WD_DECL_EXPORT_FRIEND
#  else
#    define WD_CORE_DLL WD_DECL_IMPORT
#    define WD_CORE_DLL_FRIEND WD_DECL_IMPORT_FRIEND
#  endif
#else
#  define WD_CORE_DLL
#  define WD_CORE_DLL_FRIEND
#endif
