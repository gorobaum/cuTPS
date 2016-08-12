#ifndef TPS_TPS_PARALLELTPS_H_
#define TPS_TPS_PARALLELTPS_H_

#include "tps.h"
#include "linearsystem/armalinearsystems.h"
#include "linearsystem/cudalinearsystems.h"
#include "utils/cudamemory.h"

#include <thread>

namespace tps {

class ParallelTPS : public TPS {
public:
    using TPS::TPS;

    tps::Image run();

private:
    void runThread(uint tid);

    uint numberOfThreads = std::thread::hardware_concurrency();
};

} // namespace

#endif
