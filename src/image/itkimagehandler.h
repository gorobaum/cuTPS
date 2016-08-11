#ifndef _TPS_ITKIMAGEHANDLER_H_
#define _TPS_ITKIMAGEHANDLER_H_

#include <itkImage.h>
#include <itkImageIOBase.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>

#include "image.h"
#include "imagehandler.h"

namespace tps {

class ITKImageHandler : public ImageHandler {
public:
  tps::Image loadImageData(std::string filename);
  void saveImageData(tps::Image resultImage, std::string filename);
private:
  itk::ImageIOBase::Pointer getImageIO(std::string input);
};

} // namespace

#endif