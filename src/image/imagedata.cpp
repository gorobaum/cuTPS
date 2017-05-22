#include "imagedata.h"

#include <iostream>

namespace tps {

  ImageData::ImageData(std::vector<int> dimension) {
    int size = dimension[0]*dimension[1]*dimension[2];
    imageData_ = new short[size];
    count = 1;
  }

  ImageData::ImageData(short* data, std::vector<int> dimension) {
    int size = dimension[0]*dimension[1]*dimension[2];
    imageData_ = new short[size];
    for (int i = 0; i < size; i++)
      imageData_[i] = data[i];
    count = 1;
  }

  ImageData::~ImageData() {
    count--;
    if (hasZeroReference())
      delete[] imageData_;
  }

  void ImageData::addReference() {
    count++;
  }

  void ImageData::removeReference() {
    count--;
  }

  bool ImageData::hasZeroReference() {
    return (count == 0);
  }

  short* ImageData::getPixelVector() {
    return imageData_;
  }

  short ImageData::getPixelAt(int pos) {
    return imageData_[pos];
  }

  void ImageData::changePixelAt(int pos, short value) {
    imageData_[pos] = value;
  }

} // namespace
