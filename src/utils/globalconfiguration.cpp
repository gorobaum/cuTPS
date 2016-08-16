#include "globalconfiguration.h"

#include <fstream>
#include <iostream>

namespace tps {

void GlobalConfiguration::loadConfigurationFile(std::string configurationFile) {
    std::ifstream infile;

    infile.open(configurationFile.c_str());

    std::string line;
    while (std::getline(infile, line)) {
        std::size_t pos = line.find(':');
        if (pos == std::string::npos) {
            targetFiles_.push_back(line);
        } else {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos+1);

            std::pair<std::string, std::string> newConfigEntry(key, value);
            currentConfiguration_.insert(newConfigEntry);
        }
    }
}

bool GlobalConfiguration::isCuda() {
    std::string solverConfig = getString("linearSystemSolver");
    std::string tpsConfig = getString("tps");

    bool cuda = (solverConfig.compare("cuda") == 0) || (tpsConfig.compare("cuda") == 0);

    return cuda;
}

std::map<std::string,std::string>::iterator GlobalConfiguration::findInConfiguration(std::string configurationName) {
    std::map<std::string,std::string>::iterator pair = currentConfiguration_.find(configurationName);

    if (pair == currentConfiguration_.end()) {
        std::cout << "The configuration \'" << configurationName <<
                     "\' was not found." << std::endl;
    }

    return pair;
}

bool GlobalConfiguration::getBoolean(std::string key) {
    return (getString(key).compare("true") == 0);
}

std::string GlobalConfiguration::getString(std::string key) {
    return findInConfiguration(key)->second;
}

} // namespace
