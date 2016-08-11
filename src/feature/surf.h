#ifndef TPS_FEATURE_SURF_H_
#define TPS_FEATURE_SURF_H_

#include "image.h"
#include "featuredetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

namespace tps {

// code from http://docs.opencv.org/doc/tutorials/features2d/feature_homography/feature_homography.html
class Surf : public FeatureDetector {
public:
	Surf(Image referenceImage, Image targetImage, int minHessian):
		FeatureDetector(referenceImage, targetImage),
		detector(minHessian) {};
	void run(bool createFeatureImage);
	void saveFeatureImage();
private:
	cv::SurfFeatureDetector detector;
	cv::SurfDescriptorExtractor extractor;
	cv::FlannBasedMatcher matcher;
	// Data structures
	cv::Mat descriptors_ref, descriptors_tar;
	std::vector<cv::KeyPoint> keypoints_ref, keypoints_tar;
	std::vector<cv::DMatch> good_matches;

	void detectFeatures();
	void extractDescriptors();
	void matchDescriptors();
};

} // namespace

#endif
