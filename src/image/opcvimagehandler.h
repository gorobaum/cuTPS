#ifndef _TPS_OPCVIMAGEHANDLER_H_
#define _TPS_OPCVIMAGEHANDLER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "image.h"
#include "imagehandler.h"

namespace tps {

class OPCVImageHandler : public ImageHandler {
public:
  tps::Image loadImageData(std::string filename);
  void saveImageData(tps::Image resultImage, std::string filename);
};

} // namespace

#endif