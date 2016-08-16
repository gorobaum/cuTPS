#ifndef TPS_TPS_CUTPS_H_
#define TPS_TPS_CUTPS_H_

#include <vector>

#include "utils/cudamemory.h"

int getBlockSize();

short* runTPSCUDA(tps::CudaMemory cm, std::vector<int> dimensions, int numberOfCPs, bool occupancy);
short* runTPSCUDAWithText(tps::CudaMemory cm, std::vector<int> dimensions, int numberOfCPs, bool occupancy);

#endif
