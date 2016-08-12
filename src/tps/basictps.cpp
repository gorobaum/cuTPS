#include "basictps.h"

#include <cmath>
#include <iostream>
#include <armadillo>

tps::Image tps::BasicTPS::run() {
    arma::wall_clock timer;
    timer.tic();

	for (int x = 0; x < dimensions_[0]; x++)
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

        double time = timer.toc();
        std::cout << "Basic TPS execution time: " << time << std::endl;

		return registredImage;
}
