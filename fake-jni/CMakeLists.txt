cmake_minimum_required(VERSION 3.8)
project(fake-jni VERSION 0.1.0 LANGUAGES CXX)

include(ExternalProject)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "You are using an unsupported compiler, please build with clang.")
endif ()

add_subdirectory(CX)

option(BUILD_FAKE_JNI_DEBUG "Enables fake-jni debug building" OFF)
if (BUILD_FAKE_JNI_DEBUG)
    set(compiler_flags "\
  -g\
  -fstandalone-debug\
  -O0\
  -fno-inline\
  -fno-omit-frame-pointer\
  -DFAKE_JNI_DEBUG\
 ")
else ()
    set(compiler_flags "-O0")
endif ()

if (BUILD_FAKE_JNI_ASAN)
    string(APPEND compiler_flags " -fsanitize=address")
endif ()

if (BUILD_FAKE_JNI_UBSAN)
    string(APPEND compiler_flags " -fsanitize=undefined")
endif ()

string(APPEND CMAKE_CXX_FLAGS " ${compiler_flags}")
string(APPEND CMAKE_C_FLAGS " ${compiler_flags}")

set(AUTOMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(AUTOMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
if (CMAKE_OSX_SYSROOT)
    string(APPEND AUTOMAKE_CXX_FLAGS " -isysroot ${CMAKE_OSX_SYSROOT}")
    string(APPEND AUTOMAKE_C_FLAGS " -isysroot ${CMAKE_OSX_SYSROOT}")
endif ()
if (CMAKE_CXX_COMPILER_TARGET)
    string(APPEND AUTOMAKE_CXX_FLAGS " --target=${CMAKE_CXX_COMPILER_TARGET}")
    string(APPEND AUTOMAKE_C_FLAGS " --target=${CMAKE_CXX_COMPILER_TARGET}")
endif ()

#libffi
if (NOT DEFINED FFI_CC)
    set(FFI_CC ${CMAKE_C_COMPILER})
endif ()

if (NOT DEFINED FFI_CXX)
    set(FFI_CXX ${CMAKE_CXX_COMPILER})
endif ()

if (NOT DEFINED FFI_CONFIGURE_FLAGS)
    set(FFI_CONFIGURE_FLAGS "")
endif ()

message("configure ${FFI_CONFIGURE_FLAGS} --disable-shared --disable-builddir --prefix=<INSTALL_DIR> --host=${CMAKE_CXX_COMPILER_TARGET} CC=${FFI_CC} CFLAGS=${AUTOMAKE_C_FLAGS} CXX=${FFI_CXX} CXXFLAGS=${AUTOMAKE_CXX_FLAGS}")
externalproject_add(ffi
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/libffi"
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND <SOURCE_DIR>/autogen.sh && <SOURCE_DIR>/configure ${FFI_CONFIGURE_FLAGS} --disable-shared --disable-builddir --prefix=<INSTALL_DIR> --host=${CMAKE_CXX_COMPILER_TARGET} CC=${FFI_CC} CFLAGS=${AUTOMAKE_C_FLAGS} CXX=${FFI_CXX} CXXFLAGS=${AUTOMAKE_CXX_FLAGS}
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libffi.a
        )

externalproject_get_property(ffi INSTALL_DIR)
set(LIBFFI_INSTALL_DIR ${INSTALL_DIR}/lib/libffi-3.2.1/)
externalproject_get_property(ffi INSTALL_DIR)
set(ffi_INSTALL_DIR ${INSTALL_DIR})

add_library(libffi STATIC IMPORTED GLOBAL)
add_library(libffi::libffi ALIAS libffi)

file(MAKE_DIRECTORY "${LIBFFI_INSTALL_DIR}/include")
set_target_properties(libffi PROPERTIES
        IMPORTED_LOCATION "${INSTALL_DIR}/lib/libffi.a"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBFFI_INSTALL_DIR}/include"
        )

#clean up libffi's non-compliant build artifacts
externalproject_get_property(ffi SOURCE_DIR)
add_custom_command(
        TARGET ffi PRE_BUILD
        COMMAND git clean -x -d -f
        COMMAND git reset --hard
        WORKING_DIRECTORY ${SOURCE_DIR}
        COMMENT "Cleaning libffi directory..."
)

#libunwind
option(BUILD_FAKE_JNI_WITH_LIBUNWIND "Enables integration with libunwind" OFF)

if (BUILD_FAKE_JNI_WITH_LIBUNWIND)
    if (NOT DEFINED UNWIND_CC)
        set(UNWIND_CC ${CMAKE_C_COMPILER})
    endif ()

    if (NOT DEFINED UNWIND_CXX)
        set(UNWIND_CXX ${CMAKE_CXX_COMPILER})
    endif ()

    if (NOT DEFINED UNWIND_CONFIGURE_FLAGS)
        set(UNWIND_CONFIGURE_FLAGS "--disable-coredump")
    endif ()

    externalproject_add(unwind
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/libunwind
            UPDATE_COMMAND ""
            CONFIGURE_COMMAND <SOURCE_DIR>/autogen.sh && <SOURCE_DIR>/configure ${UNWIND_CONFIGURE_FLAGS} --disable-minidebuginfo --disable-shared --prefix=<INSTALL_DIR> --host=${CMAKE_CXX_COMPILER_TARGET} CC=${UNWIND_CC} CFLAGS=${AUTOMAKE_C_FLAGS} CXX=${UNWIND_CXX} CXXFLAGS=${AUTOMAKE_CXX_FLAGS}
            BUILD_IN_SOURCE 1
            BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libunwind.a
            )
    externalproject_get_property(unwind INSTALL_DIR)
    set(LIBUNWIND_INSTALL_DIR ${INSTALL_DIR}/lib/)
    externalproject_get_property(unwind INSTALL_DIR)
    set(unwind_INSTALL_DIR ${INSTALL_DIR})
    add_library(libunwind STATIC IMPORTED GLOBAL)
    file(MAKE_DIRECTORY ${LIBUNWIND_INSTALL_DIR}/include)
    set_target_properties(libunwind PROPERTIES
            IMPORTED_LOCATION ${INSTALL_DIR}/lib/libunwind.a
            INTERFACE_INCLUDE_DIRECTORIES ${LIBUNWIND_INSTALL_DIR}/include
            )

    #clean up libunwind's non-compliant build artifacts
    externalproject_get_property(unwind SOURCE_DIR)
    add_custom_command(
            TARGET unwind PRE_BUILD
            COMMAND git clean -x -d -f
            COMMAND git reset --hard
            WORKING_DIRECTORY ${SOURCE_DIR}
            COMMENT "Cleaning unwind directory..."
    )
endif ()

#fake-jni
add_library(fake-jni STATIC
        include/darwin/jawt_md.h
        include/darwin/jni_md.h
        include/fake-jni/internal/jni/native.h
        include/fake-jni/internal/jvmti/jvmti.h
        include/fake-jni/internal/meta/field.h
        include/fake-jni/internal/meta/meta.h
        include/fake-jni/internal/meta/method.h
        include/fake-jni/internal/meta/types.h
        include/fake-jni/internal/util.h
        include/fake-jni/array.h
        include/fake-jni/fake-jni.h
        include/fake-jni/jvm.h
        include/fake-jni/string.h
        include/fake-jni/throwable.h
        include/fake-jni/weak.h
        include/freebsd/jawt_md.h
        include/freebsd/jni_md.h
        include/linux/jawt_md.h
        include/linux/jni_md.h
        include/solaris/jawt_md.h
        include/solaris/jni_md.h
        include/win32/bridge/AccessBridgeCallbacks.h
        include/win32/bridge/AccessBridgeCalls.c
        include/win32/bridge/AccessBridgeCalls.h
        include/win32/bridge/AccessBridgePackages.h
        include/win32/jawt_md.h
        include/win32/jni_md.h
        include/classfile_constants.h
        include/jawt.h
        include/jdwpTransport.h
        include/jni.h
        include/jvmti.h
        include/jvmticmlr.h
        src/internal/library.cpp
        src/jni/invoke/invoke.cpp
        src/jni/invoke/misc.cpp
        src/jni/invoke/thread.cpp
        src/jni/native/array.cpp
        src/jni/native/buffer.cpp
        src/jni/native/exception.cpp
        src/jni/native/field.cpp
        src/jni/native/method.cpp
        src/jni/native/misc.cpp
        src/jni/native/native.cpp
        src/jni/native/native_vararg.cpp
        src/jni/native/object.cpp
        src/jni/native/ref.cpp
        src/jni/native/reflect.cpp
        src/jni/native/string.cpp
        src/jni/jni_env.cpp
        src/jvm/array.cpp
        src/jvm/class.cpp
        src/jvm/field.cpp
        src/jvm/jvm.cpp
        src/jvm/method.cpp
        src/jvm/object.cpp
        src/jvm/ref_table.cpp
        src/jvm/string.cpp
        src/jvm/throwable.cpp
        src/jvm/weak.cpp
        src/jvmti/interface/class.cpp
        src/jvmti/interface/field.cpp
        src/jvmti/interface/jvmti.cpp
        src/jvmti/interface/jvmti_vararg.cpp
        src/jvmti/interface/method.cpp
        src/jvmti/interface/misc.cpp
        src/jvmti/interface/monitor.cpp
        src/jvmti/interface/thread.cpp
        src/jvmti/jvmti_env.cpp
        src/fake-jni.cpp
        )

target_link_libraries(fake-jni ${CMAKE_DL_LIBS})
add_dependencies(fake-jni ffi)
target_include_directories(fake-jni PUBLIC
        include
        ${cx_SOURCE_DIR}
        ${ffi_INSTALL_DIR}/include
        )
target_compile_options(fake-jni PUBLIC
        -Wall
        -Wextra
        # -Werror
        -pedantic
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-gnu-anonymous-struct
        -Wno-nested-anon-types
        -Wno-vla-extension
        -Wno-zero-length-array
        )

if (BUILD_FAKE_JNI_WITH_LIBUNWIND)
    target_link_libraries(fake-jni libunwind)
    add_dependencies(fake-jni unwind)
    target_include_directories(fake-jni PUBLIC ${unwind_INSTALL_DIR}/include)
    target_compile_definitions(fake-jni PRIVATE BUILD_FAKE_JNI_WITH_LIBUNWIND)
endif ()

option(BUILD_FAKE_JNI_TESTS "Enables fake-jni tests")
option(BUILD_FAKE_JNI_EXAMPLES "Enables fake-jni examples")

if (BUILD_FAKE_JNI_TESTS)
    add_subdirectory(tests)
endif ()

if (BUILD_FAKE_JNI_EXAMPLES)
    add_subdirectory(examples)
endif ()
