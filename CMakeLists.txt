cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CMAKE_CXX_COMPILER AND NOT DEFINED ENV{CXX})
    set(CMAKE_CXX_COMPILER g++-14)
endif()
if(NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED ENV{CC})
    set(CMAKE_C_COMPILER gcc-14)
endif()

project(signal-compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)

# --- GoogleTest via FetchContent ---
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Suppress clang-tidy warnings for GoogleTest sources
set_target_properties(gtest gtest_main gmock gmock_main
    PROPERTIES CXX_CLANG_TIDY ""
)

# --- Optional clang-tidy integration ---
option(ENABLE_CLANG_TIDY "Run clang-tidy during build" OFF)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy requested but not found")
    endif()
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(tests)
