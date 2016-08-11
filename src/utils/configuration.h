#ifndef TPS_UTILS_CONFIGURATION_H_
#define TPS_UTILS_CONFIGURATION_H_

#include <map>
#include <string>
#include <vector>

namespace tps {

class Configuration {
public:
  Configuration(std::string configurationFilePath):
    configurationFilePath_(configurationFilePath) {
        setUpDefaultConf();
        readConfigurations();
    };

    void printConfigs();
    void readConfigurations();
    double getDouble(std::string configurationName);
    std::string getString(std::string configurationName);
    std::vector<std::vector<double>> getDoubleVector(std::string configurationName);

private:
    void setUpDefaultConf();
    std::vector<double> getDoubleVectorFromString(std::string vectorString);
    std::map<std::string,std::string>::iterator findInConfiguration(std::string configurationName);

    std::string configurationFilePath_;
    std::map<std::string, std::string> currentConfiguration_;
    std::map<std::string, std::string> defaultConfiguration_;
};

} //namespace

#endif
