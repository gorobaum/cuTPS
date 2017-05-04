#ifndef TPS_UTILS_INSTANCECONFIGURATION_H_
#define TPS_UTILS_INSTANCECONFIGURATION_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace tps {

class InstanceConfiguration {
public:
  InstanceConfiguration(std::string configurationFilePath):
    configurationFilePath_(configurationFilePath) {
        readConfigurations();
    };

    void printConfigs();
    void readConfigurations();
    bool isCallFeatureGenerator();
    float getFloat(std::string configurationName);
    std::string getString(std::string configurationName);
    bool hasConfiguration(std::string configurationName);
    std::vector<int> getIntVector(std::string configurationName);
    std::vector<std::vector<float>> getFloatVector(std::string configurationName);

private:
    std::vector<float> getFloatVectorFromString(std::string vectorString);
    std::unordered_map<std::string,std::string>::iterator findInConfiguration(std::string configurationName);

    std::string configurationFilePath_;
    std::unordered_map<std::string, std::string> currentConfiguration_;
};

} //namespace

#endif
