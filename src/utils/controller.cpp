#include "controller.h"
#include "cudamemory.h"
#include "globalconfiguration.h"

namespace tps {

void Controller::exec() {
    bool isCuda = GlobalConfiguration::getInstance().isCuda();
    double totalGpuMemory = CudaMemory::getGpuMemory() * 0.9;
    std::vector<RunInstance> loadedInstances;
    int instance = 0;
    int lastExecution = 0;
    std::cout << "Max GPU memory = " << totalGpuMemory << "MB" << std::endl;

    while (instance < runInstances_.size()) {
        double currentUsedMemory = CudaMemory::getUsedGpuMemory();

        for (instance; instance < runInstances_.size(); instance++) {
            runInstances_[instance].loadData();
            if (isCuda) {
                double estimatedMemory = runInstances_[instance].getEstimateGpuMemory();
                std::cout << "Current GPU used memory = " << currentUsedMemory << "MB" << std::endl;
                std::cout << "Current GPU estimated memory = " << estimatedMemory << "MB" << std::endl;
                currentUsedMemory += estimatedMemory;
                if (estimatedMemory < totalGpuMemory) {
                    runInstances_[instance].allocCudaMemory();
                } else {
                    break;
                }
            }
        }

        for (lastExecution; lastExecution < instance; lastExecution++) {
            runInstances_[lastExecution].solveLinearSystem();
            runInstances_[lastExecution].executeTps();
        }
    }

}

void Controller::readConfigurationFile(std::string masterConfigFilePath) {
    GlobalConfiguration::getInstance().loadConfigurationFile(masterConfigFilePath);

    std::string referenceImagePath = GlobalConfiguration::getInstance().getString("referenceImage");

    // Based on the reference image name, load the correct ImageHandler and
    // load the reference image data to the memory
    ImageHandler* imageHandler = getCorrectImageHandler(referenceImagePath);
    Image referenceImage = imageHandler->loadImageData(referenceImagePath);

    std::vector<std::string> targetFiles = GlobalConfiguration::getInstance().getTargetFiles();

    for (std::vector<std::string>::iterator it = targetFiles.begin(); it != targetFiles.end(); it++) {
        RunInstance newRunInstance(*it, referenceImage, imageHandler);
        runInstances_.push_back(newRunInstance);
    }
}

ImageHandler* Controller::getCorrectImageHandler(std::string referenceImagePath) {
    std::size_t pos = referenceImagePath.find('.');
    std::string extension = referenceImagePath.substr(pos);

    tps::ImageHandler *imageHandler;

    if (extension.compare(".nii.gz") == 0)
        imageHandler = new tps::ITKImageHandler();
    else
        imageHandler = new tps::OPCVImageHandler();

    return imageHandler;
}


} //namespace
