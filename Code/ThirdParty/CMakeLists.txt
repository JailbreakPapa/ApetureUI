wd_add_all_subdirs()

# init vcpkg so we can install needed 3rdParty 
wd_vcpkg_init()

wd_vcpkg_install(harfbuzz)
wd_vcpkg_install(icu)
wd_vcpkg_install(chakracore)
wd_vcpkg_install(yoga)
wd_vcpkg_install(libwebm)
wd_vcpkg_install(freetype)
wd_vcpkg_install(gumbo)
wd_vcpkg_install(rapidjson)
wd_vcpkg_install(libvpx)
wd_vcpkg_install(curl)
wd_vcpkg_install(zlib)

# initalize v8 by installing all needed tools.
wd_v8_init()

