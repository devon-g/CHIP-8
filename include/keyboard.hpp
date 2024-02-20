#ifndef KEYBOARD_H
#define KEYBOARD_H

class Keyboard {
public:
  Keyboard() : pressed(false), key(0x0) {}
  void set_key(int key);
  int get_key();
  void set_key_pressed(bool pressed);
  bool key_is_pressed();

private:
  bool pressed;
  int key;
};

#endif
