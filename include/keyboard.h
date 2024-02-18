#ifndef KEYBOARD_H
#define KEYBOARD_H

enum Key {
  ZERO,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  A,
  B,
  C,
  D,
  E,
  F,
  NONE
};

class Keyboard {
public:
  void set_pressed_key(Key key);
  Key get_pressed_key();

private:
  Key pressed = Key::NONE;
};

#endif
