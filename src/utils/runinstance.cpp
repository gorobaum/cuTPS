#include "runinstance.h"

#include <iostream>

#include "tps/cudatps.h"
#include "tps/basictps.h"
#include "tps/paralleltps.h"
#include "globalconfiguration.h"
#include "linearsystem/armalinearsystems.h"
#include "linearsystem/cudalinearsystems.h"

namespace tps {

void RunInstance::loadData() {
    targetImage_ = loadImageData("targetImage");
    loadKeypoints();

    if (GlobalConfiguration::getInstance().isCuda()) {
        std::vector<int> dimensions = referenceImage_.getDimensions();
        cudaMemory_.initialize(dimensions, referenceKeypoints_);
    }
}

void RunInstance::loadKeypoints() {
    if (instanceConfiguration_.isCallFeatureGenerator()) {
        generateKeypoints();
    } else {
        targetKeypoints_ = instanceConfiguration_.getFloatVector("targetKeypoints");
        referenceKeypoints_ = instanceConfiguration_.getFloatVector("referenceKeypoints");
    }
    generateKeypointImage();
}

void RunInstance::generateKeypoints() {
    float percentage = instanceConfiguration_.getFloat("percentage");
    FeatureGenerator featureGenerator(referenceImage_, targetImage_, percentage);
    featureGenerator.run();

    targetKeypoints_ = featureGenerator.getTargetKeypoints();
    referenceKeypoints_ = featureGenerator.getReferenceKeypoints();
}

void RunInstance::allocCudaMemory() {
    cudaMemory_.allocCudaMemory(targetImage_);
}

void RunInstance::solveLinearSystem() {
    printInfo();
    std::cout << "Running solver..." << std::endl;
    std::string solverConfig = GlobalConfiguration::getInstance().getString("linearSystemSolver");
    if (solverConfig.compare("cuda") == 0)
        solveLinearSystemWithCuda();
    else if (solverConfig.compare("armadillo") == 0) {
        solveLinearSystemWithArmadillo();
    } else {
        std::cout << "The solver \"" << solverConfig << "\" isn't present." << std::endl;
    }
    std::cout << "------------------------------------------" << std::endl;
}

void RunInstance::executeTps() {
    std::cout << "Running tps..." << std::endl;

    Image result(referenceImage_.getDimensions());

    std::string tpsConfig = GlobalConfiguration::getInstance().getString("tps");

    if (tpsConfig.compare("basic") == 0)
        result = executeBasicTps();
    else if (tpsConfig.compare("parallel") == 0) {
        result = executeParallelTps();
    } else if (tpsConfig.compare("cuda") == 0) {
        result = executeCudaTps();
    } else {
        std::cout << "The TPS algorithm \"" << tpsConfig << "\" isn't present." << std::endl;
    }

    std::string resultImage = instanceConfiguration_.getString("resultImage");
    resultImage += GlobalConfiguration::getInstance().getString("resultImageMod");
    imageHandler_->saveImageData(result, resultImage);

    Image subImage = result.createSubtractionImageFrom(referenceImage_);
    imageHandler_->saveImageData(subImage, resultImage+"_sub_");
    isDone_ = true;
    if (GlobalConfiguration::getInstance().isCuda())
        cudaMemory_.freeMemory();
}

void RunInstance::printInfo() {
    std::cout << "==========================================" << std::endl;
    // instanceConfiguration_.printConfigs();
    std::cout << "------------------------------------------" << std::endl;
}

Image RunInstance::executeBasicTps() {
    BasicTPS basicTps(referenceKeypoints_, targetKeypoints_, targetImage_);
    basicTps.setLinearSystemSolutions(solutionX_, solutionY_, solutionZ_);
    return basicTps.run();
}

Image RunInstance::executeParallelTps() {
    ParallelTPS parallelTps(referenceKeypoints_, targetKeypoints_, targetImage_);
    parallelTps.setLinearSystemSolutions(solutionX_, solutionY_, solutionZ_);
    return parallelTps.run();
}

Image RunInstance::executeCudaTps() {
    CudaTPS cudaTps(referenceKeypoints_, targetKeypoints_, targetImage_);

    std::string solverConfig = GlobalConfiguration::getInstance().getString("linearSystemSolver");
    if (solverConfig.compare("armadillo") == 0) {
        cudaMemory_.setSolutionX(solutionX_);
        cudaMemory_.setSolutionY(solutionY_);
        cudaMemory_.setSolutionZ(solutionZ_);
    }
    cudaTps.setCudaMemory(cudaMemory_);

    return cudaTps.run();
}

void RunInstance::solveLinearSystemWithCuda() {
    bool isTwoDimensional = referenceImage_.isTwoDimensional();
    CudaLinearSystems cudaLinearSystems(referenceKeypoints_, targetKeypoints_,
                                        isTwoDimensional);

    cudaLinearSystems.solveLinearSystems(cudaMemory_);

    solutionX_ = cudaLinearSystems.getSolutionX();
    solutionY_ = cudaLinearSystems.getSolutionY();
    solutionZ_ = cudaLinearSystems.getSolutionZ();
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

    for (int i = 0; i < targetKeypoints_.size(); i++) {
        int x = std::ceil(targetKeypoints_[i][0]);
        int y = std::ceil(targetKeypoints_[i][1]);
        int z = std::ceil(targetKeypoints_[i][2]);
        for (int i = x - 2; i < x + 2; i++)
            for (int j = y - 2; j < y + 2; j++)
                for (int k = z - 2; k < z + 2; k++)
                    result.changePixelAt(i, j, k, 255);
    }
    std::string keypointImage = instanceConfiguration_.getString("keypointImage");
    imageHandler_->saveImageData(result, keypointImage);
}

double RunInstance::getEstimateGpuMemory() {
    return cudaMemory_.memoryEstimation();
}

Image RunInstance::loadImageData(std::string configString) {
    std::string imagePath = instanceConfiguration_.getString(configString);
    return imageHandler_->loadImageData(imagePath);
}


} //namespace
