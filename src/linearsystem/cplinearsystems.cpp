#include "cplinearsystems.h"

#include <iostream>

#define Size3D 4
#define Size2D 3

void tps::CPLinearSystems::setSysDim() {
  if (twoDimension_)
    systemDimension = referenceKeypoints_.size()+Size2D;
  else
    systemDimension = referenceKeypoints_.size()+Size3D;
}

void tps::CPLinearSystems::createMatrixA3D() {
  matrixA = std::vector<std::vector<float>>(systemDimension, std::vector<float>(systemDimension, 0.0));

  for (int j = Size3D, i = 0; j < systemDimension; j++, i++) {
    matrixA[j][0] = 1;
    matrixA[j][1] = referenceKeypoints_[i][0];
    matrixA[j][2] = referenceKeypoints_[i][1];
    matrixA[j][3] = referenceKeypoints_[i][2];
    matrixA[0][j] = 1;
    matrixA[1][j] = referenceKeypoints_[i][0];
    matrixA[2][j] = referenceKeypoints_[i][1];
    matrixA[3][j] = referenceKeypoints_[i][2];
  }

  for (int i = 0; i < referenceKeypoints_.size(); i++)
    for (int j = 0; j < referenceKeypoints_.size(); j++) {
      float r = computeRSquared(referenceKeypoints_[i][0], referenceKeypoints_[j][0], 
                                referenceKeypoints_[i][1], referenceKeypoints_[j][1],
                                referenceKeypoints_[i][2], referenceKeypoints_[j][2]);
      if (r != 0.0) matrixA[i+Size3D][j+Size3D] = r*log(r);
    }
}
  
void tps::CPLinearSystems::createBs3D() {
  bx = std::vector<float>(systemDimension, 0.0);
  by = std::vector<float>(systemDimension, 0.0);
  bz = std::vector<float>(systemDimension, 0.0);
  for (int j = Size3D, i = 0; j < systemDimension; i++, j++) {
    bx[j] = targetKeypoints_[i][0];
    by[j] = targetKeypoints_[i][1];
    bz[j] = targetKeypoints_[i][2];
  }
}
  
void tps::CPLinearSystems::createMatrixA2D() {
  matrixA = std::vector<std::vector<float>>(systemDimension, std::vector<float>(systemDimension, 0.0));

  for (int j = Size2D, i = 0; j < systemDimension; j++, i++) {
    matrixA[j][0] = 1;
    matrixA[j][1] = referenceKeypoints_[i][0];
    matrixA[j][2] = referenceKeypoints_[i][1];
    matrixA[0][j] = 1;
    matrixA[1][j] = referenceKeypoints_[i][0];
    matrixA[2][j] = referenceKeypoints_[i][1];
  }

  for (int i = 0; i < referenceKeypoints_.size(); i++)
    for (int j = 0; j < referenceKeypoints_.size(); j++) {
      float r = computeRSquared2D(referenceKeypoints_[i][0], referenceKeypoints_[j][0], 
                                referenceKeypoints_[i][1], referenceKeypoints_[j][1]);
      if (r != 0.0) matrixA[i+Size2D][j+Size2D] = r*log(r);
    }
}
  
void tps::CPLinearSystems::createBs2D() {
  bx = std::vector<float>(systemDimension, 0.0);
  by = std::vector<float>(systemDimension, 0.0);
  bz = std::vector<float>(systemDimension, 0.0);

  for (int j = Size2D, i = 0; j < systemDimension; i++, j++) {
    bx[j] = targetKeypoints_[i][0];
    by[j] = targetKeypoints_[i][1];
  } 
}

void tps::CPLinearSystems::adaptSolutionTo3D() {
  std::vector<float>::iterator itSx = solutionX.begin();
  std::vector<float>::iterator itSy = solutionY.begin();
  std::vector<float>::iterator itSz = solutionZ.begin();

  solutionX.insert(itSx+3, 0.0);
  solutionY.insert(itSy+3, 0.0);
  solutionZ.insert(itSz+3, 0.0);
}