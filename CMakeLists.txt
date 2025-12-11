cmake_minimum_required(VERSION 3.15)

# Project info
project(CarAudioCanSim
    VERSION 1.0
    DESCRIPTION "Car Audio Amplifier CAN Simulator in C++"
    LANGUAGES CXX
)

# Use Clang on macOS (default, nhưng đảm bảo luôn)
if(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
endif()

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Warnings for clean code
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Source files
file(GLOB SOURCES
    "src/*.cpp"
)

# Create executable
add_executable(car_audio_can_sim ${SOURCES})

# Include headers from include/
target_include_directories(car_audio_can_sim PRIVATE include)
