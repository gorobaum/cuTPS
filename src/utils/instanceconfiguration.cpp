#include "instanceconfiguration.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace tps {

void InstanceConfiguration::readConfigurations() {
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

bool InstanceConfiguration::isCallFeatureGenerator() {
    float percentage = getFloat("percentage");
    return (percentage != 0);
}

void InstanceConfiguration::printConfigs() {
    for (std::map<std::string,std::string>::iterator it=currentConfiguration_.begin(); it!=currentConfiguration_.end(); ++it)
        std::cout << "map[" << it->first << "] = " << it->second << std::endl;
}

std::map<std::string,std::string>::iterator InstanceConfiguration::findInConfiguration(std::string configurationName) {
    std::map<std::string,std::string>::iterator pair = currentConfiguration_.find(configurationName);
    if (pair == currentConfiguration_.end()) {
        std::cout << "The configuration \'" << configurationName <<
                     "\' was not found. Please check the configuration file over"
                     << configurationFilePath_ << std::endl;
    }
    return pair;
}

float InstanceConfiguration::getFloat(std::string configurationName) {
    std::string floatStr = findInConfiguration(configurationName)->second;
    return std::stod(floatStr);
}

std::string InstanceConfiguration::getString(std::string configurationName) {
    return findInConfiguration(configurationName)->second;
}

std::vector<std::vector<float>> InstanceConfiguration::getFloatVector(std::string configurationName) {
    std::vector<std::vector<float>> result;
    std::string vectorString = findInConfiguration(configurationName)->second;

    std::stringstream ss(vectorString);
    std::string item;
    while (std::getline(ss, item, ';')) {
        std::vector<float> auxVector = getFloatVectorFromString(item);
        result.push_back(auxVector);
    }

    return result;
}

std::vector<float> InstanceConfiguration::getFloatVectorFromString(std::string vectorString) {
    std::vector<float> vector;
    std::size_t pos = vectorString.find(',');

    std::stringstream ss(vectorString);
    std::string item;
    while (std::getline(ss, item, ','))
        vector.push_back(std::stod(item));

    return vector;
}

}
