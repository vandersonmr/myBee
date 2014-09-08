cmake_minimum_required (VERSION 2.8)
project (APIMonitor)

include(ExternalProject)

set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -Wall -Wextra -Werror ${CMAKE_CXX_FLAGS} -g")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/repa/repa.so
  COMMAND make
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/repa
  )

add_custom_target(
  extern_lib_repa
  DEPENDS ${PROJECT_SOURCE_DIR}/repa/repa.so
  )

externalproject_add(ML
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/machineLearning
  INSTALL_COMMAND ""
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/machineLearning)

add_library(monitorapi STATIC
      client_monitor.cpp
      server_monitor.cpp
      database/dataDAO.cpp)

target_link_libraries(monitorapi ${PROJECT_SOURCE_DIR}/repa/repa.so ${PROJECT_SOURCE_DIR}/machineLearning/libML.a)

add_dependencies(monitorapi extern_lib_repa)
