#include "keyboard.hpp"

void Keyboard::set_key(int key) { this->key = key; }
int Keyboard::get_key() { return this->key; }
void Keyboard::set_key_pressed(bool pressed) { this->pressed = pressed; }
bool Keyboard::key_is_pressed() { return this->pressed; }
