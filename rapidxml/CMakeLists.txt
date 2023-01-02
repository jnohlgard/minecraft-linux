cmake_minimum_required(VERSION 3.0)

project(rapidxml LANGUAGES CXX)

add_library(rapidxml INTERFACE)
target_include_directories(rapidxml INTERFACE include/)
