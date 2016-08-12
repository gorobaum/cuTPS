#include "paralleltps.h"

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
	std::vector<std::thread> th;

    arma::wall_clock timer;
    timer.tic();

	for (uint i = 0; i < numberOfThreads; ++i)
        th.push_back(std::thread(&tps::ParallelTPS::runThread, this, i));

    for(uint i = 0; i < numberOfThreads; ++i)
        th[i].join();

    double time = timer.toc();
    std::cout << "Parallel TPS execution time: " << time << std::endl;

    return registredImage;
}
