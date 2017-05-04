#include <cassert>
#include <sstream>
#include <iostream>

#include <armadillo>

#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_occupancy.h"

#include "cutps.h"

#include <cmath>

typedef struct {
  float x, y, z;
} Point;

cudaError_t checkCuda(cudaError_t result) {
  if (result != cudaSuccess) {
    std::cout << "CUDA Runtime Error: \n" << cudaGetErrorString(result) << std::endl;
    assert(result == cudaSuccess);
  }
  return result;
}

void startTimeRecord(cudaEvent_t *start, cudaEvent_t *stop) {
  checkCuda(cudaEventCreate(start));
  checkCuda(cudaEventCreate(stop));
  checkCuda(cudaEventRecord(*start, 0));
}

void showExecutionTime(cudaEvent_t *start, cudaEvent_t *stop, std::string output) {
  checkCuda(cudaEventRecord(*stop, 0));
  checkCuda(cudaEventSynchronize(*stop));
  float elapsedTime;
  checkCuda(cudaEventElapsedTime(&elapsedTime, *start, *stop));
  checkCuda(cudaEventDestroy(*start));
  checkCuda(cudaEventDestroy(*stop));
  std::cout << output << elapsedTime/1000 << " s\n";
}


//=====================
//KERNEL
//====================


// Kernel definition
__device__ short cudaGetPixel(int x, int y, int z, short* image, int width, int height, int slices){
  if (x > width-1 || x < 0) return 0;
  if (y > height-1 || y < 0) return 0;
  if (z > slices-1 || z < 0) return 0;
  return image[z*height*width+y*width+x];
}

// Kernel definition
__device__ short cudaTrilinearInterpolation(float x, float y, float z, short* image,
    int width, int height, int slices) {
  int u = trunc(x);
  int v = trunc(y);
  int w = trunc(z);

  float xd = (x - u);
  float yd = (y - v);
  float zd = (z - w);

  short c00 = cudaGetPixel(u, v, w, image, width, height, slices)*(1-xd)
    + cudaGetPixel(u+1, v, w, image, width, height, slices)*xd;

  short c10 = cudaGetPixel(u, v+1, w, image, width, height, slices)*(1-xd)
    + cudaGetPixel(u+1, v+1, w, image, width, height, slices)*xd;

  short c01 = cudaGetPixel(u, v, w+1, image, width, height, slices)*(1-xd)
    + cudaGetPixel(u+1, v, w+1, image, width, height, slices)*xd;

  short c11 = cudaGetPixel(u, v+1, w+1, image, width, height, slices)*(1-xd)
    + cudaGetPixel(u+1, v+1, w+1, image, width, height, slices)*xd;

  short c0 = c00*(1-yd)+c10*yd;
  short c1 = c01*(1-yd)+c11*yd;

  short result = c0*(1-zd)+c1*zd;
  if (result < 0) result = 0;
  return result;
}

__device__ Point calculateNewPoint(float* solutionX, float* solutionY,
                            float* solutionZ, float* keyX, float* keyY,
                            float* keyZ, int x, int y, int z, int numOfKeys) {
  Point newPoint;

  newPoint.x = solutionX[0] + x*solutionX[1] + y*solutionX[2] + z*solutionX[3];
  newPoint.y = solutionY[0] + x*solutionY[1] + y*solutionY[2] + z*solutionY[3];
  newPoint.z = solutionZ[0] + x*solutionZ[1] + y*solutionZ[2] + z*solutionZ[3];

  for (int i = 0; i < numOfKeys; i++) {
    float r = (x-keyX[i])*(x-keyX[i]) + (y-keyY[i])*(y-keyY[i]) + (z-keyZ[i])*(z-keyZ[i]);
    if (r != 0.0) {
      newPoint.x += r*log(r) * solutionX[i+4];
      newPoint.y += r*log(r) * solutionY[i+4];
      newPoint.z += r*log(r) * solutionZ[i+4];
    }
  }

  return newPoint;
}

