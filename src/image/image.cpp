#include "image.h"

#include <iostream>

#include "utils/instanceconfiguration.h"

void tps::Image::changePixelAt(int x, int y, int z, short value) {
  if (x >= 0 && x < dimensions_[0]-1 && y >= 0 && y < dimensions_[1]-1 && z >= 0 && z <= dimensions_[2]-1) {
    int pos = x+y*dimensions_[0]+z*dimensions_[0]*dimensions_[1];
    image->changePixelAt(pos, value);
  }
}

std::vector<short> tps::Image::getMinMax() {
  short min = 0;
  short max = 0;
  for (int z = 0; z < dimensions_[2]; z++)
    for (int x = 0; x < dimensions_[0]; x++)
      for (int y = 0; y < dimensions_[1]; y++) {
        short value = getPixelAt(x, y, z);
        if (value < min) min = value;
        if (value > max) max = value;
      }
  std::vector<short> minMax;
  minMax.push_back(min);
  minMax.push_back(max);
  return minMax;
}

bool tps::Image::isTwoDimensional() {
  return (dimensions_[2] == 1);
}

int tps::Image::numberOfDimension() {
  return (dimensions_[2] == 1) ? 2 : 3;
}

short tps::Image::getPixelAt(int x, int y, int z) {
  if (x > dimensions_[0]-1 || x < 0)
    return 0;
  else if (y > dimensions_[1]-1 || y < 0)
    return 0;
  else if (z > dimensions_[2]-1 || z < 0)
    return 0;
  else {
    int pos = x+y*dimensions_[0]+z*dimensions_[0]*dimensions_[1];
    return image->getPixelAt(pos);
  }
}

short tps::Image::trilinearInterpolation(float x, float y, float z) {
  int u = trunc(x);
  int v = trunc(y);
  int w = trunc(z);

  float xd = (x - u);
  float yd = (y - v);
  float zd = (z - w);

  short c00 = getPixelAt(u, v, w)*(1-xd)+getPixelAt(u+1, v, w)*xd;
  short c10 = getPixelAt(u, v+1, w)*(1-xd)+getPixelAt(u+1, v+1, w)*xd;
  short c01 = getPixelAt(u, v, w+1)*(1-xd)+getPixelAt(u+1, v, w+1)*xd;
  short c11 = getPixelAt(u, v+1, w+1)*(1-xd)+getPixelAt(u+1, v+1, w+1)*xd;

  short c0 = c00*(1-yd)+c10*yd;
  short c1 = c01*(1-yd)+c11*yd;

  short newValue = c0*(1-zd)+c1*zd;
  if (newValue < 0) newValue = 0;

  return newValue;
}

tps::Image tps::Image::createSubtractionImageFrom(Image sub) {
  tps::Image result(dimensions_);
  for (int x = 0; x < dimensions_[0]; x++)
    for (int y = 0; y < dimensions_[1]; y++)
      for (int z = 0; z < dimensions_[2]; z++) {
        short subVoxel = sub.getPixelAt(x, y, z);
        short thisVoxel = getPixelAt(x, y, z);
        short resultVoxel = std::abs(subVoxel - thisVoxel);
        result.changePixelAt(x, y, z, resultVoxel);
      }
  return result;
}

tps::Image tps::Image::createSubtractionImageFromWithRegion(Image sub,
    std::vector<int> region) {
  tps::Image result(dimensions_);
  for (int x = region[0]; x < region[1]; x++)
    for (int y = region[2]; y < region[3]; y++)
      for (int z = region[4]; z < region[5]; z++) {
        short subVoxel = sub.getPixelAt(x, y, z);
        short thisVoxel = getPixelAt(x, y, z);
        short resultVoxel = std::abs(subVoxel - thisVoxel);
        result.changePixelAt(x, y, z, resultVoxel);
      }
  return result;
}

float tps::Image::meanSquaredError(Image sub) {
  float mse = 0.0;
  for (int x = 0; x < dimensions_[0]; x++)
    for (int y = 0; y < dimensions_[1]; y++)
      for (int z = 0; z < dimensions_[2]; z++) {
        short subVoxel = sub.getPixelAt(x, y, z);
        short thisVoxel = getPixelAt(x, y, z);
        float diff = 1.0*(subVoxel - thisVoxel);
        mse += diff*diff;
      }
  mse /= dimensions_[0]*dimensions_[1]*dimensions_[2];
  return mse;
}

float tps::Image::meanSquaredErrorWithRegion(Image sub,
    std::vector<int> region) {
  float mse = 0.0;
  for (int x = region[0]; x < region[1]; x++)
    for (int y = region[2]; y < region[3]; y++)
      for (int z = region[4]; z < region[5]; z++) {
        short subVoxel = sub.getPixelAt(x, y, z);
        short thisVoxel = getPixelAt(x, y, z);
        float diff = 1.0*(subVoxel - thisVoxel);
        mse += diff*diff;
      }
  mse /= dimensions_[0]*dimensions_[1]*dimensions_[2];
  return mse;
}


short tps::Image::NNInterpolation(float x, float y, float z) {
  int nearX = getNearestInteger(x);
  int nearY = getNearestInteger(y);
  int nearZ = getNearestInteger(z);
  int aux = getPixelAt(nearX, nearY, nearZ);
  return aux;
}

short* tps::Image::getPixelVector() {
  return image->getPixelVector();
}

float* tps::Image::getFloatPixelVector() {
  float* vector = (float*)malloc(dimensions_[0]*dimensions_[1]*dimensions_[2]*sizeof(float));
  for (int x = 0; x < dimensions_[0]; x++)
    for (int y = 0; y < dimensions_[1]; y++)
      for (int z = 0; z < dimensions_[2]; z++) {
        int pos = z*dimensions_[0]*dimensions_[1]+y*dimensions_[0]+x;
        vector[pos] = image->getPixelAt(pos);
      }
  return vector;
}

void tps::Image::setPixelVector(short* vector) {
  for (int z = 0; z < dimensions_[2]; z++)
    for (int x = 0; x < dimensions_[0]; x++)
      for (int y = 0; y < dimensions_[1]; y++) {
        short newValue = vector[z*dimensions_[1]*dimensions_[0]+y*dimensions_[0]+x];
        changePixelAt(x, y, z, newValue);
      }
}

float tps::Image::radialSum(int r, std::vector<float> point) {
  float sum = 0.0;
  int cX = point[0];
  int cY = point[1];
  int cZ = point[2];
  for (int x = (cX-r); x < (cX+r); x++)
    for (int y = (cY-r); y < (cY+r); y++)
      for (int z = (cZ-r); z < (cZ+r); z++) {
        sum += 1.0*getPixelAt(x, y, z);
      }
  return sum;
}
