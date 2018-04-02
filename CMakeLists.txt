cmake_minimum_required (VERSION 2.6)

project(rapidxml LANGUAGES CXX)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE include/)