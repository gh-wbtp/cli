cmake_minimum_required(VERSION 4.2)
project(wbtpcli LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Executable
add_executable(wbtpcli)
target_sources(wbtpcli PRIVATE src/main.c)
target_include_directories(wbtpcli PRIVATE include)
set_target_properties(wbtpcli PROPERTIES OUTPUT_NAME "wbtp")