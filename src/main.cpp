
#include <vector>
#include <iostream>

#include "utils/controller.h"
#include "image/imagehandler.h"

int main(int argc, char** argv) {
  if (argc < 1) {
    std::cout << "The correct usage is:" << std::endl;
    std::cout << "/t ./tps <configuration file>" << std::endl;
    return 0;
  }
  cudaDeviceReset();
  cudaThreadExit();

  tps::Controller controller(argv[1]);
  controller.exec();

  cudaThreadExit();
  cudaDeviceReset();

  return 0;
}
