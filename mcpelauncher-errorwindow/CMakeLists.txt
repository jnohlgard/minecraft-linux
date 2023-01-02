cmake_minimum_required(VERSION 3.10)

project(mcpelauncher-errorwindow LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(ENABLE_QT_ERROR_UI "Build this QT component to show errormessages" ON)

if (ENABLE_QT_ERROR_UI AND BUILD_UI)
    find_package(Qt5 REQUIRED
            COMPONENTS Core Quick Widgets
            OPTIONAL_COMPONENTS LinguistTools QuickCompiler
            )

    set(LAUNCHER_LINGUIST_RESOURCES "")
    if (Qt5LinguistTools_FOUND)
        qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} mcpelauncher_en.ts)
        configure_file(translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
        set(LAUNCHER_LINGUIST_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc ${QM_FILES})
    endif ()

    if (Qt5QuickCompiler_FOUND)
        qtquick_compiler_add_resources(RESOURCES qml.qrc)
        set(RESOURCES ${RESOURCES} ${LAUNCHER_LINGUIST_RESOURCES})
    else ()
        set(RESOURCES qml.qrc ${LAUNCHER_LINGUIST_RESOURCES})
    endif ()

    add_executable(mcpelauncher-error src/main.cpp ${RESOURCES})

    target_link_libraries(mcpelauncher-error PUBLIC Qt5::Core Qt5::Quick Qt5::Widgets)

    install(TARGETS mcpelauncher-error RUNTIME COMPONENT mcpelauncher-error DESTINATION bin)
endif ()

if (BUILD_CLIENT)
    add_library(mcpelauncher-errorwindow src/errorwindow.cpp)
    target_link_libraries(mcpelauncher-errorwindow PUBLIC gamewindow file-util)
    target_include_directories(mcpelauncher-errorwindow PUBLIC include)
endif ()
