cmake_minimum_required(VERSION 3.6)

project(playdl-signin-ui-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets WebEngineWidgets REQUIRED)

add_library(playdl-signin-ui-qt-lib
        src/googleloginwindow.cpp
        include/googleloginwindow.h
        src/materialbusyindicator.cpp
        src/materialbusyindicator.h
        googlesigninui.qrc
        )
target_link_libraries(playdl-signin-ui-qt-lib PUBLIC Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets)
target_include_directories(playdl-signin-ui-qt-lib PUBLIC include/)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Widgets Qt5::WebEngineWidgets playdl-signin-ui-qt-lib)
