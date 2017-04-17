#ifndef TPS_TPS_H_
#define TPS_TPS_H_

#include "image/image.h"
#include "linearsystem/cplinearsystems.h"
#include "image/itkimagehandler.h"

#include <vector>

namespace tps {

class TPS {
public:
    TPS(std::vector< std::vector<float> > referenceKeypoints,
        std::vector< std::vector<float> > targetKeypoints,
        tps::Image targetImage, tps::Image referenceImage) :
            referenceKeypoints_(referenceKeypoints),
            targetKeypoints_(targetKeypoints),
            targetImage_(targetImage),
            referenceImage_(referenceImage),
            dimensions_(targetImage.getDimensions()),
            registredImage(targetImage.getDimensions()) {};

  virtual tps::Image run() = 0;
  void setLinearSystemSolutions(std::vector<float> solutionX,
                                 std::vector<float> solutionY,
                                 std::vector<float> solutionZ) {
                                    solutionX_ = solutionX;
                                    solutionY_ = solutionY;
                                    solutionZ_ = solutionZ;
                                 };

protected:
    float computeRSquared(float x, float xi, float y, float yi, float z, float zi)
        {return pow(x-xi,2) + pow(y-yi,2) + pow(z-zi,2);};

    tps::Image targetImage_;
    tps::Image registredImage;
    tps::Image referenceImage_;
    std::vector<int> dimensions_;
    std::vector<float> solutionX_;
    std::vector<float> solutionY_;
    std::vector<float> solutionZ_;
    std::vector< std::vector<float> > targetKeypoints_;
    std::vector< std::vector<float> > referenceKeypoints_;
};

} // namespace

#endif
