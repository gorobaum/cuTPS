#ifndef TPS_LINEARSYSTEM_CPLINEARSYSTEMS_H_
#define TPS_LINEARSYSTEM_CPLINEARSYSTEMS_H_

#include <vector>
#include <cmath>

namespace tps {

class CPLinearSystems {
public:
  CPLinearSystems(std::vector< std::vector<float> > referenceKeypoints,
                  std::vector< std::vector<float> > targetKeypoints,
                  bool twoDimension) :
    referenceKeypoints_(referenceKeypoints),
    targetKeypoints_(targetKeypoints),
    twoDimension_(twoDimension) {};
  std::vector<float> getSolutionX() {return solutionX;};
  std::vector<float> getSolutionY() {return solutionY;};
  std::vector<float> getSolutionZ() {return solutionZ;};

protected:
  void setSysDim();
  void createMatrixA3D();
  void createBs3D();
  void createMatrixA2D();
  void createBs2D();
  void adaptSolutionTo3D();
  virtual void transferMatrixA() = 0;
  virtual void transferBs() = 0;
  float computeRSquared(float x, float xi, float y, float yi, float z, float zi)
        {return std::pow(x-xi,2) + std::pow(y-yi,2) + std::pow(z-zi,2);};
  float computeRSquared2D(float x, float xi, float y, float yi)
        {return std::pow(x-xi,2) + std::pow(y-yi,2);};

  bool twoDimension_;
  int systemDimension;
  std::vector<float> bx;
  std::vector<float> by;
  std::vector<float> bz;
  std::vector<float> solutionX;
  std::vector<float> solutionY;
  std::vector<float> solutionZ;
  std::vector< std::vector<float> > matrixA;
  std::vector< std::vector<float> > targetKeypoints_;
  std::vector< std::vector<float> > referenceKeypoints_;
};

} //namespace

#endif
