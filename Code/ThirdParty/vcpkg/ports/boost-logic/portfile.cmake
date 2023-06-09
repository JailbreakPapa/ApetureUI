# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/logic
    REF boost-1.82.0
    SHA512 9456711b5fb4591a26e42850aa4ec39863a84c74501f53de51653e61dcd414c421a7dc0fd5469efba08e86ec8f6ecabf3ed6ae8ded83b6296ea3e628a8516436
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})
