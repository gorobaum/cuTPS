#ifndef TPS_UTILS_RUNINSTANCE_H_
#define TPS_UTILS_RUNINSTANCE_H_

#include <string>

#include "cudamemory.h"
#include "image/image.h"
#include "image/imagehandler.h"
#include "instanceconfiguration.h"
#include "feature/featuregenerator.h"
#include "linearsystem/cplinearsystems.h"

namespace tps {

class RunInstance {
public:
    RunInstance(std::string configurationFilePath, Image referenceImage,
                ImageHandler* imageHandler) :
        referenceImage_(referenceImage),
        imageHandler_(imageHandler),
        instanceConfiguration_(configurationFilePath) {}

    void loadData();
    void executeTps();
    void solveLinearSystem();

private:
    void loadKeypoints();
    void generateKeypoints();
    void generateKeypointImage();

    void allocateGpuMemory();
    void solveLinearSystemWithCuda();
    void solveLinearSystemWithArmadillo();

    void executeCudaTps();
    void executeBasicTps();
    void executeParallelTps();

    Image loadImageData(std::string configString);

    CudaMemory cudaMemory_;
    ImageHandler* imageHandler_;
    InstanceConfiguration instanceConfiguration_;

    Image targetImage_;
    Image referenceImage_;
    std::vector<float> solutionX_;
    std::vector<float> solutionY_;
    std::vector<float> solutionZ_;
    std::vector< std::vector<float> > targetKeypoints_;
    std::vector< std::vector<float> > referenceKeypoints_;
};

} //namespace

#endif
