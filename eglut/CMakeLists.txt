cmake_minimum_required(VERSION 3.14)

project(eglut LANGUAGES C)

find_package(PkgConfig)
pkg_check_modules(EGL REQUIRED IMPORTED_TARGET egl)
find_package(PNG REQUIRED)

set(EGLUT_USE_X11 true)

add_library(eglut
        include/eglut.h
        src/eglut.c
        src/eglutint.h
        )
target_include_directories(eglut PUBLIC include/)
target_link_libraries(eglut PUBLIC PNG::PNG PkgConfig::EGL)

if (EGLUT_USE_X11)
    find_package(X11 REQUIRED)
    target_sources(eglut PUBLIC
            src/eglut_x11.c
            include/eglut_x11.h
            src/xinput.c src/xinput.h
            )
    target_link_libraries(eglut PUBLIC X11::X11)
endif ()
