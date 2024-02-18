cmake_minimum_required(VERSION 3.8)
project(CHIP8)

add_executable(${PROJECT_NAME}
    main.cpp
    CHIP8.cpp
    graphics.cpp
    display.cpp)

find_package(SDL2)

target_link_libraries(${PROJECT_NAME}
    SDL2)
