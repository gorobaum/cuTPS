#ifndef TPS_TPS_BASICTPS_H_
#define TPS_TPS_BASICTPS_H_

#include "tps.h"
#include "linearsystem/armalinearsystems.h"

namespace tps {

class BasicTPS : public TPS {
public:
    using TPS::TPS;

    tps::Image run();
};

} // namespace

#endif
