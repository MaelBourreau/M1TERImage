# CMake version
cmake_minimum_required(VERSION 2.8)
# Project name
project(DetectionLigne)
# Used libraries
find_package(OpenCV REQUIRED)
# Source files
set(SOURCES PreProcess.h PreProcess.cpp)
# Output files
add_executable(detectionLigne main.cpp ${SOURCES})
# Link libraries to output files
target_link_libraries(detectionLigne ${OpenCV_LIBS})
