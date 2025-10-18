#include "external/miniaudio.hpp"
#include <iostream>

int main() {
  std::cout << "Hello, MicroSuono!" << std::endl;

  // Miniaudio initialization example
  ma_engine engine;
  ma_result result = ma_engine_init(NULL, &engine);
  if (result != MA_SUCCESS) {
    std::cerr << "Failed to initialize audio engine." << std::endl;
    return -1;
  } else {
    std::cout << "Audio engine initialized successfully." << std::endl;
  }
  ma_engine_uninit(&engine);

  return 0;
}
