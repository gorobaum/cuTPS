#ifndef TPS_TPS_CUDATPS_H_
#define TPS_TPS_CUDATPS_H_

#include "tps.h"
#include "linearsystem/cudalinearsystems.h"
#include "linearsystem/armalinearsystems.h"
#include "utils/cudamemory.h"

namespace tps {

class CudaTPS : public TPS {
  public:
    CudaTPS(std::vector< std::vector<float> > referenceKeypoints, std::vector< std::vector<float> > targetKeypoints,
            tps::Image targetImage, tps::CudaMemory& cm, bool twoDimension) :
      TPS(referenceKeypoints, targetKeypoints, targetImage),
      lienarSolver(referenceKeypoints, targetKeypoints, twoDimension),
      cm_(cm) {};
    tps::Image run();
  private:
    tps::CudaLinearSystems lienarSolver;
    tps::CudaMemory& cm_;
    float* solutionPointer(std::vector<float> solution);
};

}

#endif
