set (WD_BUILD_UNITTESTS ON CACHE BOOL "Whether unit-test projects should be added to the solution")

wd_requires(WD_BUILD_UNITTESTS)

wd_add_all_subdirs()