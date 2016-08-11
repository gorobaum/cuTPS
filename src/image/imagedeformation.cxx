#include <cmath>
#include <iostream>

#include "imagedeformation.h"
#include "itkimagehandler.h"

void tps::ImageDeformation::apply3DSinDeformation() {
  std::vector<int> dimensions = image_.getDimensions();
  for(int x = 0; x < dimensions[0]; x++)
    for(int y = 0; y < dimensions[1]; y++)
      for(int z = 0; z < dimensions[2]; z++) {
        std::vector<float> newPoint = newPointSinDef(x, y, z);
        short newVoxel = image_.trilinearInterpolation(newPoint[0], newPoint[1], newPoint[2]);
        result.changePixelAt(x, y, z, newVoxel);
      }
}

std::vector<float> tps::ImageDeformation::newPointSinDef(int x, int y, int z) {
  std::vector<float> newPoint;
  float newX = x + 2.0*std::sin(y/32.0) - 2.0*std::cos(z/16.0);
  float newY = y - 4.0*std::cos(z/8.0) + 8.0*std::sin(x/32.0);
  float newZ = z + 2.0*std::sin(x/16.0) - 4.0*std::cos(y/16.0);
  newPoint.push_back(newX);
  newPoint.push_back(newY);
  newPoint.push_back(newZ);
  return newPoint;
}