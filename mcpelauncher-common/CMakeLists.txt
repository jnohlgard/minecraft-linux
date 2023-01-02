cmake_minimum_required(VERSION 3.12)

project(mcpelauncher-common LANGUAGES CXX)

find_package(OpenSSL COMPONENTS Crypto)

add_library(mcpelauncher-common
        include/mcpelauncher/path_helper.h
        src/path_helper.cpp
        )
if (OPENSSL_FOUND)
    target_sources(mcpelauncher-common PRIVATE
            src/openssl_multithread.h
            src/openssl_multithread.cpp
            )
endif ()
if (APPLE)
    target_sources(mcpelauncher-common PRIVATE
            src/path_helper_osx.mm
            )
endif ()

target_include_directories(mcpelauncher-common PUBLIC include/)
if (OPENSSL_FOUND)
    target_link_libraries(mcpelauncher-common PUBLIC OpenSSL::Crypto)
endif ()
if (APPLE)
    target_link_libraries(mcpelauncher-common PUBLIC "-framework Foundation")
endif ()
