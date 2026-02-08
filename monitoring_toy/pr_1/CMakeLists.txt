cmake_minimum_required(VERSION 3.20)
project(memmon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(memmon
    src/main.cpp
    src/mem_macos.cpp
)

target_include_directories(memmon PRIVATE include)

if(APPLE)
  target_compile_definitions(memmon PRIVATE MEMMON_MACOS=1)
endif()
