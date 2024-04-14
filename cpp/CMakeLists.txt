cmake_minimum_required(VERSION 3.26)
project(FutureConfig)

set(CMAKE_CXX_STANDARD 14) # for google test
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# search for dependencies
find_package(yaml-cpp CONFIG REQUIRED)


# GOOGLE TEST
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_library(future-config STATIC library.cpp
	src/Parser.cpp
	src/Parser.h)
target_link_libraries(future-config PRIVATE yaml-cpp)