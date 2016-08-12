#include <iostream>
#include <cmath>

#include "cudatps.h"
#include "cutps.h"

#define MAXTHREADPBLOCK 1024

void tps::CudaTPS::setCudaMemory(tps::CudaMemory cm) {
    cm_ = cm;
}

tps::Image tps::CudaTPS::run() {
    short *regImage;
    regImage = runTPSCUDA(cm_, dimensions_, referenceKeypoints_.size());

    registredImage.setPixelVector(regImage);

    delete(regImage);
    return registredImage;
}
