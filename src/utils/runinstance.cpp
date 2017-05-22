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
    std::cout << targetImage_.getPixelVector()[0] << std::endl;
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

    generateDiffImage(result, resultImage);

    isDone_ = true;

    calculateMSE(result, resultImage);

    if (GlobalConfiguration::getInstance().isCuda())
      cudaMemory_.freeMemory();
  }

  void RunInstance::generateDiffImage(Image result, std::string resultImage) {
    Image subImage;
    if (instanceConfiguration_.hasConfiguration("region")) {
      std::vector<int> region = instanceConfiguration_.getIntVector("region");
      subImage = result.createSubtractionImageFromWithRegion(referenceImage_, region);
    } else {
      subImage = result.createSubtractionImageFrom(referenceImage_);
    }
    imageHandler_->saveImageData(subImage, resultImage+"_sub_");
  }

  void RunInstance::calculateMSE(Image result, std::string resultImage) {
    float refTarMSE, refResMES;
    bool hasRegion = instanceConfiguration_.hasConfiguration("region");
    if (hasRegion) {
      std::vector<int> region = instanceConfiguration_.getIntVector("region");
      refTarMSE = referenceImage_.meanSquaredErrorWithRegion(targetImage_, region);
      refResMES = referenceImage_.meanSquaredErrorWithRegion(result, region);
    } else {
      refTarMSE = referenceImage_.meanSquaredError(targetImage_);
      refResMES = referenceImage_.meanSquaredError(result);
    }
    std::cout << "MSE for Ref Tar(" << resultImage << ") = "
      << refTarMSE << std::endl;
    std::cout << "MSE for Ref Res(" << resultImage << ") = "
      << refResMES << std::endl;
  }

  void RunInstance::printInfo() {
    std::cout << "==========================================" << std::endl;
    // instanceConfiguration_.printConfigs();
    std::cout << "------------------------------------------" << std::endl;
  }

  Image RunInstance::executeBasicTps() {
    BasicTPS basicTps(referenceKeypoints_, targetKeypoints_, targetImage_,
        referenceImage_);
    basicTps.setLinearSystemSolutions(solutionX_, solutionY_, solutionZ_);
    return basicTps.run();
  }

  Image RunInstance::executeParallelTps() {
    ParallelTPS parallelTps(referenceKeypoints_, targetKeypoints_,
        targetImage_, referenceImage_);
    parallelTps.setLinearSystemSolutions(solutionX_, solutionY_, solutionZ_);
    return parallelTps.run();
  }

  Image RunInstance::executeCudaTps() {
    CudaTPS cudaTps(referenceKeypoints_, targetKeypoints_, targetImage_,
        referenceImage_);

    std::string solverConfig = GlobalConfiguration::getInstance().getString("linearSystemSolver");
    if (solverConfig.compare("armadillo") == 0) {
      cudaMemory_.setSolutionX(solutionX_);
      cudaMemory_.setSolutionY(solutionY_);
      cudaMemory_.setSolutionZ(solutionZ_);
    }
    cudaTps.setCudaMemory(cudaMemory_);
    std::string resultImage = instanceConfiguration_.getString("resultImage");
    cudaTps.setModifier(resultImage);

    return cudaTps.run();
  }

  void RunInstance::solveLinearSystemWithCuda() {
    bool isTwoDimensional = referenceImage_.isTwoDimensional();
    CudaLinearSystems cudaLinearSystems(referenceKeypoints_, targetKeypoints_,
        isTwoDimensional);

    std::string resultImage = instanceConfiguration_.getString("resultImage");

    cudaLinearSystems.solveLinearSystems(cudaMemory_, resultImage);

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

    short* pagode = targetImage_.getPixelVector();
    std::cout << pagode[0] << std::endl;
    result.setPixelVector(pagode);

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
    Image img = imageHandler_->loadImageData(imagePath);
    return img;
  }

  std::string RunInstance::getImageName() {
    return instanceConfiguration_.getString("resultImage");
  }

} //namespace
