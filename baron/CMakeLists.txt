cmake_minimum_required(VERSION 3.8)

project(baron VERSION 0.1.0 LANGUAGES CXX)

if (BUILD_BARON_DEBUG)
    set(compiler_flags "\
        -g\
        -fstandalone-debug\
        -O0\
        -fno-inline\
        -fno-omit-frame-pointer\
        -DBARON_DEBUG\
    ")
else ()
    set(compiler_flags "-O3")
endif ()

if (BUILD_BARON_ASAN)
    string(APPEND compiler_flags " -fsanitize=address")
endif ()

if (BUILD_BARON_UBSAN)
    string(APPEND compiler_flags " -fsanitize=undefined")
endif ()

add_library(baron STATIC
        src/impl/env/jni.cpp
        src/impl/env/jvmti.cpp
        src/impl/interface/invoke/misc.cpp
        src/impl/interface/invoke/thread.cpp
        src/impl/interface/jvmti/jvmti.cpp
        src/impl/interface/native/array.cpp
        src/impl/interface/native/buffer.cpp
        src/impl/interface/native/field.cpp
        src/impl/interface/native/method.cpp
        src/impl/interface/native/misc.cpp
        src/impl/interface/native/native.cpp
        src/impl/interface/native/object.cpp
        src/impl/interface/native/ref.cpp
        src/impl/interface/native/reflect.cpp
        src/impl/interface/native/string.cpp
        src/impl/class.cpp
        src/impl/field.cpp
        src/impl/jni_env.cpp
        src/impl/jvm.cpp
        src/impl/method.cpp
        src/util/util.cpp
        )
target_link_libraries(baron PUBLIC fake-jni)
target_include_directories(baron PUBLIC include)
target_compile_options(baron PUBLIC
        -Wall
        -Wextra
        #    -Werror
        -pedantic
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-nested-anon-types
        -Wno-vla-extension
        -Wno-zero-length-array
        )

option(BUILD_BARON_EXAMPLES "Enables baron example" ON)

if (BUILD_BARON_EXAMPLES)
    add_subdirectory(example)
endif ()
