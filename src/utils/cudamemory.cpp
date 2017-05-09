#include "cudamemory.h"

#include <cassert>
#include <cstring>
#include <armadillo>

#include "globalconfiguration.h"

inline
cudaError_t checkCuda(cudaError_t result)
{
    if (result != cudaSuccess) {
        std::cout << "CUDA Runtime Error: \n" << cudaGetErrorString(result) << std::endl;
        assert(result == cudaSuccess);
    }
    return result;
}

void tps::CudaMemory::initialize(std::vector<int> dimensions,
                      std::vector< std::vector<float> > referenceKeypoints) {
    imageSize = dimensions[0]*dimensions[1]*dimensions[2];
    referenceKeypoints_ = referenceKeypoints;
    numberOfCps = referenceKeypoints.size();
    systemDim = numberOfCps + 4;
}

void tps::CudaMemory::allocCudaMemory(tps::Image& image) {
    arma::wall_clock timer;
    allocCudaSolution();
    timer.tic();
    allocCudaKeypoints();
    bool texture = GlobalConfiguration::getInstance().getBoolean("imageTexture");
    bool cpuInterpolation = GlobalConfiguration::getInstance().getBoolean("cpuInterpolation");
    bool checkError = GlobalConfiguration::getInstance().getBoolean("checkError");
    bool radialDiff = GlobalConfiguration::getInstance().getBoolean("radialDiff");
    if (texture) {
        allocCudaImagePixelsTexture(image);
    } else if (cpuInterpolation || checkError) {
        allocCudaImagePoints(image);
    } else {
        allocCudaImagePixels(image);
    }

    if (radialDiff) {
        allocCudaImagePoints(image);
        allocCudaImagePixels(image);
    }

    double time = timer.toc();
    std::cout << "Memory copy time: " << time << std::endl;
}

void tps::CudaMemory::allocCudaSolution() {
    checkCuda(cudaMalloc(&solutionX, systemDim*sizeof(float)));
    checkCuda(cudaMalloc(&solutionY, systemDim*sizeof(float)));
    checkCuda(cudaMalloc(&solutionZ, systemDim*sizeof(float)));
}

void tps::CudaMemory::allocCudaKeypoints() {
    float* hostKeypointX = (float*)malloc(referenceKeypoints_.size()*sizeof(float));
    float* hostKeypointY = (float*)malloc(referenceKeypoints_.size()*sizeof(float));
    float* hostKeypointZ = (float*)malloc(referenceKeypoints_.size()*sizeof(float));
    for (uint i = 0; i < referenceKeypoints_.size(); i++) {
        hostKeypointX[i] = referenceKeypoints_[i][0];
        hostKeypointY[i] = referenceKeypoints_[i][1];
        hostKeypointZ[i] = referenceKeypoints_[i][2];
    }

    checkCuda(cudaMalloc(&keypointX, numberOfCps*sizeof(float)));
    checkCuda(cudaMemcpy(keypointX, hostKeypointX, numberOfCps*sizeof(float), cudaMemcpyHostToDevice));

    checkCuda(cudaMalloc(&keypointY, numberOfCps*sizeof(float)));
    checkCuda(cudaMemcpy(keypointY, hostKeypointY, numberOfCps*sizeof(float), cudaMemcpyHostToDevice));

    checkCuda(cudaMalloc(&keypointZ, numberOfCps*sizeof(float)));
    checkCuda(cudaMemcpy(keypointZ, hostKeypointZ, numberOfCps*sizeof(float), cudaMemcpyHostToDevice));

    free(hostKeypointX);
    free(hostKeypointY);
    free(hostKeypointZ);
}

void tps::CudaMemory::allocCudaImagePixelsTexture(tps::Image& image) {
    std::vector<int> dimensions = image.getDimensions();

    cudaExtent volumeExtent = make_cudaExtent(dimensions[0], dimensions[1], dimensions[2]);

    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);

    cudaMalloc3DArray(&cuArray, &channelDesc, volumeExtent, 0);

    cudaMemcpy3DParms copyParams = {0};
    copyParams.srcPtr   = make_cudaPitchedPtr((void*)image.getFloatPixelVector(), volumeExtent.width*sizeof(float), volumeExtent.width, volumeExtent.height);
    copyParams.dstArray = cuArray;
    copyParams.extent   = volumeExtent;
    copyParams.kind     = cudaMemcpyHostToDevice;
    cudaMemcpy3D(&copyParams);

    struct cudaResourceDesc resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.resType = cudaResourceTypeArray;
    resDesc.res.array.array = cuArray;

    struct cudaTextureDesc texDesc;
    memset(&texDesc, 0, sizeof(texDesc));
    texDesc.filterMode       = cudaFilterModeLinear;

    // Create texture object
    texObj = 0;
    cudaCreateTextureObject(&texObj, &resDesc, &texDesc, NULL);

    checkCuda(cudaMalloc(&regImage, imageSize*sizeof(short)));
}

