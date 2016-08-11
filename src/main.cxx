
#include <vector>
#include <iostream>

#include "utils/runinstance.h"
#include "image/imagehandler.h"
#include "image/itkimagehandler.h"
#include "image/opcvimagehandler.h"

int main(int argc, char** argv) {
  if (argc < 1) {
    std::cout << "The correct usage is:" << std::endl;
    std::cout << "/t ./tps <configuration file>" << std::endl;
    return 0;
  }

  tps::ImageHandler *imageHandler = new tps::OPCVImageHandler();

  tps::RunInstance runInstace(argv[1], imageHandler);
  runInstace.loadData();

  return 0;
}
