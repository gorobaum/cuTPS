#ifndef TPS_TPS_PARALLELTPS_H_
#define TPS_TPS_PARALLELTPS_H_

#include "tps.h"
#include "linearsystem/armalinearsystems.h"
#include "linearsystem/cudalinearsystems.h"
#include "utils/cudamemory.h"

#include <thread>

namespace tps {

class ParallelTPS : public TPS {
public:
  ParallelTPS(std::vector< std::vector<float> > referenceKeypoints, std::vector< std::vector<float> > targetKeypoints,
              tps::Image targetImage, bool twoDimension) :
    TPS(referenceKeypoints, targetKeypoints, targetImage),
    lienarSolver(referenceKeypoints, targetKeypoints, twoDimension) {};
  tps::Image run();
private:
  tps::ArmaLinearSystems lienarSolver;
  uint numberOfThreads = std::thread::hardware_concurrency();
  std::vector<float> solutionX;
  std::vector<float> solutionY;
  std::vector<float> solutionZ;
  void runThread(uint tid);
};

} // namespace

#endif
