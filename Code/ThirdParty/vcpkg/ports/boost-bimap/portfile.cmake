# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/bimap
    REF boost-1.82.0
    SHA512 3ab80d2750b472f4f3f365dc695a28df128d2b9302a6c78e94fb0974780523583e20a88a3c4e28f3c349672911bc0b730b52ef1f272a9cd70f7f219418effffa
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})
