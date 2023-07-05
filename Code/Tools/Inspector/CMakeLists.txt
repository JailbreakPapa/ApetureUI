wd_cmake_init()

wd_requires_qt()

wd_requires(WD_3RDPARTY_ADS_SUPPORT)

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

wd_create_target(APPLICATION ${PROJECT_NAME})

wd_make_winmain_executable(${PROJECT_NAME})

wd_link_target_qt(TARGET ${PROJECT_NAME} COMPONENTS Core Gui Widgets)


target_link_libraries(${PROJECT_NAME}
  PRIVATE
  Core
  GuiFoundation
  ads
)

