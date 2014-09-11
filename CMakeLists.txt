cmake_minimum_required (VERSION 2.8)
project (APIMonitor)

include(ExternalProject)

set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -Wall -Wextra -Werror ${CMAKE_CXX_FLAGS} -g")
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/monitorAPI")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/repa/librepa.so
  COMMAND make
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/repa
  )

add_custom_target(
  extern_lib_repa
  DEPENDS ${PROJECT_SOURCE_DIR}/repa/librepa.so
  )

add_library(repa STATIC IMPORTED)
set_target_properties(repa PROPERTIES IMPORTED_LOCATION /usr/lib/librepa.so )

include_directories("${PROJECT_SOURCE_DIR}/machineLearning/")

add_library(ML STATIC
  ${PROJECT_SOURCE_DIR}/machineLearning/KCentroid.cpp
  ${PROJECT_SOURCE_DIR}/machineLearning/limit.cpp
  ${PROJECT_SOURCE_DIR}/machineLearning/Krls.cpp
  ${PROJECT_SOURCE_DIR}/machineLearning/machineLearning.cpp
  ${PROJECT_SOURCE_DIR}/machineLearning/ManipulateData.cpp)

add_library(monitorapiClient STATIC
      ${PROJECT_SOURCE_DIR}/client_monitor.cpp)

add_library(monitorapiServer STATIC
  ${PROJECT_SOURCE_DIR}/server_monitor.cpp
  ${PROJECT_SOURCE_DIR}/database/dataDAO.cpp)

target_link_libraries(monitorapiServer ML)

target_link_libraries(monitorapiClient)

add_dependencies(monitorapiServer ML extern_lib_repa)
add_dependencies(monitorapiClient extern_lib_repa)

include_directories("./src/")

############## SERVER ################
add_executable(server examples/server.cpp)

target_link_libraries(server monitorapiServer repa pthread python2.7 mysqlclient msgpack)

add_dependencies(server monitorapiServer repa)

############## RANDOM ################
add_executable(clientrandom examples/random/client.cpp)

target_link_libraries(clientrandom monitorapiClient repa pthread python2.7 mysqlclient msgpack)

add_dependencies(clientrandom monitorapiClient repa)

############## SimuladorFazenda ################
add_executable(clientsim examples/simuladorFazenda/client.cpp)

target_link_libraries(clientsim monitorapiClient repa pthread python2.7 mysqlclient msgpack)

add_dependencies(clientsim monitorapiClient repa)

############## RASP  ################
if(EXISTS "/opt/vc/include/bcm_host.h")
  add_executable(client examples/raspberry/client.cpp
      examples/raspberry/lib/dht/dht.cpp examples/raspberry/lib/dht22/dht22.cpp)

  include_directories("./example/raspberry/")

  target_link_libraries(client monitorapiClient repa pthread python2.7 mysqlclient msgpack wiringPi)

  add_dependencies(client monitorapiClient repa)
  
  INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/client DESTINATION ${PROJECT_SOURCE_DIR}/../../examples/raspberry/)

endif()

INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/server DESTINATION ${PROJECT_SOURCE_DIR}/../../examples/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientrandom DESTINATION ${PROJECT_SOURCE_DIR}/../../examples/random/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientsim DESTINATION ${PROJECT_SOURCE_DIR}/../../examples/simuladorFazenda/)
