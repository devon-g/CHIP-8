#include "interpreter.hpp"
#include <iostream>

int main(int argc, char **argv) {
  Interpreter interpreter;
  std::cout << "[INFO] Loading ROM" << std::endl;
  interpreter.load_rom(argv[1]);
  std::cout << "[INFO] Running interpreter" << std::endl;
  interpreter.run();
  std::cout << "[INFO] Interpreter exited" << std::endl;
  return 0;
}
