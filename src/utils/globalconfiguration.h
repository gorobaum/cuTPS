#ifndef TPS_UTILS_GLOBALCONFIGURATION_H_
#define TPS_UTILS_GLOBALCONFIGURATION_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace tps {

class GlobalConfiguration {
public:
    static GlobalConfiguration& getInstance() {
        static GlobalConfiguration instace;
        return instace;
    }

    void loadConfigurationFile(std::string configurationFile);

    bool isCuda();
    void printConfigs();
    int getInt(std::string key);
    bool getBoolean(std::string key);
    std::string getString(std::string key);
    std::vector<std::string> getTargetFiles() {return targetFiles_;};

private:
    GlobalConfiguration() {};
    GlobalConfiguration(GlobalConfiguration const&);
    void operator=(GlobalConfiguration const&);

    std::unordered_map<std::string,std::string>::iterator findInConfiguration(
                                                std::string configurationName);

    std::vector<std::string> targetFiles_;
    std::unordered_map<std::string, std::string> currentConfiguration_;
};

} //namespace

#endif
