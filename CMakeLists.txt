cmake_minimum_required (VERSION 2.8)
project (APIMonitor)

include(ExternalProject)

set(CMAKE_CXX_FLAGS "-std=c++11 -O0 -Wall -Wextra ${CMAKE_CXX_FLAGS} -g")
set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/monitorAPI")

include_directories("${PROJECT_SOURCE_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/database/")

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/repa/repa.so
  COMMAND make
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/repa
  )

add_custom_target(
  extern_lib_repa
  DEPENDS ${PROJECT_SOURCE_DIR}/repa/repa.so
  )

add_library(repa STATIC IMPORTED)
set_target_properties(repa PROPERTIES IMPORTED_LOCATION /usr/lib/repa.so )

include_directories("${PROJECT_SOURCE_DIR}/machineLearning/")

include_directories("${PROJECT_SOURCE_DIR}/repaAPI/include/msgpack-c/include/")

add_library(ML STATIC
 ${PROJECT_SOURCE_DIR}/machineLearning/KCentroid.cpp
 ${PROJECT_SOURCE_DIR}/machineLearning/limit.cpp
 ${PROJECT_SOURCE_DIR}/machineLearning/Krls.cpp
 ${PROJECT_SOURCE_DIR}/machineLearning/machineLearning.cpp
 ${PROJECT_SOURCE_DIR}/machineLearning/ManipulateData.cpp)

target_link_libraries(ML)

include_directories("./src/")

############## SERVER ################
set_source_files_properties(examples/server.cpp PROPERTIES COMPILE_FLAGS -DML)

add_executable(server examples/server.cpp)

target_link_libraries(server repa pthread python2.7 mysqlclient ML)

add_dependencies(server repa)

############## RANDOM ################
add_executable(clientrandom examples/random/client.cpp)

target_link_libraries(clientrandom repa pthread python2.7 mysqlclient)

add_dependencies(clientrandom repa)

############## SimuladorFazenda ################
add_executable(clientsim examples/simuladorFazenda/client.cpp)

target_link_libraries(clientsim repa pthread python2.7 mysqlclient)

add_dependencies(clientsim repa)

############## RASP  ################
if(EXISTS "/opt/vc/include/bcm_host.h")
  add_executable(clientrasp examples/raspberry/client.cpp
      examples/raspberry/lib/bcm2835-dht/BcmDHT.cpp)

  include_directories("./example/raspberry/")

  target_link_libraries(clientrasp repa pthread python2.7
      mysqlclient bcm2835)

  add_dependencies(clientrasp repa)
  
  INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientrasp DESTINATION
    ${PROJECT_SOURCE_DIR}/../../examples/raspberry/)

endif()

############## GENERIC ################
add_executable(clientgeneric examples/generic/clientgeneric.cpp)

target_link_libraries(clientgeneric repa pthread python2.7 mysqlclient)

add_dependencies(clientgeneric repa)

add_executable(servergeneric examples/generic/servergeneric.cpp)

target_link_libraries(servergeneric repa pthread python2.7 mysqlclient)

add_dependencies(servergeneric repa)

INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/server DESTINATION 
  ${PROJECT_SOURCE_DIR}/../../examples/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientrandom DESTINATION 
  ${PROJECT_SOURCE_DIR}/../../examples/random/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientsim DESTINATION 
  ${PROJECT_SOURCE_DIR}/../../examples/simuladorFazenda/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/clientgeneric DESTINATION 
  ${PROJECT_SOURCE_DIR}/../../examples/generic/)
INSTALL(PROGRAMS ${PROJECT_BINARY_DIR}/servergeneric DESTINATION 
  ${PROJECT_SOURCE_DIR}/../../examples/generic/)
