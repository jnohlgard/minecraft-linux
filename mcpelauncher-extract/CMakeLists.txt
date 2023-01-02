cmake_minimum_required(VERSION 3.6)

project(mcpelauncher-extract LANGUAGES CXX)

find_package(PkgConfig)
pkg_check_modules(LibZip REQUIRED IMPORTED_TARGET libzip)

add_library(mcpelauncher-extract-lib
        include/mcpelauncher/zip_extractor.h
        lib/zip_extractor.cpp
        )
target_include_directories(mcpelauncher-extract-lib PUBLIC include/)
target_link_libraries(mcpelauncher-extract-lib PUBLIC PkgConfig::LibZip)

add_executable(mcpelauncher-extract
        src/main.cpp
        include/mcpelauncher/minecraft_extract_utils.h
        )
target_link_libraries(mcpelauncher-extract PUBLIC mcpelauncher-extract-lib)
