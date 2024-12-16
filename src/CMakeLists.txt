add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp

    chip8.cpp
    chip8.hpp
    display.cpp
    display.hpp
    graphics.cpp
    graphics.hpp
    keyboard.cpp
    keyboard.hpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 imgui)
