#ifndef TPS_TPS_CUTPS_H_
#define TPS_TPS_CUTPS_H_

#include <vector>

#include "utils/cudamemory.h"

int getBlockSize();

short* runTPSCUDA(tps::CudaMemory cm, std::vector<int> dimensions,
                  int numberOfCPs, bool occupancy, bool twoDim,
                  int blockSize);

short* runTPSCUDAWithText(tps::CudaMemory cm, std::vector<int> dimensions,
                          int numberOfCPs, bool occupancy, bool twoDim,
                          int blockSize);

short* runTPSCUDAWithoutInterpolation(tps::CudaMemory cm, short* imageVoxels,
          std::vector<int> dimensions, int numberOfCPs, bool occupancy,
          bool twoDim, int blockSize);

short* runTPSCUDAVectorFieldTest(tps::CudaMemory cm, short* imageVoxels,
                                      std::vector<int> dimensions,
                                      int numberOfCPs, bool occupancy,
                                      bool twoDim, int blockSize);

void runTPSRadialDiff(tps::CudaMemory cm, tps::Image regImage,
    tps::Image referenceImage, std::vector<std::vector<float> >referenceKeypoints,
    tps::Image targetImage, std::vector<std::vector<float> > targetKeypoints,
    bool occupancy, bool twoDim, int blockSize);

#endif
