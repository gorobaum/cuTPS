
#include <vector>
#include <iostream>

#include "utils/configuration.h"

int main(int argc, char** argv) {
  if (argc < 1) {
    std::cout << "The correct usage is:" << std::endl;
    std::cout << "/t ./tps <configuration file>" << std::endl;
    return 0;
  }

  tps::Configuration configuration(argv[1]);

  configuration.printConfigs();
  std::cout << configuration.getString("referenceImage") << std::endl;
  std::cout << configuration.getString("targetImage") << std::endl;
  std::cout << configuration.getDouble("percentage") << std::endl;

  std::vector<std::vector<double>> keypoints = configuration.getDoubleVector("keypoints");

  for (int x = 0; x < keypoints.size(); x++) {
        for (int y = 0; y < keypoints[x].size(); y++)
            std::cout << keypoints[x][y] << " ";
      std::cout << std::endl;
  }

  //
  // boundaries

  return 0;
}
