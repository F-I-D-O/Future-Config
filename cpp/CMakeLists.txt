cmake_minimum_required(VERSION 3.26)
project(FutureConfig)

set(CMAKE_CXX_STANDARD 17) # for google test
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(JINJA2CPP_DEPS_MODE "external" CACHE STRING "Jinja2Cpp dependencies mode")
set(JINJA2_PRIVATE_LIBS_INT Boost::filesystem Boost::regex fmt RapidJson CACHE STRING "Jinja2Cpp private libraries")

# search for dependencies
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem regex)
find_package(fmt CONFIG REQUIRED)
find_package(RapidJson CONFIG REQUIRED)
find_package(expected-lite CONFIG REQUIRED)
find_package(variant-lite CONFIG REQUIRED)
find_package(optional-lite CONFIG REQUIRED)
find_package(string-view-lite CONFIG REQUIRED)


# GIT DEPENDENCIES

# Google Test
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Jinja2Cpp
FetchContent_Declare(
	jinja2cpp
	URL https://github.com/jinja2cpp/Jinja2Cpp/archive/refs/tags/1.3.1.zip
)
FetchContent_MakeAvailable(jinja2cpp)


add_library(future-config STATIC library.cpp
	src/Parser.cpp
	src/Parser.h)
target_link_libraries(future-config PRIVATE yaml-cpp)
target_link_libraries(future-config PRIVATE jinja2cpp)
#target_link_libraries(future-config PRIVATE boost::variant)