#include "runinstance.h"

#include <iostream>

namespace tps {

void RunInstance::loadData() {
    targetImage_ = loadImageData("targetImage");
    referenceImage_ = loadImageData("referenceImage");

    configuration_.printConfigs();
    std::cout << configuration_.getString("referenceImage") << std::endl;
    std::cout << configuration_.getString("targetImage") << std::endl;
    std::cout << configuration_.getDouble("percentage") << std::endl;

    std::vector<std::vector<double>> keypoints = configuration_.getDoubleVector("keypoints");

    for (int x = 0; x < keypoints.size(); x++) {
          for (int y = 0; y < keypoints[x].size(); y++)
              std::cout << keypoints[x][y] << " ";
        std::cout << std::endl;
    }
}

Image RunInstance::loadImageData(std::string configString) {
    std::string imagePath = configuration_.getString(configString);
    return imageHandler_->loadImageData(imagePath);
}


} //namespace
