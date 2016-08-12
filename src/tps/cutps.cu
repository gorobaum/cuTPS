#include <iostream>
#include <cassert>

#include "cuda.h"
#include "cuda_runtime.h"
#include "cuda_occupancy.h"

#include "cutps.h"

inline
cudaError_t checkCuda(cudaError_t result)
{
    if (result != cudaSuccess) {
        std::cout << "CUDA Runtime Error: \n" << cudaGetErrorString(result) << std::endl;
        assert(result == cudaSuccess);
    }
    return result;
}

// Kernel definition
__device__ short cudaGetPixel(int x, int y, int z, short* image, int width, int height, int slices) {
  if (x > width-1 || x < 0) return 0;
  if (y > height-1 || y < 0) return 0;
  if (z > slices-1 || z < 0) return 0;
  return image[z*height*width+x*height+y];
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

// Kernel definition
__global__ void tpsCuda(short* cudaImage, short* cudaRegImage, float* solutionX, float* solutionY, 
                        float* solutionZ, int width, int height, int slices, float* keyX, float* keyY, 
                        float* keyZ, int numOfKeys) {
  int x = blockDim.x*blockIdx.x + threadIdx.x;
  int y = blockDim.y*blockIdx.y + threadIdx.y;
  int z = blockDim.z*blockIdx.z + threadIdx.z;

  float newX = solutionX[0] + x*solutionX[1] + y*solutionX[2] + z*solutionX[3];
  float newY = solutionY[0] + x*solutionY[1] + y*solutionY[2] + z*solutionY[3];
  float newZ = solutionZ[0] + x*solutionZ[1] + y*solutionZ[2] + z*solutionZ[3];

  for (int i = 0; i < numOfKeys; i++) {
    float r = (x-keyX[i])*(x-keyX[i]) + (y-keyY[i])*(y-keyY[i]) + (z-keyZ[i])*(z-keyZ[i]);
    if (r != 0.0) {
      newX += r*log(r) * solutionX[i+4];
      newY += r*log(r) * solutionY[i+4];
      newZ += r*log(r) * solutionZ[i+4];
    }
  }
  if (x <= width-1 && x >= 0)
    if (y <= height-1 && y >= 0)
      if (z <= slices-1 && z >= 0)
        cudaRegImage[z*height*width+x*height+y] = cudaTrilinearInterpolation(newX, newY, newZ, cudaImage, width, height, slices);
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
  std::cout << output << elapsedTime << " ms\n";
}

short* runTPSCUDA(tps::CudaMemory cm, std::vector<int> dimensions, int numberOfCPs) {
  dim3 threadsPerBlock(8, 8, 8);
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

  showExecutionTime(&start, &stop, "callKernel execution time = ");
  return regImage;
}