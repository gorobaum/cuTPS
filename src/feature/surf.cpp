#include "surf.h"

void tps::Surf::detectFeatures() {
	detector.detect(referenceImage_.image, keypoints_ref);
  detector.detect(targetImage_.image, keypoints_tar);
}

void tps::Surf::extractDescriptors() {
  extractor.compute(referenceImage_.image, keypoints_ref, descriptors_ref);
  extractor.compute(targetImage_.image, keypoints_tar, descriptors_tar);
}

void tps::Surf::matchDescriptors() {
	std::vector<cv::DMatch> matches;
  matcher.match(descriptors_ref, descriptors_tar, matches);

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for (int i = 0; i < descriptors_ref.rows; i++) { 
    double dist = matches[i].distance;
    if(dist < min_dist) min_dist = dist;
    if(dist > max_dist) max_dist = dist;
  }

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )

  for (int i = 0; i < descriptors_ref.rows; i++)
    if (matches[i].distance < 3*min_dist)
      good_matches.push_back( matches[i]);

  for (uint i = 0; i < good_matches.size(); i++) {
    //-- Get the keypoints from the good matches
    referenceKeypoints.push_back(keypoints_ref[ good_matches[i].queryIdx ].pt);
    targetKeypoints.push_back(keypoints_tar[ good_matches[i].trainIdx ].pt);
  }
}

void tps::Surf::saveFeatureImage() {
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(95);

  cv::Mat img_matches;
  drawMatches(referenceImage_.image, keypoints_ref, targetImage_.image, keypoints_tar,
              good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
              std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  cv::imwrite("refkeypoints.png", img_matches, compression_params);
}

void tps::Surf::run(bool createFeatureImage) {
	detectFeatures();
	extractDescriptors();
	matchDescriptors();
	if (createFeatureImage) saveFeatureImage();
}