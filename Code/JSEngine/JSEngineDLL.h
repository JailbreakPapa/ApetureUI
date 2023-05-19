/*
 *   Copyright (c) 2023 Watch Dogs LLC
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#include <JSEngine/JSEnginePCH.h>

// Configure the DLL Import/Export Define
#if WD_ENABLED(WD_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_JSENGINE_LIB
#    define WD_JSENGINE_DLL WD_DECL_EXPORT
#    define WD_JSENGINE_DLL_FRIEND WD_DECL_EXPORT_FRIEND
#  else
#    define WD_JSENGINE_DLL WD_DECL_IMPORT
#    define WD_JSENGINE_DLL_FRIEND WD_DECL_IMPORT_FRIEND
#  endif
#else
#  define WD_JSENGINE_DLL
#  define WD_JSENGINE_DLL_FRIEND
#endif
