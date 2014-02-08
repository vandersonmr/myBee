#include <mysql.h>
#include <iostream>

#include "machineLearning.h"
#include "dataDAO.hpp"
#include "hdr/repa.h"
#include "repaAPI/include/repaAPI.hpp"

RepaAPI<Data> repa;

int checkData(Data data){
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

  for (Data data: msg.data){
    int status;
    status = checkData(data);
    cout << "message: " << data.value;

    data.node = msg.prefix_address;
    saveData(data,status);

    cout <<" \" Status: \"" << status << "\" Prefix: " << 
      data.nickname << "\n\n";

    updateListOfNodesOnline();
  }
}

int main(void) {

  if (!connectDatabase()){
    return EXIT_FAILURE;
  }

  vector<string> interests;
  interests.push_back(string("server"));
  
  if (!repa.init_repa(interests)) return EXIT_FAILURE;

  repa.get_message(&handle_message);

  while(true) sleep(1);

  closeConnection();
  repa.close_repa();
  exit(EXIT_SUCCESS);
}
