cmake_minimum_required(VERSION 3.0)

project(filepicker LANGUAGES CXX)

set(FILEPICKER_USE_ZENITY true)
set(FILEPICKER_USE_COCOA false)

if (APPLE)
    set(FILEPICKER_USE_ZENITY false)
    set(FILEPICKER_USE_COCOA true)
endif ()

add_library(filepicker
        include/file_picker.h
        include/file_picker_factory.h
        src/file_picker_factory.cpp
        )
target_include_directories(filepicker PUBLIC include/)

if (FILEPICKER_USE_ZENITY)
    target_sources(filepicker PRIVATE
            src/file_picker_zenity.cpp
            src/file_picker_zenity.h
            )
    target_compile_definitions(filepicker PRIVATE FILEPICKER_USE_ZENITY)
endif ()
if (FILEPICKER_USE_COCOA)
    target_sources(filepicker PRIVATE
            src/file_picker_cocoa.mm
            src/file_picker_cocoa.h
            )
    target_compile_definitions(filepicker PRIVATE FILEPICKER_USE_COCOA)
endif ()
