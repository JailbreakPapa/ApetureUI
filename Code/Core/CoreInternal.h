#pragma once

#ifdef BUILDSYSTEM_BUILDING_CORE_LIB
#  define WD_CORE_INTERNAL_HEADER_ALLOWED 1
#else
#  define WD_CORE_INTERNAL_HEADER_ALLOWED 0
#endif

#define WD_CORE_INTERNAL_HEADER static_assert(WD_CORE_INTERNAL_HEADER_ALLOWED, "This is an internal wd header. Please do not #include it directly.");