// Kernel definition
__global__ void tpsCuda(short* cudaImage, short* cudaRegImage, float* solutionX, float* solutionY,
    float* solutionZ, int width, int height, int slices, float* keyX, float* keyY,
    float* keyZ, int numOfKeys) {
  int x = blockDim.x*blockIdx.x + threadIdx.x;
  int y = blockDim.y*blockIdx.y + threadIdx.y;
  int z = blockDim.z*blockIdx.z + threadIdx.z;

  Point newPoint = calculateNewPoint(solutionX, solutionY, solutionZ, keyX,
                                     keyY, keyZ, x, y, z, numOfKeys);

  if (x <= width-1 && x >= 0)
    if (y <= height-1 && y >= 0)
      if (z <= slices-1 && z >= 0)
        cudaRegImage[z*height*width+y*width+x] =
              cudaTrilinearInterpolation(newPoint.x, newPoint.y, newPoint.z,
                                         cudaImage, width, height, slices);
}

// Kernel definition
__global__ void tpsCudaWithText(cudaTextureObject_t textObj, short* cudaRegImage, float* solutionX, float* solutionY,
    float* solutionZ, int width, int height, int slices, float* keyX, float* keyY,
    float* keyZ, int numOfKeys) {
  int x = blockDim.x*blockIdx.x + threadIdx.x;
  int y = blockDim.y*blockIdx.y + threadIdx.y;
  int z = blockDim.z*blockIdx.z + threadIdx.z;

  Point newPoint = calculateNewPoint(solutionX, solutionY, solutionZ,
                                     keyX, keyY, keyZ, x, y, z, numOfKeys);

  if (x <= width-1 && x >= 0)
    if (y <= height-1 && y >= 0)
      if (z <= slices-1 && z >= 0)
        cudaRegImage[z*width*height+y*width+x] =
            (short)tex3D<float>(textObj, newPoint.x, newPoint.y, newPoint.z);
}

__global__ void tpsCudaWithoutInterpolation(float* cudapointsx,
    float* cudapointsy, float* cudapointsz, float* solutionx,
    float* solutiony, float* solutionz, int width, int height,
    int slices, float* keyx, float* keyy, float* keyz,
    int numofkeys) {

  int x = blockDim.x*blockIdx.x + threadIdx.x;
  int y = blockDim.y*blockIdx.y + threadIdx.y;
  int z = blockDim.z*blockIdx.z + threadIdx.z;

  Point newPoint = calculateNewPoint(solutionx, solutiony, solutionz,
                                     keyx, keyy, keyz, x, y, z, numofkeys);

  if (x <= width-1 && x >= 0)
    if (y <= height-1 && y >= 0)
      if (z <= slices-1 && z >= 0) {
        cudapointsx[z*height*width+y*width+x] = newPoint.x;
        cudapointsy[z*height*width+y*width+x] = newPoint.y;
        cudapointsz[z*height*width+y*width+x] = newPoint.z;
      }
}

__global__ void tpscudaVectorField(float* cudapointsx, float* cudapointsy, float* cudapointsz, float* solutionx, float* solutiony,
    float* solutionz, int width, int height, int slices, float* keyx, float* keyy,
    float* keyz, int numofkeys) {
  int x = blockDim.x*blockIdx.x + threadIdx.x;
  int y = blockDim.y*blockIdx.y + threadIdx.y;
  int z = blockDim.z*blockIdx.z + threadIdx.z;

  Point newPoint = calculateNewPoint(solutionx, solutiony, solutionz,
                                     keyx, keyy, keyz, x, y, z, numofkeys);

  if (x <= width-1 && x >= 0)
    if (y <= height-1 && y >= 0)
      if (z <= slices-1 && z >= 0) {
        cudapointsx[z*height*width+y*width+x] = newPoint.x - x;
        cudapointsy[z*height*width+y*width+x] = newPoint.y - y;
        cudapointsz[z*height*width+y*width+x] = newPoint.z - z;
      }
}

short getPixel(int x, int y, int z, short* image, std::vector<int> dimensions) {
  if (x > dimensions[0]-1 || x < 0) return 0;
  if (y > dimensions[1]-1 || y < 0) return 0;
  if (z > dimensions[2]-1 || z < 0) return 0;
  return image[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x];
}

