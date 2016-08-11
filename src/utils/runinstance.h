#ifndef TPS_UTILS_RUNINSTANCE_H_
#define TPS_UTILS_RUNINSTANCE_H_

#include <string>

#include "image/image.h"
#include "configuration.h"
#include "image/imagehandler.h"
#include "feature/featuregenerator.h"

namespace tps {

class RunInstance {
public:
    RunInstance(std::string configurationFilePath, Image referenceImage,
                ImageHandler* imageHandler) :
        referenceImage_(referenceImage),
        imageHandler_(imageHandler),
        configuration_(configurationFilePath) {}

    void loadData();

private:
    void loadKeypoints();
    void generateKeypoints();
    void generateKeypointImage();
    Image loadImageData(std::string configString);

    ImageHandler* imageHandler_;
    Configuration configuration_;

    Image targetImage_;
    Image referenceImage_;
    std::vector< std::vector<float> > targetKeypoints_;
    std::vector< std::vector<float> > referenceKeypoints_;
};

} //namespace

#endif
