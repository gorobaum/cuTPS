#ifndef TPS_BASICTPS_H_
#define TPS_BASICTPS_H_

#include "tps.h"
#include "linearsystem/armalinearsystems.h"

namespace tps {

class BasicTPS : public TPS {
public:
  BasicTPS(std::vector< std::vector<float> > referenceKeypoints, std::vector< std::vector<float> > targetKeypoints, 
           tps::Image targetImage, bool twoDimension) :
    TPS(referenceKeypoints, targetKeypoints, targetImage),
    lienarSolver(referenceKeypoints, targetKeypoints, twoDimension) {};	
  tps::Image run();
private:
  tps::ArmaLinearSystems lienarSolver;
  std::vector<float> solutionX;
  std::vector<float> solutionY;
  std::vector<float> solutionZ;
};

} // namespace

#endif