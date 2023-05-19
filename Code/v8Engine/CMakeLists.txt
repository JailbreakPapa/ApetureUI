wd_cmake_init()

set(PROJECT_NAME apertureuiv8)

wd_create_target(LIBRARY ${PROJECT_NAME})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  Core
  Foundation
  Utilities
)