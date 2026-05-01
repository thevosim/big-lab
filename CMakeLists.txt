cmake_minimum_required(VERSION 3.16)
project(laba1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(qt_app)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_FILES
        ${CMAKE_SOURCE_DIR}/tests/*.cpp
)
add_executable(tests ${TEST_FILES})
target_link_libraries(tests
        poly_include
        poly_src
        GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(tests)