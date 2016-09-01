#include "featuregenerator.h"

#include <iostream>
#include <vector>
#include <cmath>

#define PI 3.14159265
#define ANG PI/72.0

void tps::FeatureGenerator::run() {
  std::vector<int> dimensions = referenceImage_.getDimensions();
  xStep = dimensions[0]/(dimensions[0]*percentage_);
  yStep = dimensions[1]/(dimensions[1]*percentage_);
  zStep = dimensions[2]/(dimensions[2]*percentage_);
  if (referenceImage_.isTwoDimensional()) zStep = 0.0;

  // std::cout << "xStep = " << xStep << std::endl;
  // std::cout << "yStep = " << yStep << std::endl;
  // std::cout << "zStep = " << zStep << std::endl;
  createReferenceImageFeatures();
  createTargetImageFeatures();
}

void tps::FeatureGenerator::createReferenceImageFeatures() {
    std::vector<int> dimensions = referenceImage_.getDimensions();
    for (int z = 0; z < dimensions[2]; z+=zStep)
        for (int y = 0; y < dimensions[1]; y+=yStep)
            for (int x = 0; x < dimensions[0]; x+=xStep) {
                std::vector<float> newCP;
                newCP.push_back(x);
                newCP.push_back(y);
                newCP.push_back(z);
                referenceKeypoints.push_back(newCP);
            }
}

void tps::FeatureGenerator::createTargetImageFeatures() {
    for (int pos = 0; pos < referenceKeypoints.size(); pos++) {
        std::vector<float> referenceCP = referenceKeypoints[pos];
        // std::cout << referenceCP[0] << " " << referenceCP[1] << " " << referenceCP[2] << std::endl;
        std::vector<float> newPoint = applySinDeformationTo(referenceCP[0], referenceCP[1], referenceCP[2]);
        // std::cout << newPoint[0] << " " << newPoint[1] << " " << newPoint[2] << std::endl;
        targetKeypoints.push_back(newPoint);
    }
}

std::vector<float> tps::FeatureGenerator::applySinDeformationTo(float x, float y, float z) {
  std::vector<float> newPoint;
  // newX = x - 2.0*std::sin(y/32.0) + 2.0*std::cos(z/16.0);
  float newX = x - 2.0*std::sin(y/32.0);
  // newY = y + 4.0*std::cos(z/8.0) - 8.0*std::sin(x/32.0);
  float newY = y - 8.0*std::sin(x/32.0);
  // newZ = z - 2.0*std::sin(x/16.0) + 4.0*std::cos(y/16.0);
  float newZ;
  if (referenceImage_.isTwoDimensional()) {
    newZ = 0.0;
  } else {
    newX += 2.0*std::cos(z/16.0);
    newY += 4.0*std::sin(z/8.0);
    newZ = z - 2.0*std::sin(x/16.0) + 4.0*std::cos(y/16.0);
  }
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
