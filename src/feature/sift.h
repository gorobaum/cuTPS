#ifndef TPS_FEATURE_SIFT_H_
#define TPS_FEATURE_SIFT_H_

#include "image/image.h"
#include "featuredetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/nonfree/features2d.hpp>

namespace tps {

// code from http://docs.opencv.org/doc/tutorials/features2d/feature_homography/feature_homography.html
class Sift {
public:
  Sift(cv::Mat referenceImage, cv::Mat targetImage, float distanceMetric, int nFeatures, int nOctavesLayers,
       double contrastThreshold, double edgeThreshold, double sigma):
    referenceImage_(referenceImage),
    targetImage_(targetImage),
    distanceMetric_(distanceMetric),
    sift(nFeatures, nOctavesLayers, contrastThreshold, edgeThreshold, sigma) {};
  void run();
  void drawKeypointsImage(cv::Mat tarImg, std::string filename);
  void drawFeatureImage(cv::Mat refImg, cv::Mat tarImg, std::string filename);
  std::vector< std::vector<float> > getReferenceKeypoints() {return referenceKeypoints;};
  std::vector< std::vector<float> > getTargetKeypoints() {return targetKeypoints;};
  void addRefKeypoints(std::vector< std::vector< float > > newKPs);
  void addTarKeypoints(std::vector< std::vector< float > > newKPs);
  void addNewMatches(int numbersOfNewKPs);
private:
  cv::Mat referenceImage_;
  cv::Mat targetImage_;
  float distanceMetric_;
  cv::SIFT sift;
  cv::FlannBasedMatcher matcher;
  // Data structures
  cv::Mat descriptors_ref, descriptors_tar;
  std::vector<cv::KeyPoint> keypoints_ref, keypoints_tar;
  std::vector<cv::DMatch> good_matches;

  std::vector< std::vector<float> > referenceKeypoints;
  std::vector< std::vector<float> > targetKeypoints;

  void runSift();
  void matchDescriptors();
  void addKeypoints(std::vector<cv::KeyPoint> &keypoints, std::vector< std::vector< float > > newKPs);
};

} // namespace

#endif
