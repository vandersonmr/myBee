cmake_minimum_required (VERSION 2.8)
project (APIMonitor)

set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -Wall -Wextra -Werror ${CMAKE_CXX_FLAGS} -g")

include_directories("${PROJECT_SOURCE_DIR}/")
include_directories("${PROJECT_SOURCE_DIR}/../include")

add_library(ML STATIC
  KCentroid.cpp
  limit.cpp
  Krls.cpp
  machineLearning.cpp
  ManipulateData.cpp)
