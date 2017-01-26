cmake_minimum_required (VERSION 2.8)

project (tps)

find_package(OpenCV REQUIRED)
find_package(Armadillo REQUIRED)
find_package(CUDA REQUIRED)
find_package(ITK REQUIRED)

include(${ITK_USE_FILE})

include_directories(src "${ARMADILLO_INCLUDE_DIRS}")

set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -Xptxas -v -Xcompiler -std=c++98")

cuda_add_executable(tps
  src/main.cpp

  src/feature/featuregenerator.cpp

  src/image/image.cpp
  src/image/itkimagehandler.cpp
  src/image/opcvimagehandler.cpp
  src/image/imagedeformation.cpp

  src/linearsystem/cplinearsystems.cpp
  src/linearsystem/cudalinearsystems.cpp
  src/linearsystem/armalinearsystems.cpp

  src/tps/basictps.cpp
  src/tps/paralleltps.cpp
  src/tps/cudatps.cpp
  src/tps/cutps.cu

  src/utils/cudamemory.cpp
  src/utils/controller.cpp
  src/utils/runinstance.cpp
  src/utils/globalconfiguration.cpp
  src/utils/instanceconfiguration.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -fpermissive -include cstddef")

# TODO: Fix CUDA_cusolver_LIBRARY in FindCUDA.cmake
target_link_libraries(tps
    ${ITK_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OpenCV_LIBS}
    ${CUDA_cusolver_LIBRARY}
    ${CUDA_CUBLAS_LIBRARIES}
    ${ARMADILLO_LIBRARIES})

message(STATUS ${CUDA_cusolver_LIBRARY})

add_custom_target (
	cleanResults
	COMMAND ${CMAKE_COMMAND} -E remove result-*
)

add_custom_target(
  cleanConfigurations
  COMMAND ${CMAKE_COMMAND} -E remove configuration/tps.config* tps.configs
)
