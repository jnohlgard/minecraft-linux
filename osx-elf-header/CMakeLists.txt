cmake_minimum_required (VERSION 3.0)

project(osx-elf-header LANGUAGES C)

add_library(osx-elf-header INTERFACE)
target_include_directories(osx-elf-header INTERFACE include/)
