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

private:
    ImageHandler* getCorrectImageHandler(std::string referenceImagePath);

    std::vector<RunInstance> runInstances_;
};

} //namespace

#endif
