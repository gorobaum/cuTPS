#include "paralleltps.h"

#include <cmath>
#include <iostream>

void tps::ParallelTPS::runThread(uint tid) {
	int chunck = dimensions_[0]/numberOfThreads;
	uint limit = (tid+1)*chunck;
	if ((tid+1) == numberOfThreads) limit = dimensions_[0];
	for (uint x = tid*chunck; x < limit; x++)
		for (int y = 0; y < dimensions_[1]; y++)
			for (int z = 0; z < dimensions_[2]; z++) {
				double newX = solutionX[0] + x * solutionX[1] + y * solutionX[2] + z * solutionX[3];
				double newY = solutionY[0] + x * solutionY[1] + y * solutionY[2] + z * solutionY[3];
				double newZ = solutionZ[0] + x * solutionZ[1] + y * solutionZ[2] + z * solutionZ[3];
				for (uint i = 0; i < referenceKeypoints_.size(); i++) {
					float r = computeRSquared(x, referenceKeypoints_[i][0],
																		y, referenceKeypoints_[i][1],
																		z, referenceKeypoints_[i][2]);
					if (r != 0.0) {
						newX += r * log(r) * solutionX[i+4];
						newY += r * log(r) * solutionY[i+4];
						newZ += r * log(r) * solutionZ[i+4];
					}
				}
			short value = targetImage_.trilinearInterpolation(newX, newY, newZ);
			registredImage.changePixelAt(x, y, z, value);
		}
}

tps::Image tps::ParallelTPS::run() {
    lienarSolver.solveLinearSystems();
	solutionX = lienarSolver.getSolutionX();
	solutionY = lienarSolver.getSolutionY();
	solutionZ = lienarSolver.getSolutionZ();
	std::vector<std::thread> th;

	for (uint i = 0; i < numberOfThreads; ++i)
    th.push_back(std::thread(&tps::ParallelTPS::runThread, this, i));

    for(uint i = 0; i < numberOfThreads; ++i) th[i].join();

    return registredImage;
}
