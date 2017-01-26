#include <iostream>
#include <cmath>

#include "cutps.h"
#include "cudatps.h"
#include "utils/globalconfiguration.h"

#define MAXTHREADPBLOCK 1024

void tps::CudaTPS::setCudaMemory(tps::CudaMemory cm) {
    cm_ = cm;
}

tps::Image tps::CudaTPS::run() {
    short *regImage;
    bool occupancy = GlobalConfiguration::getInstance().getBoolean("calculateOccupancy");
    bool twoDim = targetImage_.isTwoDimensional();
    bool texture = GlobalConfiguration::getInstance().getBoolean("imageTexture");
    bool cpuInterpolation = GlobalConfiguration::getInstance().getBoolean("cpuInterpolation");
    bool checkError = GlobalConfiguration::getInstance().getBoolean("checkError");
    int blockSize = GlobalConfiguration::getInstance().getInt("blockSize");
    if (texture) {
        regImage = runTPSCUDAWithText(cm_, dimensions_,
                      referenceKeypoints_.size(), occupancy, twoDim, blockSize);
    } else if (cpuInterpolation){
        regImage = runTPSCUDAWithoutInterpolation(cm_,
                      targetImage_.getPixelVector(), dimensions_,
                      referenceKeypoints_.size(), occupancy, twoDim, blockSize);
    } else if (checkError) {
        regImage = runTPSCUDAVectorFieldTest(cm_, targetImage_.getPixelVector(),
                      dimensions_, referenceKeypoints_.size(), occupancy,
                      twoDim, blockSize);
    } else {
        regImage = runTPSCUDA(cm_, dimensions_, referenceKeypoints_.size(),
                      occupancy, twoDim, blockSize);
    }


    registredImage.setPixelVector(regImage);
    delete(regImage);
    return registredImage;
}
