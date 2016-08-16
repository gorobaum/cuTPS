#ifndef TPS_UTILS_CONTROLLER_H_
#define TPS_UTILS_CONTROLLER_H_

#include <string>
#include <vector>

#include "runinstance.h"
#include "image/imagehandler.h"
#include "image/itkimagehandler.h"
#include "image/opcvimagehandler.h"

namespace tps {

class Controller {
public:
    Controller(std::string masterConfigFilePath) {
        readConfigurationFile(masterConfigFilePath);
    }

    void exec();
    void readConfigurationFile(std::string masterConfigFilePath);
    static void runThread(void* data);

private:
    void runSingleProcess();
    void runMultipleProcess();
    void runLoadedInstances();
    ImageHandler* getCorrectImageHandler(std::string referenceImagePath);

    int lastInstaceLoaded_ = 0;
    int lastInstanceExecuted_ = 0;
    std::vector<RunInstance> runInstances_;
};

} //namespace

#endif
