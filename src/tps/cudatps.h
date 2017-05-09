#ifndef TPS_TPS_CUDATPS_H_
#define TPS_TPS_CUDATPS_H_

#include "tps.h"
#include "linearsystem/cudalinearsystems.h"
#include "linearsystem/armalinearsystems.h"
#include "utils/cudamemory.h"

namespace tps {

class CudaTPS : public TPS {
public:
    using TPS::TPS;

    tps::Image run();
    void setCudaMemory(tps::CudaMemory cm);
    void setModifier(std::string mod) { mod_ = mod; }

private:
    std::string mod_;
    tps::CudaMemory cm_;
};

}

#endif
