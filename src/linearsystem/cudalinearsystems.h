#ifndef TPS_LINEARSYSTEM_CUDALINEARSYSTEMS_H_
#define TPS_LINEARSYSTEM_CUDALINEARSYSTEMS_H_

#include "cplinearsystems.h"
#include "utils/cudamemory.h"

namespace tps {

class CudaLinearSystems : public CPLinearSystems {
using CPLinearSystems::CPLinearSystems;
public:
  void solveLinearSystems(tps::CudaMemory& cm, std::string mod);

private:
  void solveLinearSystem(double *B, std::vector<float>& solution);
  std::vector<float> pointerToVector(double *pointer);
  void transferMatrixA();
  void transferBs();
  void freeResources();
  double *CLSbx, *CLSby, *CLSbz, *CLSA;
  std::string mod_;
};

} //namepsace

#endif