short trilinearInterpolation(float x, float y, float z, short* image, std::vector<int> dimensions) {
  int u = trunc(x);
  int v = trunc(y);
  int w = trunc(z);

  float xd = (x - u);
  float yd = (y - v);
  float zd = (z - w);

  short c00 = getPixel(u, v, w, image, dimensions)*(1-xd)
    + getPixel(u+1, v, w, image, dimensions)*xd;

  short c10 = getPixel(u, v+1, w, image, dimensions)*(1-xd)
    + getPixel(u+1, v+1, w, image, dimensions)*xd;

  short c01 = getPixel(u, v, w+1, image, dimensions)*(1-xd)
    + getPixel(u+1, v, w+1, image, dimensions)*xd;

  short c11 = getPixel(u, v+1, w+1, image, dimensions)*(1-xd)
    + getPixel(u+1, v+1, w+1, image, dimensions)*xd;

  short c0 = c00*(1-yd)+c10*yd;
  short c1 = c01*(1-yd)+c11*yd;

  short result = c0*(1-zd)+c1*zd;
  if (result < 0) result = 0;
  return result;
}

int getBlockSize(int maxBlockSize) {
  int maxOccupancyBlockSize = 0;
  float maxOccupancy = 0.0;
  int device;
  cudaDeviceProp prop;
  cudaGetDevice(&device);
  cudaGetDeviceProperties(&prop, device);

  for (int blockSize = 32; blockSize <= maxBlockSize; blockSize += 32) {
    int numBlocks;        // Occupancy in terms of active blocks

    cudaOccupancyMaxActiveBlocksPerMultiprocessor(
        &numBlocks,
        tpsCuda,
        blockSize,
        0);

    int activeWarps = numBlocks * blockSize / prop.warpSize;
    int maxWarps = prop.maxThreadsPerMultiProcessor / prop.warpSize;
    float currentOccupancy = 1.0*activeWarps/maxWarps;
    if (currentOccupancy >= maxOccupancy ) {
      maxOccupancy = currentOccupancy;
      maxOccupancyBlockSize = blockSize;
    }
  }
  return maxOccupancyBlockSize;
}

dim3 calculateBestThreadsPerBlock(int blockSize, bool twoDim) {
  dim3 threadsPerBlock;
  std::vector<int> threadsPerDim(3, 1);
  int divisor = 8;
  int imageDimension;
  if (twoDim) {
    imageDimension = 2;
  } else {
    imageDimension = 3;
  }

  for (int i = 0; divisor > 1;) {
    if (blockSize%divisor == 0) {
      threadsPerDim[i%imageDimension] *= divisor;
      blockSize /= divisor;
      i++;
    } else {
      divisor /= 2;
    }
  }

  threadsPerBlock.x = threadsPerDim[0];
  threadsPerBlock.y = threadsPerDim[1];
  threadsPerBlock.z = threadsPerDim[2];

  return threadsPerBlock;
}

short* runTPSCUDA(tps::CudaMemory cm, std::vector<int> dimensions, int numberOfCPs, bool occupancy, bool twoDim, int blockSize) {
  dim3 threadsPerBlock;

  if (occupancy) {
    int maxBlockSize = getBlockSize(blockSize);
    threadsPerBlock = calculateBestThreadsPerBlock(maxBlockSize, twoDim);
  } else {
    threadsPerBlock.x = 8;
    threadsPerBlock.y = 8;
    if (twoDim) {
      threadsPerBlock.z = 1;
    } else {
      threadsPerBlock.z = 8;
    }
  }
  std::cout << "threadsPerBlock.x = " << threadsPerBlock.x << std::endl;
  std::cout << "threadsPerBlock.y = " << threadsPerBlock.y << std::endl;
  std::cout << "threadsPerBlock.z = " << threadsPerBlock.z << std::endl;

  dim3 numBlocks(std::ceil(1.0*dimensions[0]/threadsPerBlock.x),
      std::ceil(1.0*dimensions[1]/threadsPerBlock.y),
      std::ceil(1.0*dimensions[2]/threadsPerBlock.z));

  short* regImage = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));

  for (int slice = 0; slice < dimensions[2]; slice++)
    for (int col = 0; col < dimensions[0]; col++)
      for (int row = 0; row < dimensions[1]; row++)
        regImage[slice*dimensions[1]*dimensions[0]+col*dimensions[1]+row] = 0;

  cudaEvent_t start, stop;
  startTimeRecord(&start, &stop);

  tpsCuda<<<numBlocks, threadsPerBlock>>>(cm.getTargetImage(), cm.getRegImage(), cm.getSolutionX(), cm.getSolutionY(),
      cm.getSolutionZ(), dimensions[0], dimensions[1], dimensions[2], cm.getKeypointX(),
      cm.getKeypointY(), cm.getKeypointZ(), numberOfCPs);
  checkCuda(cudaDeviceSynchronize());
  checkCuda(cudaMemcpy(regImage, cm.getRegImage(), dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short), cudaMemcpyDeviceToHost));

  std::ostringstream oss;
  oss << "callKernel execution time with sysDim(" << numberOfCPs << ")= ";

  showExecutionTime(&start, &stop, oss.str());
  return regImage;
}

