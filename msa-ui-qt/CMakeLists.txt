cmake_minimum_required(VERSION 3.6)

project(msa-ui-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION 5 CACHE STRING "Qt version to use")
set_property(CACHE QT_VERSION PROPERTY STRINGS 5 6)

set(qt-ver "Qt${QT_VERSION}")

find_package(${qt-ver}
        REQUIRED COMPONENTS Core Widgets WebEngineWidgets Network
        )
set(msa-ui-qt-libs ${qt-ver}::Core ${qt-ver}::Widgets ${qt-ver}::WebEngineWidgets ${qt-ver}::Network daemon-server-utils msa-daemon-client)

add_executable(msa-ui-qt
        src/main.cpp
        src/webloginwindow.cpp
        src/webloginwindow.h
        src/materialbusyindicator.cpp
        src/materialbusyindicator.h
        src/loginipcservice.cpp
        src/loginipcservice.h
        src/loginuihandler.cpp
        src/loginuihandler.h
        src/pickaccountwindow.cpp
        src/pickaccountwindow.h
        src/msadaemonmanager.cpp
        src/msadaemonmanager.h
        src/msadaemonmanager.cpp
        src/profilepicturemanager.cpp
        src/profilepicturemanager.h
        msaui.qrc
        )
target_link_libraries(msa-ui-qt PUBLIC ${msa-ui-qt-libs})

if (APPLE)
    target_sources(msa-ui-qt PRIVATE src/macosutil.mm src/macosutil.h)
    target_link_libraries(msa-ui-qt PUBLIC "-framework AppKit")
endif ()

if (QT_RPATH AND NOT QT_RPATH STREQUAL "")
    set_target_properties(msa-ui-qt PROPERTIES INSTALL_RPATH ${QT_RPATH})
endif ()

install(TARGETS msa-ui-qt RUNTIME COMPONENT msa-ui-qt DESTINATION bin)
include(CPackSettings.cmake)
