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
    if (texture) {
        regImage = runTPSCUDAWithText(cm_, dimensions_, referenceKeypoints_.size(), occupancy, twoDim);
    } else {
        regImage = runTPSCUDA(cm_, dimensions_, referenceKeypoints_.size(), occupancy, twoDim);
    }

    registredImage.setPixelVector(regImage);

    delete(regImage);
    return registredImage;
}
