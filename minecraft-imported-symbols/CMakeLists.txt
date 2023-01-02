cmake_minimum_required(VERSION 3.0)

project(minecraft-imported-symbols LANGUAGES C CXX)

add_library(minecraft-imported-symbols INTERFACE)
target_include_directories(minecraft-imported-symbols INTERFACE include/)
