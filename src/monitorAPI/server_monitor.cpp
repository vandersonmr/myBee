#include "include/server_monitor.hpp"
#include "machineLearning.h"

void ServerMonitor::UpdateListOfNodesOnline() {
  vector<string> nodes = repa.GetNodesOnline();
  clearNodesOnline();
  for (string nodeOnline : nodes) {
    insertNodeOnline(nodeOnline);
  }
}

int ServerMonitor::CheckData(Data data) {
  return testData(data);
}

void ServerMonitor::HandleMessage(message<Data> msg) {
  for (Data data: msg.data){ 
    int status = CheckData(data); 
    data.node = msg.prefix_address; 

    saveData(data,status); 

    UpdateListOfNodesOnline(); 
  }
}

ServerMonitor::ServerMonitor() {
  vector<string> interests;
  interests.push_back(string("server"));
  
  if (!repa.InitRepa(interests)) throw;

  repa.GetMessage([this](message<Data> msg) { HandleMessage(msg); });
}

void ServerMonitor::SetFilter(function<Data(Data)> filter) {
  this->filter       = filter;
  this->has_a_filter = true;
}

int ServerMonitor::EnablePersistence(string config_path) {
  if (!connectDatabase()){
    return EXIT_FAILURE;
  }

  this->is_persistence_enable = true;
  return EXIT_SUCCESS;
}

void ServerMonitor::EnableMachineLearning(int sensible) {
  this->is_machine_learning_enable = true;
}

void ServerMonitor::Close() { 
  closeConnection();
  repa.CloseRepa();
}
