set(CMAKE_CXX_FLAGS "-std=c++11")

# CMake version
cmake_minimum_required(VERSION 3.1)




# Project name
project(DetectionLigne)
# Used libraries
find_package(OpenCV REQUIRED)

set(Boost_ADDITIONAL_VERSIONS "1.69" "1.69.0")
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}) 
# Source files
set(SOURCES PreProcess.h PreProcess.cpp  LineDetection.h LineDetection.cpp )
# Output files
add_executable(detectionLigne main.cpp ${SOURCES})
# Link libraries to output files
target_link_libraries(detectionLigne ${OpenCV_LIBS} ${Boost_LIBRARIES})


