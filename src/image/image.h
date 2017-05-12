#ifndef TPS_IMAGE_IMAGE_H_
#define TPS_IMAGE_IMAGE_H_

#include <cmath>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace tps {

  class Image {
    public:
      Image() {
        dimensions_.push_back(0);
        dimensions_.push_back(0);
        dimensions_.push_back(0);
        image = NULL;
      };
      Image(short* matImage, std::vector<int> dimensions) :
        dimensions_(dimensions) {
          image = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));
          for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
            image[i] = matImage[i];
        }
      Image(std::vector<int> dimensions) :
        dimensions_(dimensions) {
          image = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));
          for (int i = 0; i < dimensions[0]*dimensions[1]*dimensions[2]; i++)
            image[i] = 0;
        }
      Image(const Image &obj) {
        dimensions_ = obj.dimensions_;
        if (!obj.dimensions_.empty()) {
          image = (short*)malloc(dimensions_[0]*dimensions_[1]*dimensions_[2]*sizeof(short));
          if (obj.image) {
            for (int i = 0; i < dimensions_[0]*dimensions_[1]*dimensions_[2]; i++)
              image[i] = obj.image[i];
          }
        }
      }
      Image& operator=(const Image& obj) {
        dimensions_ = obj.dimensions_;
        if (!obj.dimensions_.empty()) {
          image = (short*)malloc(dimensions_[0]*dimensions_[1]*dimensions_[2]*sizeof(short));
          if (obj.image) {
            for (int i = 0; i < dimensions_[0]*dimensions_[1]*dimensions_[2]; i++)
              image[i] = obj.image[i];
          }
        }

        return *this;
      }
      ~Image() { if (image) delete(image); }

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

    private:
      short* image;
      std::vector<int> dimensions_;
      int getNearestInteger(float number) {
        if ((number - std::floor(number)) <= 0.5) return std::floor(number);
        return std::floor(number) + 1.0;
      }
  };

} // namespace

#endif
