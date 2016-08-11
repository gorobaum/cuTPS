#ifndef TPS_IMAGE_IMAGEDEFORMATION_H_
#define TPS_IMAGE_IMAGEDEFORMATION_H_

#include <vector>
#include <string>

#include "image.h"

namespace tps {

class ImageDeformation {
public:
  ImageDeformation(tps::Image image) :
    image_(image),
    result(image.getDimensions()) {};
  void apply3DSinDeformation();
  tps::Image getResult() { return result; };
private:
  std::vector<float> newPointSinDef(int x, int y, int z);
  tps::Image image_;
  tps::Image result;
  int getNearestInteger(float number) {
    if ((number - std::floor(number)) <= 0.5) return std::floor(number);
    return std::floor(number) + 1.0;
  }
};

}

#endif
