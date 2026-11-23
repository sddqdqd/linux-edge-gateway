cmake_minimum_required(VERSION 3.10)
project(imx6ull_edge_gateway VERSION 0.1.0 LANGUAGES C CXX)

# Enforce C11 and C++11 standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for VS Code / clangd IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ENABLE_ASAN "Enable AddressSanitizer (recommended for x86 host debugging)" OFF)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

if(ENABLE_ASAN)
    message(STATUS "AddressSanitizer enabled")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address)
endif()

find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_party
)

# Core sources
set(GATEWAY_SOURCES
    src/main.cpp
    src/common/config.cpp
    src/protocol/ring_buffer.cpp
    src/protocol/frame_codec.cpp
    src/net/connection.cpp
    src/net/reactor_server.cpp
    src/concurrency/thread_pool.cpp
    src/hal/hal_sysfs.c
)

# TODO (Week 3): After adding third_party/cjson/cJSON.c and third_party/log/log.c,
# append them to GATEWAY_SOURCES here.

add_executable(edge_gateway ${GATEWAY_SOURCES})

target_link_libraries(edge_gateway PRIVATE Threads::Threads)

# Summary
message(STATUS "Build type       : ${CMAKE_BUILD_TYPE}")
message(STATUS "C compiler       : ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler     : ${CMAKE_CXX_COMPILER}")
message(STATUS "Target processor : ${CMAKE_SYSTEM_PROCESSOR}")
