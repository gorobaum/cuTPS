#ifndef TPS_FEATURE_FEATUREFACTORY_H_
#define TPS_FEATURE_FEATUREFACTORY_H_

#include "featuredetector.h"

namespace tps {

class FeatureGenerator : public FeatureDetector {
public:
  FeatureGenerator(Image referenceImage, Image targetImage, float percentage, std::vector< std::vector< int > > boundaries):
    FeatureDetector(referenceImage, targetImage),
    percentage_(percentage),
    boundaries_(boundaries) {};
  void run();
private:
  float percentage_;
  std::vector< std::vector < int > > boundaries_;
  int gridSizeX, gridSizeY, gridSizeZ;
  float xStep, yStep, zStep;
  std::vector<float> applyXRotationalDeformationTo(float x, float y, float z, float ang);
  std::vector<float> applySinDeformationTo(float x, float y, float z);
  void createReferenceImageFeatures();
  void createTargetImageFeatures();
  void createUniformFeatures();
  bool checkBoundary(float x, float y, float z);
};

} //namespace

#endif
