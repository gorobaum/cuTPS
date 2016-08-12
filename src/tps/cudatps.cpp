#include <iostream>
#include <cmath>

#include "cudatps.h"
#include "cutps.h"

#define MAXTHREADPBLOCK 1024

float* tps::CudaTPS::solutionPointer(std::vector<float> solution) {
  float* vector = (float*)malloc(solution.size()*sizeof(float));
  for(int i = 0; i < solution.size(); i++)
    vector[i] = solution[i];
  return vector;
}

tps::Image tps::CudaTPS::run() {
  lienarSolver.solveLinearSystems(cm_);

  // cm_.setSolutionX(lienarSolver.getSolutionX());
  // cm_.setSolutionY(lienarSolver.getSolutionY());
  // cm_.setSolutionZ(lienarSolver.getSolutionZ());

  short *regImage = runTPSCUDA(cm_, dimensions_, referenceKeypoints_.size());

  registredImage.setPixelVector(regImage);
  
  delete(regImage);
  return registredImage;
}