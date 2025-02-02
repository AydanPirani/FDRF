cmake_minimum_required(VERSION 3.10)

# Project name
project(DRF_Implementation)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler and flags
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Iinclude -Wall -Wextra -std=c++20")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/common.cpp
    src/exact_queue.cpp
    src/fuzzy_queue.cpp
    src/main.cpp
    src/resource.cpp
)

# Add executable
add_executable(main ${SOURCES})

# Ensure output directories exist
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# Installation target (optional)
install(TARGETS main DESTINATION bin)