void tps::CudaMemory::allocCudaImagePixels(tps::Image& image) {
    checkCuda(cudaMalloc(&targetImage, imageSize*sizeof(short)));
    checkCuda(cudaMemcpy(targetImage, image.getPixelVector(), imageSize*sizeof(short), cudaMemcpyHostToDevice));
    checkCuda(cudaMalloc(&regImage, imageSize*sizeof(short)));
}

void tps::CudaMemory::allocCudaImagePoints(tps::Image& image) {
    int numDim = image.numberOfDimension();
    checkCuda(cudaMalloc(&imagePointsX, imageSize*sizeof(float)));
    checkCuda(cudaMalloc(&imagePointsY, imageSize*sizeof(float)));
    checkCuda(cudaMalloc(&imagePointsZ, imageSize*sizeof(float)));
}

std::vector<float> tps::CudaMemory::getHostSolX() {
    return cudaToHost(solutionX);
}

std::vector<float> tps::CudaMemory::getHostSolY() {
    return cudaToHost(solutionY);
}

std::vector<float> tps::CudaMemory::getHostSolZ() {
    return cudaToHost(solutionZ);
}

float* vectorToPointer(std::vector<float> input) {
    float* output = (float*)malloc(input.size()*sizeof(float));
    for (int i = 0; i < input.size(); i++)
        output[i] = input[i];
    return output;
}

void tps::CudaMemory::setSolutionX(std::vector<float> solution) {
  float* solPointer = vectorToPointer(solution);
  checkCuda(cudaMemcpy(solutionX, solPointer, systemDim*sizeof(float), cudaMemcpyHostToDevice));
  free(solPointer);
}

void tps::CudaMemory::setSolutionY(std::vector<float> solution) {
  float* solPointer = vectorToPointer(solution);
  checkCuda(cudaMemcpy(solutionY, solPointer, systemDim*sizeof(float), cudaMemcpyHostToDevice));
  free(solPointer);
}

void tps::CudaMemory::setSolutionZ(std::vector<float> solution) {
  float* solPointer = vectorToPointer(solution);
  checkCuda(cudaMemcpy(solutionZ, solPointer, systemDim*sizeof(float), cudaMemcpyHostToDevice));
  free(solPointer);
}

std::vector<float> tps::CudaMemory::cudaToHost(float *cudaMemory) {
    float *hostSolPointer = (float*)malloc(systemDim*sizeof(float));
    cudaMemcpy(hostSolPointer, cudaMemory, systemDim*sizeof(float), cudaMemcpyDeviceToHost);
    std::vector<float> hostSol;
    for (int i =0; i < systemDim; i++)
      hostSol.push_back(hostSolPointer[i]);
    delete(hostSolPointer);
    return hostSol;
}

double tps::CudaMemory::getGpuMemory() {
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    size_t gpuGlobalMem = deviceProp.totalGlobalMem;
    return static_cast<double>(gpuGlobalMem/(1024.0*1024.0));
}

double tps::CudaMemory::getUsedGpuMemory() {
    size_t avail;
    size_t total;
    cudaMemGetInfo( &avail, &total );
    size_t used = (total - avail)/(1024*1024);
    return used;
}

double tps::CudaMemory::memoryEstimation() {
  int intSize = sizeof(int);
  int floatSize = sizeof(float);
  int ucharSize = sizeof(short);
  int doubleSize = sizeof(double);

  int sysDim = numberOfCps+4;

  double solutionsMemory = 3.0*systemDim*floatSize/(1024*1024);
  // std::cout << "solutionsMemory = " << solutionsMemory << std::endl;
  double keypointsMemory = 3.0*numberOfCps*floatSize/(1024*1024);
  // std::cout << "keypointsMemory = " << keypointsMemory << std::endl;
  bool cpuInterpolation = GlobalConfiguration::getInstance().getBoolean("cpuInterpolation");
  double pixelsMemory;
  if (cpuInterpolation) {
    pixelsMemory = 3.0*imageSize*floatSize/(1024*1024);
  } else {
    pixelsMemory = 2.0*imageSize*floatSize/(1024*1024);
  }

  // std::cout << "pixelsMemory = " << pixelsMemory << std::endl;
  double solverMemory = (sysDim*sysDim*doubleSize +
                        sysDim*doubleSize +
                        65.0*sysDim*doubleSize)/(1024*1024);
  std::cout << "sysDim = " << sysDim << std::endl;
  std::cout << "solverMemory = " << solverMemory << std::endl;

  double totalMemory = solutionsMemory+keypointsMemory+pixelsMemory+solverMemory;
  return totalMemory;
}

void tps::CudaMemory::freeMemory() {
  cudaFree(solutionX);
  cudaFree(solutionY);
  cudaFree(solutionZ);
  cudaFree(keypointX);
  cudaFree(keypointY);
  cudaFree(keypointZ);
  cudaFree(targetImage);
  cudaFree(regImage);
}
