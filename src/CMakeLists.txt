set(CMAKE_CXX_FLAGS "-std=c++11")

# CMake version
cmake_minimum_required(VERSION 3.1)


# Project name
project(DetectionLigne)
# Used libraries
find_package(OpenCV REQUIRED)
# Source files
set(SOURCES PreProcess.h PreProcess.cpp  LineDetection.h LineDetection.cpp )
# Output files
add_executable(detectionLigne main.cpp ${SOURCES})
# Link libraries to output files
target_link_libraries(detectionLigne ${OpenCV_LIBS})


