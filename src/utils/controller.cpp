#include "controller.h"
#include "cudamemory.h"
#include "globalconfiguration.h"

struct ThreadData {
  int tId;
  int endPoint;
  int startPoint;
  std::vector<tps::RunInstance> executionInstances;
};

#define MAXRUNNINGINSTANCES 4

namespace tps {

  void Controller::exec() {
    bool isCuda = GlobalConfiguration::getInstance().isCuda();
    double totalGpuMemory = CudaMemory::getGpuMemory() * 0.7;
    std::vector<RunInstance> loadedInstances;

    // GlobalConfiguration::getInstance().printConfigs();

    while (lastInstaceLoaded_ < runInstances_.size()) {
      for (lastInstaceLoaded_; lastInstaceLoaded_ < runInstances_.size(); lastInstaceLoaded_++) {
        if ((lastInstaceLoaded_ - lastInstanceExecuted_) >= MAXRUNNINGINSTANCES) break;
        runInstances_[lastInstaceLoaded_].loadData();
        if (isCuda) {
          double currentUsedMemory = CudaMemory::getUsedGpuMemory();
          double estimatedMemory = runInstances_[lastInstaceLoaded_].getEstimateGpuMemory();
          std::string imageName = runInstances_[lastInstaceLoaded_].getImageName();
          std::cout << "Allocating memory for " << imageName << std::endl;
          std::cout << "Total GPU memory = " << totalGpuMemory << std::endl;
          std::cout << "Current GPU memory = " << currentUsedMemory << std::endl;
          std::cout << "Estimated GPU memory = " << estimatedMemory << std::endl;
          if (estimatedMemory > totalGpuMemory) {
            std::cout << "GPU memory isn't big enough!" << std::endl;
            exit(-1);
          }
          estimatedMemory += currentUsedMemory;
          if (estimatedMemory < totalGpuMemory) {
            runInstances_[lastInstaceLoaded_].allocCudaMemory();
          } else {
            break;
          }
        }
      }

      runLoadedInstances();
    }
    std::cout << "==========================================" << std::endl;
  }

  void Controller::runLoadedInstances() {
    bool parallel = GlobalConfiguration::getInstance().getBoolean("parallelExecution");

    if (parallel) {
      runMultipleProcess();
    } else {
      runSingleProcess();
    }
  }

  void Controller::runSingleProcess() {
    for (lastInstanceExecuted_; lastInstanceExecuted_ < lastInstaceLoaded_; lastInstanceExecuted_++) {
      runInstances_[lastInstanceExecuted_].solveLinearSystem();
      runInstances_[lastInstanceExecuted_].executeTps();
    }
  }

  void Controller::runMultipleProcess() {
    int numberOfThreads = GlobalConfiguration::getInstance().getInt("numberOfThreads");
    pthread_t threads[numberOfThreads];

    for (int i = 0; i < numberOfThreads; i++) {
      ThreadData* data = new ThreadData;
      data->tId = i;
      data->endPoint = lastInstaceLoaded_;
      data->startPoint = lastInstanceExecuted_;
      data->executionInstances = runInstances_;
      if (pthread_create(&threads[i], NULL, runThread, static_cast<void*>(data))) {
        fprintf(stderr, "Error creating thread");
      }
    }

    for (int i = 0; i < numberOfThreads; i++) {
      if(pthread_join(threads[i], NULL)) {
        fprintf(stderr, "Error joining thread");
      }
    }
    lastInstanceExecuted_ = lastInstaceLoaded_;
  }

  void Controller::runThread(void* data) {
    int numberOfThreads = GlobalConfiguration::getInstance().getInt("numberOfThreads");
    ThreadData* threadData = static_cast<ThreadData*>(data);

    int lastExec = threadData->startPoint;
    int lastLoaded = threadData->endPoint;
    int i = lastExec + threadData->tId;

    for (i; i < lastLoaded; i += numberOfThreads) {
      threadData->executionInstances[i].solveLinearSystem();
      threadData->executionInstances[i].executeTps();
    }
  }

  void Controller::readConfigurationFile(std::string masterConfigFilePath) {
    GlobalConfiguration::getInstance().loadConfigurationFile(masterConfigFilePath);

    std::string referenceImagePath = GlobalConfiguration::getInstance().getString("referenceImage");

    // Based on the reference image name, load the correct ImageHandler and
    // load the reference image data to the memory
    ImageHandler* imageHandler = getCorrectImageHandler(referenceImagePath);
    Image referenceImage = imageHandler->loadImageData(referenceImagePath);

    std::vector<std::string> targetFiles = GlobalConfiguration::getInstance().getTargetFiles();

    for (std::vector<std::string>::iterator it = targetFiles.begin(); it != targetFiles.end(); it++) {
      RunInstance newRunInstance(*it, referenceImage, imageHandler);
      runInstances_.push_back(newRunInstance);
    }
  }

  ImageHandler* Controller::getCorrectImageHandler(std::string referenceImagePath) {
    std::size_t pos = referenceImagePath.rfind('.');
    std::string extension = referenceImagePath.substr(pos);

    tps::ImageHandler *imageHandler;
    if ((extension.compare(".nii") == 0) || (extension.compare(".gz") == 0)) {
      imageHandler = new tps::ITKImageHandler();
    } else {
      imageHandler = new tps::OPCVImageHandler();
    }

    return imageHandler;
  }


} //namespace
