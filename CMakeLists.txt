cmake_minimum_required(VERSION 3.8)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CHIP_8)

add_executable(${PROJECT_NAME}
    main.cpp
    CHIP8.h
    CHIP8.cpp)
find_package(SDL2)

target_link_libraries(${PROJECT_NAME}
    SDL2)
