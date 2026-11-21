
cmake_minimum_required(VERSION 3.20)

project(DurangoX VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DurangoX
    src/main.cpp
    src/BinaryInspector.cpp
)

target_include_directories(DurangoX PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(DurangoX PRIVATE /W4)
else()
    target_compile_options(DurangoX PRIVATE -Wall -Wextra -Wpedantic)
endif()
