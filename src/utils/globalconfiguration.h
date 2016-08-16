#ifndef TPS_UTILS_GLOBALCONFIGURATION_H_
#define TPS_UTILS_GLOBALCONFIGURATION_H_

#include <map>
#include <string>
#include <vector>

namespace tps {

class GlobalConfiguration {
public:
    static GlobalConfiguration& getInstance() {
        static GlobalConfiguration instace;
        return instace;
    }

    void loadConfigurationFile(std::string configurationFile);

    bool isCuda();
    int getInt(std::string key);
    bool getBoolean(std::string key);
    std::string getString(std::string key);
    std::vector<std::string> getTargetFiles() {return targetFiles_;};

private:
    GlobalConfiguration() {};
    GlobalConfiguration(GlobalConfiguration const&);
    void operator=(GlobalConfiguration const&);

    std::map<std::string,std::string>::iterator findInConfiguration(
                                                std::string configurationName);

    std::vector<std::string> targetFiles_;
    std::map<std::string, std::string> currentConfiguration_;
};

} //namespace

#endif
