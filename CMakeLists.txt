# ============================================================
# NESter - Build Configuration
#
# CMake manages the compilation of NESter and its dependencies.
# This configuration uses FetchContent to automatically
# download and build SDL3, eliminating the need for others
# to manually install the SDL3 library.
# ============================================================

cmake_minimum_required(VERSION 3.21)

project(NESter LANGUAGES C)


# ------------------------------------------------------------
# C Language Standard
#
# Compile the project using C11 and require compiler support
# for that standard.
# ------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# ------------------------------------------------------------
# Build Output Directories
#
# Place executable files and shared libraries in the build/bin
# directory. On Windows, this helps keep SDL3.dll alongside
# NESter.exe so the application can locate it at runtime.
# ------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


# ------------------------------------------------------------
# SDL3 Dependency Management
#
# FetchContent allows CMake to retrieve external dependencies
# automatically during project configuration.
#
# SDL3 is downloaded from its official GitHub repository
# using a specific release tag to ensure team builds
# against the same SDL3 version.
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.2
    GIT_SHALLOW TRUE
)

# Download SDL3 if necessary and make its CMake targets
# available to the NESter project.
FetchContent_MakeAvailable(SDL3)


# ------------------------------------------------------------
# NESter Executable
#
# Define the executable and identify the source files that
# CMake should compile.
#
# Additional emulator source files can be added here as
# development progresses.
# ------------------------------------------------------------
add_executable(NESter src/main.c)


# ------------------------------------------------------------
# Header File Directories
#
# Tell the compiler where to find NESter's custom header
# files, such as nes.h, when included by source files.
# ------------------------------------------------------------
target_include_directories(NESter PRIVATE include)


# ------------------------------------------------------------
# SDL3 Linking
#
# Link NESter against SDL3 so the program can use SDL3
# functions for graphics, input handling, and audio.
# ------------------------------------------------------------
target_link_libraries(NESter PRIVATE SDL3::SDL3)