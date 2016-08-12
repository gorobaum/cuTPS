#include "controller.h"

namespace tps {

void Controller::exec() {
    for (std::vector<RunInstance>::iterator it = runInstances_.begin(); it != runInstances_.end(); it++)
        it->loadData();

    for (std::vector<RunInstance>::iterator it = runInstances_.begin(); it != runInstances_.end(); it++)
        it->solveLinearSystem();

    for (std::vector<RunInstance>::iterator it = runInstances_.begin(); it != runInstances_.end(); it++)
        it->executeTps();
}

void Controller::readConfigurationFile(std::string masterConfigFilePath) {
    std::ifstream infile;

    infile.open(masterConfigFilePath.c_str());

    std::string referenceImagePath;
    std::getline(infile, referenceImagePath);

    // Based on the reference image name, load the correct ImageHandler and
    // load the reference image data to the memory
    ImageHandler* imageHandler = getCorrectImageHandler(referenceImagePath);
    Image referenceImage = imageHandler->loadImageData(referenceImagePath);

    std::string newInstanceConfigurationFile;
    while (std::getline(infile, newInstanceConfigurationFile)) {
        RunInstance newRunInstance(newInstanceConfigurationFile, referenceImage,
                                   imageHandler);
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
