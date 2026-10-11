cmake_minimum_required(VERSION 3.16)
project(rtl_activity_analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(rtl_activity_analyzer
    src/main.cpp
    src/TraceParser.cpp
    src/ActivityAnalyzer.cpp
)
target_include_directories(rtl_activity_analyzer PRIVATE include)

include(CTest)

if(BUILD_TESTING)
    add_executable(test_activity_analyzer
        tests/test_activity_analyzer.cpp
        src/ActivityAnalyzer.cpp
    )
    target_include_directories(test_activity_analyzer PRIVATE include)
    add_test(NAME activity_analyzer COMMAND test_activity_analyzer)
endif()
