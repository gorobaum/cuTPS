
#include <vector>
#include <iostream>

#include <armadillo>

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
  arma::wall_clock timer;
  timer.tic();

  tps::Controller controller(argv[1]);
  controller.exec();

  double time = timer.toc();
  std::cout << "Total execution time: " << time << std::endl;

  cudaThreadExit();
  cudaDeviceReset();

  return 0;
}
