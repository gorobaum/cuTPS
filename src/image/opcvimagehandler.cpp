#include "opcvimagehandler.h"

tps::Image tps::OPCVImageHandler::loadImageData(std::string filename) {
  cv::Mat cvTarImg = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

  std::vector<int> dimensions;
  dimensions.push_back(cvTarImg.size().width);
  dimensions.push_back(cvTarImg.size().height);
  dimensions.push_back(1);

  short* vecImage = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));

  for (int col = 0; col < dimensions[0]; col++)
    for (int row = 0; row < dimensions[1]; row++)
      vecImage[col*dimensions[1]+row] = cvTarImg.at<uchar>(row, col);

  tps::Image image(vecImage, dimensions);

  delete(vecImage);

  return image;
}

void tps::OPCVImageHandler::saveImageData(tps::Image resultImage, std::string filename) {
  std::vector<int> compression_params;
  compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  compression_params.push_back(95);

  std::vector<int> dimensions = resultImage.getDimensions();

  cv::Mat savImage = cv::Mat::zeros(dimensions[1], dimensions[0], CV_8U);
  for (int col = 0; col < dimensions[0]; col++)
      for (int row = 0; row < dimensions[1]; row++)
        savImage.at<uchar>(row, col) = (uchar)resultImage.getPixelAt(col, row, 0);

  cv::imwrite(filename.c_str(), savImage, compression_params);
}