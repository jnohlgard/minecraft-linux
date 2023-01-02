cmake_minimum_required(VERSION 3.6)
project(msa-ui-gtk-extension LANGUAGES C CXX)
set(MSA_UI_EXTENSION_PROJECT_NAME msa-ui-gtk-extension)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM3 REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(Webkit_GTK REQUIRED IMPORTED_TARGET webkit2gtk-4.0)

add_library(msa-ui-gtk-extension SHARED
        main.cpp
        ExternalObject.cpp
        ExternalObject.h
        JSUtils.cpp
        JSUtils.h
        IPCClient.cpp
        IPCClient.h
        )
target_link_libraries(msa-ui-gtk-extension PUBLIC PkgConfig::GTKMM3 PkgConfig::Webkit_GTK)