short* runTPSCUDAWithText(tps::CudaMemory cm, std::vector<int> dimensions, int numberOfCPs, bool occupancy, bool twoDim, int blockSize) {
  dim3 threadsPerBlock;

  if (occupancy) {
    int maxBlockSize = getBlockSize(blockSize);
    threadsPerBlock = calculateBestThreadsPerBlock(maxBlockSize, twoDim);
  } else {
    threadsPerBlock.x = 8;
    threadsPerBlock.y = 8;
    if (twoDim) {
      threadsPerBlock.z = 1;
    } else {
      threadsPerBlock.z = 8;
    }
  }
  std::cout << "threadsPerBlock.x = " << threadsPerBlock.x << std::endl;
  std::cout << "threadsPerBlock.y = " << threadsPerBlock.y << std::endl;
  std::cout << "threadsPerBlock.z = " << threadsPerBlock.z << std::endl;

  dim3 numBlocks(std::ceil(1.0*dimensions[0]/threadsPerBlock.x),
      std::ceil(1.0*dimensions[1]/threadsPerBlock.y),
      std::ceil(1.0*dimensions[2]/threadsPerBlock.z));

  short* regImage = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));

  for (int slice = 0; slice < dimensions[2]; slice++)
    for (int col = 0; col < dimensions[0]; col++)
      for (int row = 0; row < dimensions[1]; row++)
        regImage[slice*dimensions[1]*dimensions[0]+col*dimensions[1]+row] = 0;

  cudaEvent_t start, stop;
  startTimeRecord(&start, &stop);

  tpsCudaWithText<<<numBlocks, threadsPerBlock>>>(cm.getTexObj(), cm.getRegImage(), cm.getSolutionX(), cm.getSolutionY(),
      cm.getSolutionZ(), dimensions[0], dimensions[1], dimensions[2], cm.getKeypointX(),
      cm.getKeypointY(), cm.getKeypointZ(), numberOfCPs);
  checkCuda(cudaDeviceSynchronize());
  checkCuda(cudaMemcpy(regImage, cm.getRegImage(), dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short), cudaMemcpyDeviceToHost));

  std::ostringstream oss;
  oss << "callKernel execution time with sysDim(" << numberOfCPs << ")= ";

  showExecutionTime(&start, &stop, oss.str());
  return regImage;
}

short* interpolateImage(short* imageVoxels, float* imagePointsX, float* imagePointsY, float* imagePointsZ, std::vector<int> dimensions) {
  short* regImage = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        float newX = imagePointsX[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x];
        float newY = imagePointsY[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x];
        float newZ = imagePointsZ[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x];
        short newValue = trilinearInterpolation(newX, newY, newZ, imageVoxels, dimensions);
        regImage[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x] = newValue;
      }

  return regImage;
}

