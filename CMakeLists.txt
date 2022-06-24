cmake_minimum_required(VERSION 3.22)
project(CHIP_8)

add_executable(${PROJECT_NAME} main.cpp CHIP8.cpp CHIP8.h)
target_link_libraries(${PROJECT_NAME} SDL2)