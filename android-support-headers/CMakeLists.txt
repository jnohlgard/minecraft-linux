cmake_minimum_required(VERSION 3.0)

project(android-support-headers LANGUAGES CXX)

add_library(android-support-headers INTERFACE)
target_include_directories(android-support-headers INTERFACE include/)
