# #####################################
# ## wd_v8_init()
# #####################################
set(WD_V8_ROOT CACHE PATH "Path to the directory where v8 is located/wanted installed.")
function(wd_v8_init)
    if(WD_V8_ROOT STREQUAL "")
        message("A Directory was not provided to install v8 inside.")
    endif()
    message("Setting up v8. this will take some time to configure and build.")
    message("Make sure that there is no prior build to v8 (meaning any prebuilt versions) before starting.")
    message(STATUS "Currently, we are planning to do our work inside: ${WD_V8_ROOT}")
    message("")
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
    message(STATUS "Set up depot_tools. you should open the extracted directory and put: (gclient) to fully initalize everything.")
    execute_process(COMMAND "gclient" WORKING_DIRECTORY "${WD_V8_ROOT}")

endfunction()

function(wd_v8_configurate)

endfunction()