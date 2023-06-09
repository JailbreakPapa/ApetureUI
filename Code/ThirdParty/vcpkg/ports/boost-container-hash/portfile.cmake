# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/container_hash
    REF boost-1.82.0
    SHA512 651fa89c9281c16f6fa4cf56dee7bf050e3fca1b6322560ba89c35671cf4a5c14cc90b1001fbacc4084ed1b43d0084d71189e74ddd38523bab42c3880b91abcb
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})
