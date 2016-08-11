#include "configuration.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace tps {

void Configuration::setUpDefaultConf() {
    defaultConfiguration_["referenceImage"] = "/home/goroba/reps/tps-c/images/grid-Ref.png";
    defaultConfiguration_["targetImage"] = "/home/goroba/reps/tps-c/images/grid-Tar.png";
    defaultConfiguration_["percentage"] = "2";
    defaultConfiguration_["keypoints"] = "2619, 1927; 2457, 2162; 2292, 2344; 2799, 1682; 2942, 1489; 3097, 1267; 3242, 1067; 2209, 1659; 2372, 1719; 2882, 2102; 3074, 2214; 2314, 1429; 3207, 1867";
    defaultConfiguration_["boundaries"] = "1, 512; 1, 512; 1, 1;";
}

void Configuration::readConfigurations() {
    std::ifstream infile;

    infile.open(configurationFilePath_.c_str());

    std::string line;
    while (std::getline(infile, line)) {
        std::size_t pos = line.find(':');
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos+1);

        std::pair<std::string, std::string> newConfigEntry(key, value);
        currentConfiguration_.insert(newConfigEntry);
    }
}

void Configuration::printConfigs() {
    for (std::map<std::string,std::string>::iterator it=currentConfiguration_.begin(); it!=currentConfiguration_.end(); ++it)
        std::cout << "map[" << it->first << "] = " << it->second << std::endl;
}

std::map<std::string,std::string>::iterator Configuration::findInConfiguration(std::string configurationName) {
    std::map<std::string,std::string>::iterator pair = currentConfiguration_.find(configurationName);
    if (pair == currentConfiguration_.end()) {
        pair = defaultConfiguration_.find(configurationName);
    }
    return pair;
}

double Configuration::getDouble(std::string configurationName) {
    std::string doubleStr = findInConfiguration(configurationName)->second;
    return std::stod(doubleStr);
}

std::string Configuration::getString(std::string configurationName) {
    return findInConfiguration(configurationName)->second;
}

std::vector<std::vector<double>> Configuration::getDoubleVector(std::string configurationName) {
    std::vector<std::vector<double>> result;
    std::string vectorString = findInConfiguration(configurationName)->second;

    std::stringstream ss(vectorString);
    std::string item;
    while (std::getline(ss, item, ';')) {
        std::vector<double> auxVector = getDoubleVectorFromString(item);
        result.push_back(auxVector);
    }
    
    return result;
}

std::vector<double> Configuration::getDoubleVectorFromString(std::string vectorString) {
    std::vector<double> vector;
    std::size_t pos = vectorString.find(',');

    std::stringstream ss(vectorString);
    std::string item;
    while (std::getline(ss, item, ','))
        vector.push_back(std::stod(item));

    return vector;
}

}
