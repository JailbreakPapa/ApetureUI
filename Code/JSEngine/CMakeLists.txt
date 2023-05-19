wd_cmake_init()

set(PROJECT_NAME apetrureuijs)

wd_create_target(LIBRARY ${PROJECT_NAME})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()
# Try to find vcpkg libs needed for the JSEngine.

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Core
  Foundation
  Utilities
  V8
  unofficial::chakracore::chakracore
  
)

wd_export_v8_binary(${PROJECT_NAME})