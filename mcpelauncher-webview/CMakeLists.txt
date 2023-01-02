cmake_minimum_required(VERSION 3.12)

project(mcpelauncher-webview LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets WebEngineWidgets Qml)

add_executable(mcpelauncher-webview
        src/main.cpp
        src/qml.qrc
        src/main.h
        )
target_link_libraries(mcpelauncher-webview
        Qt5::Core
        Qt5::Widgets
        Qt5::WebEngineWidgets
        Qt5::Qml
        )

if (QT_RPATH)
    set_target_properties(mcpelauncher-webview PROPERTIES INSTALL_RPATH ${QT_RPATH})
endif ()

install(TARGETS mcpelauncher-webview RUNTIME COMPONENT webview DESTINATION bin)
