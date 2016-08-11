#include "image.h"

void tps::Image::changePixelAt(int x, int y, int z, short value) {
  if (x >= 0 && x < dimensions_[0]-1 && y >= 0 && y < dimensions_[1]-1 && z >= 0 && z <= dimensions_[2]-1)
    image[y+x*dimensions_[1]+z*dimensions_[0]*dimensions_[1]] = value;
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

short tps::Image::getPixelAt(int x, int y, int z) {
  if (x > dimensions_[0]-1 || x < 0)
    return 0;
  else if (y > dimensions_[1]-1 || y < 0)
    return 0;
  else if (z > dimensions_[2]-1 || z < 0)
    return 0;
  else {
    return image[y+x*dimensions_[1]+z*dimensions_[0]*dimensions_[1]];
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

short tps::Image::NNInterpolation(float x, float y, float z) {
  int nearX = getNearestInteger(x);
  int nearY = getNearestInteger(y);
  int nearZ = getNearestInteger(z);
  int aux = getPixelAt(nearX, nearY, nearZ);
  return aux;
}

short* tps::Image::getPixelVector() {
  return image;
}

void tps::Image::setPixelVector(short* vector) {
  for (int z = 0; z < dimensions_[2]; z++)
    for (int x = 0; x < dimensions_[0]; x++)
      for (int y = 0; y < dimensions_[1]; y++) {
        short newValue = vector[z*dimensions_[1]*dimensions_[0]+x*dimensions_[1]+y];
        changePixelAt(x, y, z, newValue);
      }
}
