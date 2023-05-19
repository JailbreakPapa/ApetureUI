# #####################################
# ## wd_v8_init()
# #####################################
set(WD_V8_ROOT CACHE PATH "Path to the directory where v8 is located/wanted installed.")
set(WD_V8_BUILD_PATH CACHE PATH "Path to the directory where v8 will be cloned and built.")
set(WD_V8_READY_TO_BUILD ON CACHE BOOL "Are you ready to build v8? Make sure that you set up depot_tools. you should open the extracted directory and put: (gclient) to fully initialize everything.")
set(WD_V8_MANUAL_BUILD ON CACHE BOOL "If you want to build the latest provided version of v8, then you can manually build v8. WARNING: May have Breaking API Changes.")
set(WD_V8_PREBUILT OFF CACHE BOOL "If manually building ends up failing, you can use the latest uploaded version of v8 that works with apertureui.")

# Sets Where We Should Look For Prebuilt Version DLLs.
if(CMAKE_BUILD_TYPE STREQUAL "Dev" OR "Debug")
        set(WD_V8_DLL_TYPE "x64.debug")
    else()
        set(WD_V8_DLL_TYPE "x64.release")
endif()

function(wd_v8_init)
    if(WD_V8_ROOT STREQUAL "")
        message(FATAL_ERROR "A Directory was not provided to install v8 inside.")
    endif()
    message(STATUS "Setting up v8. this will take some time to configure and build.")
    message(STATUS "Make sure that there is no prior build to v8 (meaning any prebuilt versions) before starting.")
    message(STATUS "Currently, we are planning to do our work inside: ${WD_V8_ROOT}")
    message("")
    if(WD_V8_MANUAL_BUILD)

    if(WD_CMAKE_PLATFORM_LINUX)
            # see if we can clone gn for building cmake.
            execute_process(COMMAND "git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git" WORKING_DIRECTORY "${WD_V8_ROOT}")
            execute_process(COMMAND "export PATH=${WD_V8_ROOT}/depot_tools:$PATH" WORKING_DIRECTORY "${WD_V8_ROOT}")
    endif()
    if(WD_CMAKE_PLATFORM_WINDOWS AND WD_CMAKE_COMPILER_MSVC)
            set(DOWNLOAD_LK "${WD_DEPOT_TOOLS_INSTALL_LINK_WIN}")
            wd_download_and_extract("https://github.com/KuraStudios/WD_THIRDPARTY/raw/main/depot_tools.7z" "${WD_V8_ROOT}" "depot_tools")
    endif()

    message(WARNING "Depot tools should have been installed. MAKE SURE that depot_tools is the first thing in your OS(s) PATH. ")

    # once we have successfully extracted depot_tools, we should run gclient to set everything up.
    message(STATUS "Set up depot_tools. you should open the extracted directory and put: (gclient) to fully initialize everything.")
    execute_process(COMMAND "gclient" WORKING_DIRECTORY "${WD_V8_ROOT}")

    if(WD_V8_READY_TO_BUILD)
        wd_v8_configurate()
        # wd_v8_build()
    endif()
    else()
    # Assume user wants latest prebuilt version
        if(WD_CMAKE_PLATFORM_WINDOWS AND WD_CMAKE_COMPILER_MSVC AND WD_V8_READY_TO_BUILD)
            set(DOWNLOAD_LK "${WD_V8_PREBUILT_INSTALL_WIN}")
            wd_download_and_extract("https://github.com/KuraStudios/WD_THIRDPARTY/raw/main/v8_prebuilt.7z" "${WD_V8_ROOT}" "v8_prebuilt")
        endif()
    endif()
endfunction()

function(wd_v8_configurate)
        # From this point on, we assume gclient was called within depot_tools, and set to the FRONT of path.
        if((WD_V8_BUILD_PATH STREQUAL "WD_V8_BUILD_PATH-NOTFOUND") OR (WD_V8_BUILD_PATH STREQUAL ""))
            execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${WD_V8_ROOT}/v8_final)
            set(WD_V8_BUILD_PATH "${WD_V8_ROOT}/v8_final")
        endif()

        # Its now time to fetch v8. notify the user that this might take a while, depending on the connection.
        message(STATUS "We are now going to fetch v8. this might take a while, depending on your connection.")
        execute_process(COMMAND "fetch v8" WORKING_DIRECTORY "${WD_V8_BUILD_PATH}")

        # After cloning, we need to sync and retrieve the dependency's. 
        message(STATUS "Syncing v8.")
        execute_process(COMMAND "git fetch" WORKING_DIRECTORY "${WD_V8_BUILD_PATH}")
        execute_process(COMMAND "gclient sync" WORKING_DIRECTORY "${WD_V8_BUILD_PATH}")

endfunction()

function(wd_v8_build)
    # Notify the user that we will start building v8. this will take about 2-3 hours. 
    message(WARNING "Building V8. (DLL VERSION) this will take about 2-3 hours. coffee is recommended. NOTE: PYTHON SHOULD ALSO BE IN PATH!")
    execute_process(COMMAND "python tools/dev/gm.py x64.debug" WORKING_DIRECTORY "${WD_V8_BUILD_PATH}/v8")
    execute_process(COMMAND "python tools/dev/gm.py x64.release" WORKING_DIRECTORY "${WD_V8_BUILD_PATH}/v8")
endfunction(wd_v8_build)
