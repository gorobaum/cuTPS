#ifndef TPS_UTILS_INSTANCECONFIGURATION_H_
#define TPS_UTILS_INSTANCECONFIGURATION_H_

#include <map>
#include <string>
#include <vector>

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
    std::vector<std::vector<float>> getFloatVector(std::string configurationName);

private:
    std::vector<float> getFloatVectorFromString(std::string vectorString);
    std::map<std::string,std::string>::iterator findInConfiguration(std::string configurationName);

    std::string configurationFilePath_;
    std::map<std::string, std::string> currentConfiguration_;
};

} //namespace

#endif
