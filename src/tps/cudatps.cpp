#include <iostream>
#include <cmath>

#include "cudatps.h"
#include "cutps.h"

#define MAXTHREADPBLOCK 1024

float* tps::CudaTPS::solutionPointer(std::vector<float> solution) {
    float* vector = (float*)malloc(solution.size()*sizeof(float));
    for(int i = 0; i < solution.size(); i++)
        vector[i] = solution[i];
    return vector;
}

void tps::CudaTPS::setCudaMemory(tps::CudaMemory cm) {
    cm_ = cm;
    isCmSet = true;
}

tps::Image tps::CudaTPS::run() {
    short *regImage;
    if (isCmSet) {
        regImage = runTPSCUDAwithCm(cm_, dimensions_, referenceKeypoints_.size());
    }

    registredImage.setPixelVector(regImage);

    delete(regImage);
    return registredImage;
}
