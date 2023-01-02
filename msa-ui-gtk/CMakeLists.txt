cmake_minimum_required(VERSION 3.6)
project(msa-ui-gtk LANGUAGES C CXX)

add_subdirectory(extension)

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
# pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
pkg_check_modules(GTKMM3 REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(Webkit_GTK REQUIRED IMPORTED_TARGET webkit2gtk-4.0)

add_executable(msa-ui-gtk
        src/main.cpp
        src/WebLoginWindow.cpp
        src/WebLoginWindow.h
        src/WebView.cpp
        src/WebView.h
        src/ExtensionIPCServer.cpp
        src/ExtensionIPCServer.h
        src/LoginIPCService.cpp
        src/LoginIPCService.h
        src/UIThreadExecutor.cpp
        src/UIThreadExecutor.h
        src/PickAccountWindow.cpp
        src/PickAccountWindow.h
        src/ProfilePictureManager.cpp
        src/ProfilePictureManager.h
        src/MsaDaemonManager.cpp
        src/MsaDaemonManager.h
        )
target_link_libraries(msa-ui-gtk PUBLIC simpleipc
        daemon-server-utils
        msa-daemon-client
        PkgConfig::GTKMM3
        PkgConfig::Webkit_GTK
        CURL::libcurl
        )
target_include_directories(msa-ui-gtk PRIVATE ${GTKMM3_INCLUDE_DIRS} ${WEBKIT_GTK_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_link_libraries(msa-ui-gtk PUBLIC msa-ui-gtk-extension)

add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/resources.c" PRE_BUILD
        COMMAND glib-compile-resources --target "${CMAKE_CURRENT_BINARY_DIR}/resources.c" --sourcedir "${CMAKE_CURRENT_SOURCE_DIR}/res/" --generate-source "${CMAKE_CURRENT_SOURCE_DIR}/res/resources.xml"
)
target_sources(msa-ui-gtk PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/resources.c")

install(TARGETS msa-ui-gtk RUNTIME COMPONENT msa-ui-gtk DESTINATION bin)
include(CPackSettings.cmake)
