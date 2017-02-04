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
      tps::Image runNormal(int numberOfCPs);
      tps::Image calculateError(int numberOfCPs);
    private:
      void runThread(uint tid);
      void runThreadForError(uint tid, std::vector<std::vector<float>>& field);

      float* pgenerateDeforVectorAt(int x, int y, int z);
      float pnormOf(float x, float y, float z);
      float pcalculateSD(float meanError, std::vector<std::vector<float>> field,
          std::vector<int> dimensions);
      float pcalculateError(std::vector<std::vector<float>> field, std::vector<int> dimensions);


      uint numberOfThreads = std::thread::hardware_concurrency();
  };

} // namespace

#endif
