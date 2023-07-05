#include <V8Engine/V8EnginePCH.h>

// Configure the DLL Import/Export Define
#if WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_V8ENGINE_LIB
#    define WD_V8ENGINE_DLL WD_DECL_EXPORT
#    define WD_V8ENGINE_DLL_FRIEND WD_DECL_EXPORT_FRIEND
#  else
#    define WD_V8ENGINE_DLL WD_DECL_IMPORT
#    define WD_V8ENGINE_DLL_FRIEND WD_DECL_IMPORT_FRIEND
#  endif
#else
#  define WD_V8ENGINE_DLL
#  define WD_V8ENGINE_DLL_FRIEND
#endif
