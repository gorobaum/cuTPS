#include "featuregenerator.h"

#include <iostream>
#include <vector>
#include <cmath>

#define PI 3.14159265
#define ANG PI/72.0

bool tps::FeatureGenerator::checkBoundary(float x, float y, float z) {
  if (boundaries_.empty()) return true;
  if (x <= boundaries_[0][0] || x >= boundaries_[0][1])
    if (y <= boundaries_[1][0] || y >= boundaries_[1][1])
      if (z <= boundaries_[2][0] || z >= boundaries_[2][0])
        return true;
  return false;
}

void tps::FeatureGenerator::run() {
  std::vector<int> dimensions = referenceImage_.getDimensions();
  gridSizeX = dimensions[0]*percentage_ + 1;
  gridSizeY = dimensions[1]*percentage_ + 1;
  gridSizeZ = dimensions[2]*percentage_ + 1;
  xStep = dimensions[0]*1.0/(std::max(1, gridSizeX-1));
  yStep = dimensions[1]*1.0/(std::max(1, gridSizeY-1));
  zStep = dimensions[2]*1.0/(std::max(1, gridSizeZ-1));
  std::cout << "gridSizeX = " << gridSizeX << std::endl;
  std::cout << "gridSizeY = " << gridSizeY << std::endl;
  std::cout << "gridSizeZ = " << gridSizeZ << std::endl;
  std::cout << "xStep = " << xStep << std::endl;
  std::cout << "yStep = " << yStep << std::endl;
  std::cout << "zStep = " << zStep << std::endl;
  createReferenceImageFeatures();
  createTargetImageFeatures();
}

void tps::FeatureGenerator::createUniformFeatures() {
    for (int z = 0; z < gridSizeZ; z++)
      for (int x = 0; x < gridSizeX; x++)
        for (int y = 0; y < gridSizeY; y++) {
          if(checkBoundary(x*xStep, y*yStep, z*zStep)) {
            std::vector<float> newCP;
            newCP.push_back(x*xStep);
            newCP.push_back(y*yStep);
            newCP.push_back(z*zStep);
            referenceKeypoints.push_back(newCP);
            targetKeypoints.push_back(newCP);
          }
      }
}

void tps::FeatureGenerator::createReferenceImageFeatures() {
    for (int z = 0; z < gridSizeZ; z++)
      for (int x = 0; x < gridSizeX; x++)
        for (int y = 0; y < gridSizeY; y++) {
        std::vector<float> newCP;
        newCP.push_back(x*xStep);
        newCP.push_back(y*yStep);
        newCP.push_back(z*zStep);
        referenceKeypoints.push_back(newCP);
      }
}

void tps::FeatureGenerator::createTargetImageFeatures() {
  int pos = 0;
  for (int z = 0; z < gridSizeZ; z++)
    for (int x = 0; x < gridSizeX; x++) 
      for (int y = 0; y < gridSizeY; y++) {
        std::vector<float> referenceCP = referenceKeypoints[pos];
        std::vector<float> newPoint = applySinDeformationTo(referenceCP[0], referenceCP[1], referenceCP[2]);
        targetKeypoints.push_back(newPoint);
        pos++;
      }
}

std::vector<float> tps::FeatureGenerator::applySinDeformationTo(float x, float y, float z) {
  std::vector<float> newPoint;
  // newX = x - 2.0*std::sin(y/32.0) + 2.0*std::cos(z/16.0);
  float newX = x - 2.0*std::sin(y/32.0) + 2.0*std::cos(z/16.0);
  // newY = y + 4.0*std::cos(z/8.0) - 8.0*std::sin(x/32.0);
  float newY = y + 4.0*std::cos(z/8.0) - 8.0*std::sin(x/32.0);
  // newZ = z - 2.0*std::sin(x/16.0) + 4.0*std::cos(y/16.0);
  float newZ = z - 2.0*std::sin(x/16.0) + 4.0*std::cos(y/16.0);
  newPoint.push_back(newX);
  newPoint.push_back(newY);
  newPoint.push_back(newZ);
  return newPoint;
}

std::vector<float> tps::FeatureGenerator::applyXRotationalDeformationTo(float x, float y, float z, float ang) {
  std::vector<int> dimensions = referenceImage_.getDimensions();
  float newY = y*std::cos(ang)-z*std::sin(ang);
  float newZ = z*std::cos(ang)+y*std::sin(ang);
  std::vector<float> newPoint;
  newPoint.push_back(x);
  newPoint.push_back(newY);
  newPoint.push_back(newZ);
  return newPoint;
}