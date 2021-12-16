cmake_minimum_required(VERSION 3.13)  # CMake version check
project(bank_proj)               # Create project "simple_example"
set(CMAKE_C_STANDARD 11)            # Enable c++11 standard

set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -lpam -lpam_misc")

set (CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set (CMAKE_C_FLAGS_DEBUG "-g")
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(app app.c safe.c safe.h menu.c menu.h list.c list.h)
add_executable(auth auth.c)
