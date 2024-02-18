#include "display.h"
#include <algorithm>
#include <iostream>
#include <vector>

Display::Display(unsigned int width, unsigned int height) {
  this->width = width;
  this->height = height;
  this->buffer.resize(width * height);
  this->clear_buffer();
}

const std::vector<bool> &Display::get_buffer() { return this->buffer; }

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

const unsigned int Display::get_width() { return this->width; }
const unsigned int Display::get_height() { return this->height; }
