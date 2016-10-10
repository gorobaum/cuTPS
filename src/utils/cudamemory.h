#ifndef TPS_CUDAMEMORY_H_
#define TPS_CUDAMEMORY_H_

#include <iostream>

#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_occupancy.h"

#include "image/image.h"

namespace tps {

class CudaMemory {
public:
  static double getGpuMemory();
  static double getUsedGpuMemory();

  void freeMemory();
  double memoryEstimation();
  void allocCudaMemory(tps::Image& image);
  void initialize(std::vector<int> dimensions,
                  std::vector< std::vector<float> > referenceKeypoints);

  float* getSolutionX() { return solutionX; };
  float* getSolutionY() { return solutionY; };
  float* getSolutionZ() { return solutionZ; };

  void setSolutionX(std::vector<float> solution);
  void setSolutionY(std::vector<float> solution);
  void setSolutionZ(std::vector<float> solution);

  float* getKeypointX() { return keypointX; };
  float* getKeypointY() { return keypointY; };
  float* getKeypointZ() { return keypointZ; };

  short* getRegImage() { return regImage; };
  short* getTargetImage() { return targetImage; };
  float* getImagePointsX() { return imagePointsX; };
  float* getImagePointsY() { return imagePointsY; };
  float* getImagePointsZ() { return imagePointsZ; };
  cudaTextureObject_t getTexObj() { return texObj; };

  std::vector<float> getHostSolX();
  std::vector<float> getHostSolY();
  std::vector<float> getHostSolZ();

private:
  void allocCudaSolution();
  void allocCudaKeypoints();
  void allocCudaImagePixels(tps::Image& image);
  void allocCudaImagePoints(tps::Image& image);
  std::vector<float> cudaToHost(float *cudaMemory);
  void allocCudaImagePixelsTexture(tps::Image& image);

  int imageSize;
  int systemDim;
  int numberOfCps;
  cudaArray* cuArray;
  cudaTextureObject_t texObj;
  short *targetImage, *regImage;
  float *imagePointsX, *imagePointsY, *imagePointsZ;
  float *solutionX, *solutionY, *solutionZ;
  float *keypointX, *keypointY, *keypointZ;
  std::vector< std::vector<float> > referenceKeypoints_;
};

} // namespace

#endif
