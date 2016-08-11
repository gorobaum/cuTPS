#include "cudalinearsystems.h"

#include "cusolver_common.h"
#include "cusolverDn.h"

#include <armadillo>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <cublas_v2.h>

inline
cudaError_t checkCuda(cudaError_t result)
{
    if (result != cudaSuccess) {
        std::cout << "CUDA Runtime Error: \n" << cudaGetErrorString(result) << std::endl;
        assert(result == cudaSuccess);
    }
    return result;
}


void tps::CudaLinearSystems::solveLinearSystems(tps::CudaMemory& cm) {
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

  solveLinearSystem(CLSbx, solutionX);
  solveLinearSystem(CLSby, solutionY);
  solveLinearSystem(CLSbz, solutionZ);
  double time = timer.toc();
  std::cout << "Cuda solver execution time: " << time << std::endl;

  if (twoDimension_)
    adaptSolutionTo3D();
  cm.setSolutionX(solutionX);
  cm.setSolutionY(solutionY);
  cm.setSolutionZ(solutionZ);

  freeResources();
}

void tps::CudaLinearSystems::solveLinearSystem(double *B, std::vector<float>& solution) {
  int lwork = 0;
  int info_gpu = 0;

  const int nrhs = 1;
  const double one = 1;

  double *cudaA = NULL;
  double *d_work = NULL;
  double *d_tau = NULL;
  double *cudaSolution = NULL;
  double *hostSolution = (double*)malloc(systemDimension*sizeof(double));
  int *devInfo = NULL;

  cusolverStatus_t cusolver_status;
  cublasStatus_t cublas_status;
  cudaError_t cudaStat;
  cusolverDnHandle_t handle;
  cublasHandle_t cublasH;
  cusolverDnCreate(&handle);
  cublasCreate(&cublasH);

  // step 1: copy A and B to device
  checkCuda(cudaMalloc(&cudaA, systemDimension*systemDimension*sizeof(double)));
  checkCuda(cudaMalloc(&cudaSolution, systemDimension*sizeof(double)));

  checkCuda(cudaMemcpy(cudaA, CLSA, systemDimension*systemDimension*sizeof(double), cudaMemcpyHostToDevice));
  checkCuda(cudaMemcpy(cudaSolution, B, systemDimension*sizeof(double), cudaMemcpyHostToDevice));

  checkCuda(cudaMalloc((void**)&d_tau, sizeof(double) * systemDimension));
  checkCuda(cudaMalloc((void**)&devInfo, sizeof(int)));

  // step 2: query working space of geqrf and ormqr
  cusolver_status = cusolverDnDgeqrf_bufferSize(handle, systemDimension, systemDimension, cudaA, systemDimension, &lwork);

  checkCuda(cudaMalloc((void**)&d_work, sizeof(double) * lwork));

  // step 3: compute QR factorization
  cusolver_status = cusolverDnDgeqrf(handle, systemDimension, systemDimension, cudaA, systemDimension, d_tau, d_work, lwork, devInfo);
  cudaDeviceSynchronize();
  
  // step 4: compute Q^T*B
  cusolver_status = cusolverDnDormqr(handle, CUBLAS_SIDE_LEFT, CUBLAS_OP_T, systemDimension, nrhs, 
    systemDimension, cudaA, systemDimension, d_tau, cudaSolution, systemDimension, d_work, lwork, devInfo);
  cudaDeviceSynchronize();

  // step 5: compute x = R \ Q^T*B
  cublas_status = cublasDtrsm(cublasH, CUBLAS_SIDE_LEFT, CUBLAS_FILL_MODE_UPPER, CUBLAS_OP_N, 
    CUBLAS_DIAG_NON_UNIT, systemDimension, nrhs, &one, cudaA, systemDimension, cudaSolution, 
    systemDimension);
  cudaMemcpy(hostSolution, cudaSolution, systemDimension*sizeof(double), cudaMemcpyDeviceToHost);
  cudaDeviceSynchronize();

  for (int i = 0; i < systemDimension; i++)
    solution.push_back(hostSolution[i]);

  checkCuda(cudaFree(cudaA));
  checkCuda(cudaFree(cudaSolution));
  checkCuda(cudaFree(d_tau));
  checkCuda(cudaFree(devInfo));
  checkCuda(cudaFree(d_work));
  delete(hostSolution);

  cublasDestroy(cublasH);   
  cusolverDnDestroy(handle);
}

void tps::CudaLinearSystems::transferMatrixA() {
  CLSA = (double*)malloc(systemDimension*systemDimension*sizeof(double));

  for (uint i = 0; i < systemDimension; i++)
    for (uint j = 0; j < systemDimension; j++)
      CLSA[i*systemDimension+j] = matrixA[i][j];
}

void tps::CudaLinearSystems::transferBs() {
  CLSbx = (double*)malloc(systemDimension*sizeof(double));
  CLSby = (double*)malloc(systemDimension*sizeof(double));
  CLSbz = (double*)malloc(systemDimension*sizeof(double));
  for (uint i = 0; i < systemDimension; i++) {
    CLSbx[i] = bx[i];
    CLSby[i] = by[i];
    CLSbz[i] = bz[i];
  }
}

std::vector<float> tps::CudaLinearSystems::pointerToVector(double *pointer) {
  std::vector<float> vector;
  for (int i = 0; i < systemDimension; i++) {
    vector.push_back(pointer[i]);
  }
  return vector;
}

void tps::CudaLinearSystems::freeResources() {
  free(CLSA);
  free(CLSbx);
  free(CLSby);
  free(CLSbz);
}