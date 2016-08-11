#ifndef TPS_FEATURE_FEATUREFACTORY_H_
#define TPS_FEATURE_FEATUREFACTORY_H_

#include "featuredetector.h"

namespace tps {

class FeatureGenerator : public FeatureDetector {
public:
    FeatureGenerator(Image referenceImage, Image targetImage, float percentage):
        FeatureDetector(referenceImage, targetImage),
        percentage_(percentage) {};
    void run();

private:
    bool isTwoDimensional();
    void createTargetImageFeatures();
    void createReferenceImageFeatures();
    std::vector<float> applySinDeformationTo(float x, float y, float z);
    std::vector<float> applyXRotationalDeformationTo(float x, float y, float z,
                                                     float ang);

    float percentage_;
    float xStep, yStep, zStep;
    int gridSizeX, gridSizeY, gridSizeZ;
    std::vector< std::vector < int > > boundaries_;
};

} //namespace

#endif