short* runTPSCUDAWithoutInterpolation(tps::CudaMemory cm, short* imageVoxels,
                    std::vector<int> dimensions, int numberOfCPs,
                    bool occupancy, bool twoDim, int blockSize) {
  dim3 threadsPerBlock;

  if (occupancy) {
    int maxBlockSize = getBlockSize(blockSize);
    threadsPerBlock = calculateBestThreadsPerBlock(maxBlockSize, twoDim);
  } else {
    threadsPerBlock.x = 8;
    threadsPerBlock.y = 8;
    if (twoDim) {
      threadsPerBlock.z = 1;
    } else {
      threadsPerBlock.z = 8;
    }
  }
  std::cout << "threadsPerBlock.x = " << threadsPerBlock.x << std::endl;
  std::cout << "threadsPerBlock.y = " << threadsPerBlock.y << std::endl;
  std::cout << "threadsPerBlock.z = " << threadsPerBlock.z << std::endl;

  dim3 numBlocks(std::ceil(1.0*dimensions[0]/threadsPerBlock.x),
      std::ceil(1.0*dimensions[1]/threadsPerBlock.y),
      std::ceil(1.0*dimensions[2]/threadsPerBlock.z));

  float* imagePointsX = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* imagePointsY = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* imagePointsZ = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));

  cudaEvent_t start, stop;
  startTimeRecord(&start, &stop);

  tpsCudaWithoutInterpolation<<<numBlocks, threadsPerBlock>>>(
      cm.getImagePointsX(), cm.getImagePointsY(), cm.getImagePointsZ(),
      cm.getSolutionX(), cm.getSolutionY(),cm.getSolutionZ(), dimensions[0],
      dimensions[1], dimensions[2], cm.getKeypointX(), cm.getKeypointY(),
      cm.getKeypointZ(), numberOfCPs);

  checkCuda(cudaDeviceSynchronize());
  checkCuda(cudaMemcpy(imagePointsX, cm.getImagePointsX(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(imagePointsY, cm.getImagePointsY(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(imagePointsZ, cm.getImagePointsZ(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));

  std::ostringstream oss;
  oss << "callKernel execution time with sysDim(" << numberOfCPs << ")= ";

  showExecutionTime(&start, &stop, oss.str());

  arma::wall_clock timer;
  timer.tic();
  short* regImage = interpolateImage(imageVoxels, imagePointsX, imagePointsY, imagePointsZ, dimensions);
  double time = timer.toc();
  std::cout << "Interpolation execution time(" << numberOfCPs << "): " << time << "s" << std::endl;

  return regImage;
}

float* generateDeforVectorAt(int x, int y, int z) {
  float* vectorField = (float*)std::malloc(3*sizeof(float));

  vectorField[0] = x + 2.0*sin(y/8.0) - 2.0*cos(z/16.0);
  vectorField[1] = y + 4.0*sin(x/8.0) - 2.0*sin(z/8.0);
  vectorField[2] = z + 2.0*sin(x/16.0) - 4.0*cos(y/8.0);

  return vectorField;
}

float normOf(float x, float y, float z) {
  return std::sqrt(x*x + y*y + z*z);
}

float calculateSD(float meanError, float *vectorFieldX, float* vectorFieldY,
    float* vectorFieldZ, std::vector<int> dimensions) {
  float standardDeviation = 0.0;

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        float* evf = generateDeforVectorAt(x, y, z); // expectedVectorField
        int pos = z*dimensions[1]*dimensions[0]+x*dimensions[1]+y;
        float currentError = normOf(evf[0] - vectorFieldX[pos], evf[1] - vectorFieldY[pos],
            evf[2] - vectorFieldZ[pos])/normOf(evf[0], evf[1], evf[2]);
        standardDeviation += std::pow(currentError - meanError, 2);
      }
  standardDeviation /= (dimensions[0]*dimensions[1]*dimensions[2]*1.0);
  standardDeviation = std::sqrt(standardDeviation);

  return standardDeviation;
}

float calculateError(float *vectorFieldX, float* vectorFieldY,
    float* vectorFieldZ, std::vector<int> dimensions) {
  float error = 0.0;

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        float* evf = generateDeforVectorAt(x, y, z); // expectedVectorField
        int pos = z*dimensions[1]*dimensions[0]+x*dimensions[1]+y;
        error += normOf(evf[0] - vectorFieldX[pos], evf[1] - vectorFieldY[pos],
            evf[2] - vectorFieldZ[pos])/normOf(evf[0], evf[1], evf[2]);
      }
  error /= (dimensions[0]*dimensions[1]*dimensions[2]*1.0);

  return error;
}

short* runTPSCUDAVectorFieldTest(tps::CudaMemory cm, short* imageVoxels,
    std::vector<int> dimensions,
    int numberOfCPs, bool occupancy,
    bool twoDim, int blockSize) {
  dim3 threadsPerBlock;

  if (occupancy) {
    int maxBlockSize = getBlockSize(blockSize);
    threadsPerBlock = calculateBestThreadsPerBlock(maxBlockSize, twoDim);
  } else {
    threadsPerBlock.x = 8;
    threadsPerBlock.y = 8;
    if (twoDim) {
      threadsPerBlock.z = 1;
    } else {
      threadsPerBlock.z = 8;
    }
  }
  std::cout << "threadsPerBlock.x = " << threadsPerBlock.x << std::endl;
  std::cout << "threadsPerBlock.y = " << threadsPerBlock.y << std::endl;
  std::cout << "threadsPerBlock.z = " << threadsPerBlock.z << std::endl;

  dim3 numBlocks(std::ceil(1.0*dimensions[0]/threadsPerBlock.x),
      std::ceil(1.0*dimensions[1]/threadsPerBlock.y),
      std::ceil(1.0*dimensions[2]/threadsPerBlock.z));

  float* vectorFieldX = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* vectorFieldY = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* vectorFieldZ = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));

  for (int slice = 0; slice < dimensions[2]; slice++)
    for (int col = 0; col < dimensions[0]; col++)
      for (int row = 0; row < dimensions[1]; row++) {
        vectorFieldX[slice*dimensions[1]*dimensions[0]+col*dimensions[1]+row] = 0;
        vectorFieldY[slice*dimensions[1]*dimensions[0]+col*dimensions[1]+row] = 0;
        vectorFieldZ[slice*dimensions[1]*dimensions[0]+col*dimensions[1]+row] = 0;
      }

  cudaEvent_t start, stop;
  startTimeRecord(&start, &stop);

  tpsCudaWithoutInterpolation<<<numBlocks, threadsPerBlock>>>(
      cm.getImagePointsX(), cm.getImagePointsY(), cm.getImagePointsZ(),
      cm.getSolutionX(), cm.getSolutionY(),cm.getSolutionZ(), dimensions[0],
      dimensions[1], dimensions[2], cm.getKeypointX(), cm.getKeypointY(),
      cm.getKeypointZ(), numberOfCPs);

  checkCuda(cudaDeviceSynchronize());
  checkCuda(cudaMemcpy(vectorFieldX, cm.getImagePointsX(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(vectorFieldY, cm.getImagePointsY(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(vectorFieldZ, cm.getImagePointsZ(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));

  std::ostringstream oss;
  oss << "callKernel execution time with sysDim(" << numberOfCPs << ")= ";

  showExecutionTime(&start, &stop, oss.str());

  arma::wall_clock timer;
  timer.tic();
  float error = calculateError(vectorFieldX, vectorFieldY, vectorFieldZ, dimensions);
  float sd = calculateSD(error, vectorFieldX, vectorFieldY, vectorFieldZ, dimensions);
  double time = timer.toc();
  std::cout << "Calculate error execution time(" << numberOfCPs << "): " << time << "s" << std::endl;
  std::cout << "Error for (" << numberOfCPs << ") = " << error << std::endl;
  std::cout << "SD for (" << numberOfCPs << ") = " << sd << std::endl;

  short* regImage = (short*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(short));

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        regImage[z*dimensions[0]*dimensions[1]+y*dimensions[0]+x] = 0;
      }

  return regImage;
}

float radialDiff(tps::Image imageA, std::vector<std::vector<float> > keypointsA,
    tps::Image imageB, std::vector<std::vector<float> > keypointsB) {
  float diff = 0.0;
  int r = 3;
  for (int i = 0; i < keypointsA.size(); i++) {
    float radialA = imageA.radialSum(r, keypointsA[i]);
    float radialB = imageB.radialSum(r, keypointsB[i]);
    diff += (radialA-radialB);
  }
  diff /= keypointsA.size();
  return diff;
}

std::vector<std::vector<float> > applyVectorField(
    std::vector<std::vector<float> > keypoints, float* vectorFieldX,
    float* vectorFieldY, float* vectorFieldZ, std::vector<int> dimensions) {
  std::vector<std::vector<float> > newKeypoints;

  for (int i = 0; i < keypoints.size(); i++) {
    int x = keypoints[i][0];
    int y = keypoints[i][1];
    int z = keypoints[i][2];
    int pos = z*dimensions[0]*dimensions[1]+y*dimensions[0]+z;
    std::vector<float> newKeypoint;
    newKeypoint.push_back(keypoints[i][0] + vectorFieldX[pos]);
    newKeypoint.push_back(keypoints[i][1] + vectorFieldY[pos]);
    newKeypoint.push_back(keypoints[i][2] + vectorFieldZ[pos]);

    newKeypoints.push_back(newKeypoint);
  }

  return newKeypoints;
}

void runTPSRadialDiff(tps::CudaMemory cm, tps::Image regImage,
    tps::Image referenceImage, std::vector<std::vector<float> >referenceKeypoints,
    tps::Image targetImage, std::vector<std::vector<float> > targetKeypoints,
    bool occupancy, bool twoDim, int blockSize) {
  std::vector<int> dimensions = referenceImage.getDimensions();
  int numberOfCPs = targetKeypoints.size();
  dim3 threadsPerBlock;

  if (occupancy) {
    int maxBlockSize = getBlockSize(blockSize);
    threadsPerBlock = calculateBestThreadsPerBlock(maxBlockSize, twoDim);
  } else {
    threadsPerBlock.x = 8;
    threadsPerBlock.y = 8;
    if (twoDim) {
      threadsPerBlock.z = 1;
    } else {
      threadsPerBlock.z = 8;
    }
  }

  dim3 numBlocks(std::ceil(1.0*dimensions[0]/threadsPerBlock.x),
      std::ceil(1.0*dimensions[1]/threadsPerBlock.y),
      std::ceil(1.0*dimensions[2]/threadsPerBlock.z));

  float* vectorFieldX = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* vectorFieldY = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));
  float* vectorFieldZ = (float*)malloc(dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float));

  cudaEvent_t start, stop;
  startTimeRecord(&start, &stop);

  tpsCudaWithoutInterpolation<<<numBlocks, threadsPerBlock>>>(
      cm.getImagePointsX(), cm.getImagePointsY(), cm.getImagePointsZ(),
      cm.getSolutionX(), cm.getSolutionY(),cm.getSolutionZ(), dimensions[0],
      dimensions[1], dimensions[2], cm.getKeypointX(), cm.getKeypointY(),
      cm.getKeypointZ(), numberOfCPs);

  checkCuda(cudaDeviceSynchronize());
  checkCuda(cudaMemcpy(vectorFieldX, cm.getImagePointsX(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(vectorFieldY, cm.getImagePointsY(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));
  checkCuda(cudaMemcpy(vectorFieldZ, cm.getImagePointsZ(),
        dimensions[0]*dimensions[1]*dimensions[2]*sizeof(float),
        cudaMemcpyDeviceToHost));

  std::ostringstream oss;
  oss << "callKernel execution time with sysDim(" << numberOfCPs << ")= ";

  showExecutionTime(&start, &stop, oss.str());

  arma::wall_clock timer;
  timer.tic();
  std::vector<std::vector<float> > resultKeypoints =
    applyVectorField(targetKeypoints, vectorFieldX, vectorFieldY,
                     vectorFieldZ, dimensions);

  float diffRefTar = radialDiff(referenceImage, referenceKeypoints,
                                targetImage, targetKeypoints);
  float diffRefRes = radialDiff(referenceImage, referenceKeypoints,
                                regImage, resultKeypoints);
  double time = timer.toc();
  std::cout << "Calculate error execution time(" << numberOfCPs << "): " << time << "s" << std::endl;
  std::cout << "Radial diff Ref Tar (" << numberOfCPs << ") = " << diffRefTar << std::endl;
  std::cout << "Radial diff Ref Reg (" << numberOfCPs << ") = " << diffRefRes << std::endl;


}

