#ifndef TPS_FEATURE_FEATUREDETECTOR_H_
#define TPS_FEATURE_FEATUREDETECTOR_H_

#include "image/image.h"

namespace tps {

// code from http://docs.opencv.org/doc/tutorials/features2d/feature_homography/feature_homography.html
class FeatureDetector
{
public:
	FeatureDetector(Image referenceImage, Image targetImage):
		referenceImage_(referenceImage),
		targetImage_(targetImage) {};
  virtual void run() = 0;
  std::vector< std::vector<float> > getReferenceKeypoints() {return referenceKeypoints;};
  std::vector< std::vector<float> > getTargetKeypoints() {return targetKeypoints;};
protected:
	Image referenceImage_;
	Image targetImage_;
	// Data structures
	std::vector< std::vector<float> > referenceKeypoints;
  std::vector< std::vector<float> > targetKeypoints;
};

} // namespace

#endif
