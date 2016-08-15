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
        instanceConfiguration_(configurationFilePath) {
            isDone_ = false;
        }

    void loadData();
    void executeTps();
    void allocCudaMemory();
    void solveLinearSystem();

    bool isDone() {return isDone_;};
    double getEstimateGpuMemory();

private:
    void loadKeypoints();
    void generateKeypoints();
    void generateKeypointImage();

    void allocateGpuMemory();
    void solveLinearSystemWithCuda();
    void solveLinearSystemWithArmadillo();

    Image executeCudaTps();
    Image executeBasicTps();
    Image executeParallelTps();

    Image loadImageData(std::string configString);

    CudaMemory cudaMemory_;
    ImageHandler* imageHandler_;
    InstanceConfiguration instanceConfiguration_;

    bool isDone_;
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
