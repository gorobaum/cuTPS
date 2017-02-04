#include "paralleltps.h"

#include "utils/globalconfiguration.h"

#include <cmath>
#include <iostream>
#include <armadillo>

void tps::ParallelTPS::runThread(uint tid) {
  int chunck = dimensions_[0]/numberOfThreads;
  uint limit = (tid+1)*chunck;
  if ((tid+1) == numberOfThreads) limit = dimensions_[0];
  for (uint x = tid*chunck; x < limit; x++)
    for (int y = 0; y < dimensions_[1]; y++)
      for (int z = 0; z < dimensions_[2]; z++) {
        double newX = solutionX_[0] + x * solutionX_[1] + y * solutionX_[2] + z * solutionX_[3];
        double newY = solutionY_[0] + x * solutionY_[1] + y * solutionY_[2] + z * solutionY_[3];
        double newZ = solutionZ_[0] + x * solutionZ_[1] + y * solutionZ_[2] + z * solutionZ_[3];
        for (uint i = 0; i < referenceKeypoints_.size(); i++) {
          float r = computeRSquared(x, referenceKeypoints_[i][0],
              y, referenceKeypoints_[i][1],
              z, referenceKeypoints_[i][2]);
          if (r != 0.0) {
            newX += r * log(r) * solutionX_[i+4];
            newY += r * log(r) * solutionY_[i+4];
            newZ += r * log(r) * solutionZ_[i+4];
          }
        }
        short value = targetImage_.trilinearInterpolation(newX, newY, newZ);
        registredImage.changePixelAt(x, y, z, value);
      }
}

tps::Image tps::ParallelTPS::run() {
  bool checkError = GlobalConfiguration::getInstance().getBoolean("checkError");

  tps::Image result;

  int numberOfCPs = targetKeypoints_.size();

  if (checkError) {
    result = calculateError(numberOfCPs);
  } else {
    result = runNormal(numberOfCPs);
  }
  return result;
}

tps::Image tps::ParallelTPS::runNormal(int numberOfCPs) {
  std::vector<std::thread> th;

  arma::wall_clock timer;
  timer.tic();


  for (uint i = 0; i < numberOfThreads; ++i)
    th.push_back(std::thread(&tps::ParallelTPS::runThread, this, i));

  for(uint i = 0; i < numberOfThreads; ++i)
    th[i].join();

  double time = timer.toc();
  std::cout << "(" << numberOfCPs << ")Parallel TPS execution time: " << time << std::endl;

  return registredImage;
}

  void tps::ParallelTPS::runThreadForError(uint tid, std::vector<std::vector<float>>& field) {
  int chunck = dimensions_[0]/numberOfThreads;
  uint limit = (tid+1)*chunck;
  if ((tid+1) == numberOfThreads) limit = dimensions_[0];
  for (uint x = tid*chunck; x < limit; x++)
    for (int y = 0; y < dimensions_[1]; y++)
      for (int z = 0; z < dimensions_[2]; z++) {
        double newX = solutionX_[0] + x * solutionX_[1] + y * solutionX_[2] + z * solutionX_[3];
        double newY = solutionY_[0] + x * solutionY_[1] + y * solutionY_[2] + z * solutionY_[3];
        double newZ = solutionZ_[0] + x * solutionZ_[1] + y * solutionZ_[2] + z * solutionZ_[3];
        for (uint i = 0; i < referenceKeypoints_.size(); i++) {
          float r = computeRSquared(x, referenceKeypoints_[i][0],
              y, referenceKeypoints_[i][1],
              z, referenceKeypoints_[i][2]);
          if (r != 0.0) {
            newX += r * log(r) * solutionX_[i+4];
            newY += r * log(r) * solutionY_[i+4];
            newZ += r * log(r) * solutionZ_[i+4];
          }
        }
        int pos = z*dimensions_[0]*dimensions_[1]+y*dimensions_[0]+x;
        field[pos][0] = newX - x;
        field[pos][1] = newY - y;
        field[pos][2] = newZ - z;
        short value = targetImage_.trilinearInterpolation(newX, newY, newZ);
        registredImage.changePixelAt(x, y, z, value);
      }
}

float* tps::ParallelTPS::pgenerateDeforVectorAt(int x, int y, int z) {
  float* vectorField = (float*)std::malloc(3*sizeof(float));

  vectorField[0] = x + 2.0*sin(y/8.0) - 2.0*cos(z/16.0);
  vectorField[1] = y + 4.0*sin(x/8.0) - 2.0*sin(z/8.0);
  vectorField[2] = z + 2.0*sin(x/16.0) - 4.0*cos(y/8.0);

  return vectorField;
}

float tps::ParallelTPS::pnormOf(float x, float y, float z) {
  return std::sqrt(x*x + y*y + z*z);
}

float tps::ParallelTPS::pcalculateSD(float meanError, std::vector<std::vector<float>> field,
    std::vector<int> dimensions) {
  float standardDeviation = 0.0;

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        float* evf = pgenerateDeforVectorAt(x, y, z); // expectedVectorField
        int pos = z*dimensions[1]*dimensions[0]+x*dimensions[1]+y;
        float currentError = pnormOf(evf[0] - field[pos][0], evf[1] - field[pos][1],
            evf[2] - field[pos][2])/pnormOf(evf[0], evf[1], evf[2])*1.0;
        standardDeviation += std::pow(currentError - meanError, 2);
      }
  standardDeviation /= (dimensions[0]*dimensions[1]*dimensions[2]*1.0);
  standardDeviation = std::sqrt(standardDeviation);

  return standardDeviation;
}

float tps::ParallelTPS::pcalculateError(std::vector<std::vector<float>> field, std::vector<int> dimensions) {
  float error = 0.0;

  for (int x = 0; x < dimensions[0]; x++)
    for (int y = 0; y < dimensions[1]; y++)
      for (int z = 0; z < dimensions[2]; z++) {
        float* evf = pgenerateDeforVectorAt(x, y, z); // expectedVectorField
        int pos = z*dimensions[1]*dimensions[0]+x*dimensions[1]+y;
        error += pnormOf(evf[0] - field[pos][0], evf[1] - field[pos][1],
            evf[2] - field[pos][2])/pnormOf(evf[0], evf[1], evf[2])*1.0;
      }
  error /= (dimensions[0]*dimensions[1]*dimensions[2]*1.0);

  return error;
}



tps::Image tps::ParallelTPS::calculateError(int numberOfCPs) {
  std::vector<std::thread> th;

  int size = dimensions_[0]*dimensions_[1]*dimensions_[2];
  std::vector<std::vector<float>> vectorField(size, std::vector<float>(3, 0));

  arma::wall_clock timer;
  timer.tic();

  for (uint i = 0; i < numberOfThreads; ++i)
    th.push_back(std::thread(&tps::ParallelTPS::runThreadForError, this, i,
          std::ref(vectorField)));

  for(uint i = 0; i < numberOfThreads; ++i)
    th[i].join();

  float error = pcalculateError(vectorField, dimensions_);
  float sd = pcalculateSD(error, vectorField, dimensions_);

  for(int i = 0; i < 100; i++)
    std::cout << vectorField[i][0] << " " << vectorField[i][1] << " " << vectorField[i][2] << std::endl;

  std::cout << "Error for (" << numberOfCPs << ") = " << error << std::endl;
  std::cout << "SD for (" << numberOfCPs << ") = " << sd << std::endl;

  double time = timer.toc();
  std::cout << "(" << numberOfCPs << ")Parallel TPS execution time: " << time << std::endl;

  return registredImage;
}
