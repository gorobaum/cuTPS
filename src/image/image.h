#ifndef TPS_IMAGE_IMAGE_H_
#define TPS_IMAGE_IMAGE_H_

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "imagedata.h"

namespace tps {

  class Image {
    public:
      Image() {
        dimensions_.push_back(0);
        dimensions_.push_back(0);
        dimensions_.push_back(0);
        image = new ImageData(dimensions_);
      };
      Image(short* matImage, std::vector<int> dimensions) :
        dimensions_(dimensions) {
          image = new ImageData(dimensions);
          for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
            image->changePixelAt(i, matImage[i]);
        }
      Image(std::vector<int> dimensions) :
        dimensions_(dimensions) {
          image = new ImageData(dimensions);
          for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
            image->changePixelAt(i, 0);
        }
      Image(const Image &obj) {
        dimensions_ = obj.dimensions_;
        if (obj.image != NULL) {
          image = obj.image;
          image->addReference();
        }
      }
      Image& operator=(const Image& obj) {
        dimensions_ = obj.dimensions_;
        if (image != NULL)
          image->removeReference();
        if (obj.image != NULL) {
          image = obj.image;
          image->addReference();
        }

        return *this;
      }
      ~Image() {
        if (image != NULL) {
          if (image->getCount() == 1)
            delete image;
          else
            image->removeReference();
        }
      }

      bool isTwoDimensional();
      int numberOfDimension();
      short* getPixelVector();
      float* getFloatPixelVector();
      std::vector<short> getMinMax();
      float meanSquaredError(Image sub);
      void setPixelVector(short* vector);
      short getPixelAt(int x, int y, int z);
      Image createSubtractionImageFrom(Image sub);
      short NNInterpolation(float x, float y, float z);
      float radialSum(int r, std::vector<float> point);
      void changePixelAt(int x, int y, int z, short value);
      short trilinearInterpolation(float x, float y, float z);
      float meanSquaredErrorWithRegion(Image sub, std::vector<int> region);
      std::vector<int> getDimensions() { return dimensions_; };
      Image createSubtractionImageFromWithRegion(Image sub,
          std::vector<int> region);

      int getCount() {return image->getCount(); }

    private:
      ImageData* image;
      std::vector<int> dimensions_;
      int getNearestInteger(float number) {
        if ((number - std::floor(number)) <= 0.5) return std::floor(number);
        return std::floor(number) + 1.0;
      }
  };

} // namespace

#endif
