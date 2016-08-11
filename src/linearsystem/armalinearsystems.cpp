#include "armalinearsystems.h"

void tps::ArmaLinearSystems::solveLinearSystems() {
  setSysDim();
  if (twoDimension_) {
    createMatrixA2D();
    createBs2D();
  } else {
    createMatrixA3D();
    createBs3D();
  }

  transferMatrixA();
  transferBs();

  arma::wall_clock timer;
  timer.tic();

  solutionX = solveLinearSystem(ALSbx);
  solutionY = solveLinearSystem(ALSby);
  solutionZ = solveLinearSystem(ALSbz);

  if (twoDimension_)
    adaptSolutionTo3D();

  double time = timer.toc();
  std::cout << "Arma solver execution time: " << time << std::endl;

}

std::vector<float> tps::ArmaLinearSystems::solveLinearSystem(arma::vec b) {
  std::vector<float> solution;
  arma::vec armaSol = arma::solve(ALSA, b);

  // std::cout << armaSol << std::endl;

  for (uint i = 0; i < systemDimension; i++)
    solution.push_back(armaSol(i));
  
  return solution;
}

void tps::ArmaLinearSystems::transferMatrixA() {
  ALSA = arma::mat(systemDimension, systemDimension, arma::fill::zeros);

  for (uint i = 0; i < systemDimension; i++)
    for (uint j = 0; j < systemDimension; j++) {
      ALSA(i,j) = matrixA[i][j];
    }
}

void tps::ArmaLinearSystems::transferBs() {
  ALSbx = arma::vec(systemDimension, arma::fill::zeros);
  ALSby = arma::vec(systemDimension, arma::fill::zeros);
  ALSbz = arma::vec(systemDimension, arma::fill::zeros);
  for (uint i = 0; i < systemDimension; i++) {
    ALSbx(i) = bx[i];
    ALSby(i) = by[i];
    ALSbz(i) = bz[i];
  }
}