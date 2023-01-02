cmake_minimum_required(VERSION 3.0)

project(logger LANGUAGES CXX)

add_library(logger
        include/log.h
        src/log.cpp
        )
target_include_directories(logger PUBLIC include/)
