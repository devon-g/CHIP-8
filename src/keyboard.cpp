#include "keyboard.hpp"

void Keyboard::set_pressed_key(Key key) { this->pressed = key; }
Key Keyboard::get_pressed_key() { return this->pressed; }
