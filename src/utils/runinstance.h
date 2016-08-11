#ifndef TPS_UTILS_RUNINSTANCE_H_
#define TPS_UTILS_RUNINSTANCE_H_

#include <string>

#include "image/image.h"
#include "configuration.h"
#include "image/imagehandler.h"

namespace tps {

class RunInstance {
public:
    RunInstance(std::string configurationFilePath, ImageHandler* imageHandler) :
        imageHandler_(imageHandler),
        configuration_(configurationFilePath) {}

    void loadData();

private:
    void loadKeypoints();
    Image loadImageData(std::string configString);

    Image targetImage_;
    Image referenceImage_;
    ImageHandler* imageHandler_;
    Configuration configuration_;
};

} //namespace

#endif
