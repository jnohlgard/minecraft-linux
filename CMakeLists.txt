cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0071 NEW)

project(mcpelauncher LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IS_64BIT FALSE)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(IS_64BIT TRUE)
endif ()

#if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
#    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -m32")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
#    set(CMAKE_CXX_COMPILER_TARGET "i686-linux-gnu")
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

string(TIMESTAMP BUILD_TIMESTAMP "%Y%m%d-%H%M%S" UTC)

set(IS_ARM_BUILD FALSE)
set(IS_ARMHF_BUILD FALSE)
if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(IS_ARM_BUILD TRUE)
    set(IS_ARMHF_BUILD TRUE)
endif ()

# if(APPLE)
#     set(CMAKE_LIBRARY_ARCHITECTURE "i386-apple-darwin")
# elseif(NOT IS_ARM_BUILD)
#     set(CMAKE_LIBRARY_ARCHITECTURE "i386-linux-gnu")
# endif()

set(DEB_OS_NAME "none" CACHE STRING "Specifies the target OS for building the debs (ubuntu-xenial, ubuntu-bionic, ubuntu-disco)")
mark_as_advanced(DEB_OS_NAME)

find_program(GIT_EXEC git)
mark_as_advanced(GIT_EXEC ADVANCED)
function(git_commit_hash DIR OUT_VAL)
    if (GIT_EXEC)
        execute_process(
                COMMAND "${GIT_EXEC}" log -1 --format=%h
                WORKING_DIRECTORY "${DIR}"
                OUTPUT_VARIABLE OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(${OUT_VAL} "${OUTPUT}" PARENT_SCOPE)
    else ()
        set(${OUT_VAL} "" PARENT_SCOPE)
    endif ()
endfunction()
git_commit_hash("${CMAKE_SOURCE_DIR}" MANIFEST_GIT_COMMIT_HASH)

option(BUILD_CLIENT "Enables building of the client launcher." ON)
option(BUILD_UI "Enables building of the client ui requires qt." ON)
option(BUILD_METALAUNCHER "Build the launcher UI for managing game versions, aka metalauncher" ON)
option(BUILD_TESTING "Build tests for cll-telemetry (requires GTest)" OFF)
option(BUILD_WEBVIEW "Build webview for xboxlive login" ON)

set(QT_RPATH "" CACHE STRING "Sets the install rpath to set for qt-enabled targets")
mark_as_advanced(QT_RPATH)

option(ENABLE_MSA_DAEMON "Enables building the daemon" ON)
option(ENABLE_MSA_GTK_UI "Enables the Gtk-based UI" OFF)
option(ENABLE_MSA_QT_UI "Enables the Qt-based UI" OFF)
option(MSA_UI_PATH_DEV "Enable development MSA paths. This should be disabled for a packaged build." ON)

option(ENABLE_DEV_PATHS "Enables lookup in source directories for misc files required by the launcher. This should be disabled for package builds." ON)

option(USE_OWN_CURL "Builds CURL as an ExternalProject dependency" OFF)
option(USE_GAMECONTROLLERDB "Downloads gamecontrollerdb.txt from gabomdq/SDL_GameControllerDB" ON)

option(JNI_USE_JNIVM "if off it uses fake-jni + baron or it uses jnivm instead as jni interface" ON)

option(VERBOSE_LOGGING "Enable verbose logging in the compiled modules" OFF)

if (APPLE)
    set(NATIVES_PATH_DIR "${CMAKE_SOURCE_DIR}/mcpelauncher-mac-bin")
    set(NATIVES_PATH_LIB_DIR "${NATIVES_PATH_DIR}/lib")
else ()
    set(NATIVES_PATH_DIR "${CMAKE_SOURCE_DIR}/mcpelauncher-linux-bin")
    set(NATIVES_PATH_LIB_DIR "${NATIVES_PATH_DIR}/lib")
endif ()

if (ENABLE_DEV_PATHS)
    add_compile_definitions("DEV_EXTRA_PATHS=\"${NATIVES_PATH_DIR}:${CMAKE_BINARY_DIR}/gamecontrollerdb\"")
endif ()

if (VERBOSE_LOGGING)
    add_compile_definitions(
            SIMPLEIPC_DEBUG_LOGGING
            DAEMON_UTILS_LOGGING
            MSA_LOG_NETWORK
    )
endif ()

if (USE_OWN_CURL)
    include(ext/curl.cmake)
endif ()

include(ext/json.cmake)

if (BUILD_CLIENT)
    if (USE_GAMECONTROLLERDB)
        include(ext/gamepad_mappings.cmake)
    endif ()
    include(game-window/BuildSettings.cmake)
    if (GAMEWINDOW_SYSTEM STREQUAL "EGLUT")
        add_subdirectory(eglut)
        add_subdirectory(linux-gamepad)
    elseif (GAMEWINDOW_SYSTEM STREQUAL "GLFW")
        include(ext/glfw.cmake)
    endif ()
endif ()

if (BUILD_WEBVIEW AND BUILD_UI)
    add_subdirectory(mcpelauncher-webview)
endif ()

if (APPLE)
    add_subdirectory(osx-elf-header)
    add_subdirectory(epoll-shim)
endif ()

if (BUILD_CLIENT)
    add_subdirectory(properties-parser)
    add_subdirectory(mcpelauncher-linker)
    add_subdirectory(libc-shim)
    add_subdirectory(file-picker)
    add_subdirectory(game-window)
    add_subdirectory(cll-telemetry)
    add_subdirectory(minecraft-imported-symbols)
    add_subdirectory(mcpelauncher-common)
    add_subdirectory(mcpelauncher-core)
    add_subdirectory(android-support-headers)
    if (JNI_USE_JNIVM)
        set(JNIVM_ENABLE_RETURN_NON_ZERO ON CACHE BOOL "" FORCE)
        set(JNIVM_FAKE_JNI_MINECRAFT_LINUX_COMPAT ON CACHE BOOL "" FORCE)
        add_subdirectory(libjnivm)
    else ()
        add_subdirectory(fake-jni)
        add_subdirectory(baron)
    endif ()
    add_subdirectory(mcpelauncher-client)
endif ()
if (BUILD_CLIENT OR BUILD_UI)
    add_subdirectory(mcpelauncher-errorwindow)
endif ()

install(DIRECTORY ${NATIVES_PATH_LIB_DIR} COMPONENT mcpelauncher-bin-libs DESTINATION share/mcpelauncher PATTERN "*"
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

add_subdirectory(logger)
add_subdirectory(base64)
add_subdirectory(file-util)
add_subdirectory(arg-parser)
add_subdirectory(rapidxml)
add_subdirectory(simple-ipc)
add_subdirectory(daemon-utils/client)
add_subdirectory(daemon-utils/server)
add_subdirectory(msa-daemon-client)

if (BUILD_METALAUNCHER)
    add_subdirectory(axml-parser)
    add_subdirectory(mcpelauncher-apkinfo)
    add_subdirectory(mcpelauncher-extract)
    add_subdirectory(google-play-api)
    add_subdirectory(playdl-signin-ui-qt)
    add_subdirectory(mcpelauncher-ui-qt)

    if (ENABLE_APPIMAGE_UPDATE_CHECK)
        add_subdirectory(lib/AppImageUpdate)
    endif ()
endif ()

if (ENABLE_MSA_DAEMON)
    add_subdirectory(msa)
    add_subdirectory(msa-daemon)
endif ()

set(MSA_UI_PATH .)
set(MSA_UI_DEV_PATH "")

if (ENABLE_MSA_GTK_UI)
    add_subdirectory(msa-ui-gtk)
    list(APPEND MSA_UI_DEV_PATH ../msa-ui-gtk)
endif ()

if (ENABLE_MSA_QT_UI)
    add_subdirectory(msa-ui-qt)
    list(APPEND MSA_UI_DEV_PATH ../msa-ui-qt)
endif ()

if (MSA_UI_PATH_DEV)
    list(APPEND MSA_UI_PATH ${MSA_UI_DEV_PATH})
endif ()

if (ENABLE_MSA_DAEMON)
    list(JOIN MSA_UI_PATH ":" MSA_UI_PATH_STR)
    message("MSA UI app path has been set to: ${MSA_UI_PATH_STR}")
    target_compile_definitions(msa-daemon PRIVATE "MSA_UI_APP_PATH=\"${MSA_UI_PATH_STR}\"")
endif ()
