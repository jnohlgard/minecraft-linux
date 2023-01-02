cmake_minimum_required(VERSION 3.0)

project(argparser LANGUAGES CXX)

add_library(argparser INTERFACE)
target_include_directories(argparser INTERFACE include/)
