cmake_minimum_required(VERSION 3.16)
project(rtl_activity_analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rtl_activity_analyzer src/main.cpp)
target_include_directories(rtl_activity_analyzer PRIVATE include)
