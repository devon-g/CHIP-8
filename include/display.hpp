#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

class Display {
public:
  Display();
  Display(unsigned int width, unsigned int height);

  std::vector<bool> &get_buffer();
  // Set all pixels in buffer to off
  void clear_buffer();
  // Flip pixel state
  void toggle_pixel(unsigned int x, unsigned int y);
  // Get pixel state
  bool get_pixel(unsigned int x, unsigned int y);

  unsigned int get_width();
  unsigned int get_height();

private:
  unsigned int width, height;
  std::vector<bool> buffer;
};

#endif
