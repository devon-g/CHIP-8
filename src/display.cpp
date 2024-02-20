#include "display.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

Display::Display() : width(64), height(32) {
  this->buffer.resize(this->width * this->height);
  this->clear_buffer();
}

Display::Display(unsigned int width, unsigned int height)
    : width(width), height(height) {
  this->buffer.resize(this->width * this->height);
  this->clear_buffer();
}

std::vector<bool> &Display::get_buffer() { return this->buffer; }

void Display::clear_buffer() {
  std::fill(this->buffer.begin(), this->buffer.end(), false);
}

void Display::toggle_pixel(unsigned int x, unsigned int y) {
  int pixel = this->width * y + x;
  if (pixel < buffer.size())
    this->buffer[pixel] = !this->buffer[pixel];
  else
    std::cout << "[ERROR] toggle_pixel: invalid pixel coordinates (" << x
              << ", " << y << ")" << std::endl;
}

bool Display::get_pixel(unsigned int x, unsigned int y) {
  int pixel = this->width * y + x;
  if (pixel > buffer.size()) {
    std::cout << "[ERROR] get_pixel: invalid pixel coordinates (" << x << ", "
              << y << ")" << std::endl;
    return false;
  }

  return this->buffer[pixel];
}

unsigned int Display::get_width() { return this->width; }
unsigned int Display::get_height() { return this->height; }
