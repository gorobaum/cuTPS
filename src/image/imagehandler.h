#ifndef _TPS_IMAGEHANDLER_H_
#define _TPS_IMAGEHANDLER_H_

#include <string>

#include "image.h"

namespace tps {

class ImageHandler {
public:
  virtual tps::Image loadImageData(std::string filename) = 0;
  virtual void saveImageData(tps::Image resultImage, std::string filename) = 0;
};

} // namespace

#endif