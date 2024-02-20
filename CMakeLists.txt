cmake_minimum_required(VERSION 3.8)
project(chip8)

find_package(SDL2)

set(SOURCES
  src/main.cpp
  src/chip8.cpp
  src/graphics.cpp
  src/display.cpp
  src/keyboard.cpp
  src/interpreter.cpp
  include/chip8.hpp
  include/graphics.hpp
  include/display.hpp
  include/keyboard.hpp
  include/interpreter.hpp)

set(IMGUI_SOURCES
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_tables.cpp
  imgui/backends/imgui_impl_sdlrenderer2.cpp
  imgui/backends/imgui_impl_sdl2.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${IMGUI_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include imgui)
target_link_libraries(${PROJECT_NAME}
  SDL2::SDL2)
