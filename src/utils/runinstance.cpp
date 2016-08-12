#include "runinstance.h"

#include <iostream>

#include "linearsystem/armalinearsystems.h"
#include "linearsystem/cudalinearsystems.h"

namespace tps {

void RunInstance::loadData() {
    targetImage_ = loadImageData("targetImage");
    loadKeypoints();
}

void RunInstance::loadKeypoints() {
    if (configuration_.isCallFeatureGenerator())
        generateKeypoints();
    else {
        targetKeypoints_ = configuration_.getFloatVector("targetKeypoints");
        referenceKeypoints_ = configuration_.getFloatVector("referenceKeypoints");
    }
    generateKeypointImage();
}

void RunInstance::generateKeypoints() {
    float percentage = configuration_.getFloat("percentage");
    FeatureGenerator featureGenerator(referenceImage_, targetImage_, percentage);
    featureGenerator.run();

    targetKeypoints_ = featureGenerator.getTargetKeypoints();
    referenceKeypoints_ = featureGenerator.getReferenceKeypoints();
}

void RunInstance::solveLinearSystem() {
    std::string solver = configuration_.getString("linearSystemSolver");
    // if (solver.compare("cuda") == 0)
        solveLinearSystemWithCuda();
    // else {
        solveLinearSystemWithArmadillo();
    // }
}

void RunInstance::solveLinearSystemWithCuda() {
    bool isTwoDimensional = referenceImage_.isTwoDimensional();
    CudaLinearSystems cudaLinearSystems(referenceKeypoints_, targetKeypoints_,
                                        isTwoDimensional);

    std::vector<int> dimensions = referenceImage_.getDimensions();
    cudaMemory_.initialize(dimensions, referenceKeypoints_, targetImage_);

    cudaLinearSystems.solveLinearSystems(cudaMemory_);
}

void RunInstance::solveLinearSystemWithArmadillo() {
    bool isTwoDimensional = referenceImage_.isTwoDimensional();
    ArmaLinearSystems armaLinearSystems(referenceKeypoints_, targetKeypoints_,
                                        isTwoDimensional);

    armaLinearSystems.solveLinearSystems();

    solutionX_ = armaLinearSystems.getSolutionX();
    solutionY_ = armaLinearSystems.getSolutionY();
    solutionZ_ = armaLinearSystems.getSolutionZ();
}

void RunInstance::generateKeypointImage() {
    Image result(referenceImage_.getDimensions());

    for (int i = 0; i < targetKeypoints_.size(); i++)
        for (int j = 0; j < targetKeypoints_[i].size(); j++) {
            int x = std::ceil(targetKeypoints_[i][0]);
            int y = std::ceil(targetKeypoints_[i][1]);
            int z = std::ceil(targetKeypoints_[i][2]);
            result.changePixelAt(x, y, z, 255);
        }
    imageHandler_->saveImageData(result, "keypoints");
}

Image RunInstance::loadImageData(std::string configString) {
    std::string imagePath = configuration_.getString(configString);
    return imageHandler_->loadImageData(imagePath);
}


} //namespace
