#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

class Display {
public:
  Display(unsigned int width, unsigned int height);

  const std::vector<bool> &get_buffer();
  // Set all pixels in buffer to off
  void clear_buffer();
  // Flip pixel state
  void toggle_pixel(unsigned int x, unsigned int y);
  // Get pixel state
  bool get_pixel(unsigned int x, unsigned int y);

  const unsigned int get_width();
  const unsigned int get_height();

private:
  unsigned int width, height;
  std::vector<bool> buffer;
};

#endif
