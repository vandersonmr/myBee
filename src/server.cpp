#include <mysql.h>
#include <iostream>

#include "machineLearning.h"
#include "dataDAO.hpp"
#include "hdr/repa.h"
#include "repaAPI/include/repaAPI.hpp"

RepaAPI<Data> repa;

int checkTemperature(Data data){
  return testData(data);
}

void updateListOfNodesOnline() {
  vector<string> nodes = repa.get_nodes_online();
  clearNodesOnline();
  for (string nodeOnline : nodes) {
    insertNodeOnline(nodeOnline); 
  }
}

void handle_message(message<Data> msg) {       
     
   int status;
   status = checkTemperature(msg.data);
   cout << "message: " << msg.data.value << endl;
   
   msg.data.node = msg.prefix_address;
   saveData(msg.data,status);
   
   std::cout << "Message: \"" << msg.data.value << "\" Status: \"" <<
        status << "\" Prefix: " << msg.data.nickname << endl;
      
   updateListOfNodesOnline();
}

int main(void) {

  if (!connectDatabase()){
    return EXIT_FAILURE;
  }

  vector<string> interests;
  interests.push_back(string("server"));
  repa.init_repa(interests);

  repa.get_message(&handle_message);

  while(true) sleep(1);

  closeConnection();
  repa.close_repa();
  exit(EXIT_SUCCESS);
}
