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
  void freeMemory();
  double memoryEstimation();
  double getUsedGpuMemory();
  void allocCudaMemory(tps::Image& image);
  void initialize(std::vector<int> dimensions,
                        std::vector< std::vector<float> > referenceKeypoints,
                        Image targetImage);

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

  std::vector<float> getHostSolX();
  std::vector<float> getHostSolY();
  std::vector<float> getHostSolZ();

private:
  void allocCudaSolution();
  void allocCudaKeypoints();
  void allocCudaImagePixels(tps::Image& image);
  std::vector<float> cudaToHost(float *cudaMemory);

  int imageSize;
  int systemDim;
  int numberOfCps;
  short *targetImage, *regImage;
  float *solutionX, *solutionY, *solutionZ;
  float *keypointX, *keypointY, *keypointZ;
  std::vector< std::vector<float> > referenceKeypoints_;
};

} // namespace

#endif
