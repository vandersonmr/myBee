#include "include/server_monitor.hpp"
#include "include/machineLearning.h"

bool quit = false;

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
  for (auto& data: msg.data){ 
    int status = 0;

    if (!is_time_client)
      GetTime(data);

    if (has_a_filter)
      data = filter(data);

    if (is_machine_learning_enable) 
      status = CheckData(data);

    data.node = msg.prefix_address; 

    if (is_persistence_enable)
      saveData(data,status); 

    UpdateListOfNodesOnline(); 
  }
}

void ServerMonitor::GetTime(Data& data){
  time_t timenow;
  time(&timenow);
  data.time = timenow;
}

ServerMonitor::ServerMonitor(int* argc, char** argv) {
  this->is_machine_learning_enable = true;
  this->is_time_client = true;

  if (argc != NULL && argv != NULL) ParseArgs(argc, argv);
  else Usage();

  vector<string> interests;
  interests.push_back(string("server"));

  if (!repa.InitRepa(interests)) exit(1);

  repa.GetMessage([this](message<Data> msg) { HandleMessage(msg); });

  signal(SIGINT, ServerMonitor::Handler);
}

void ServerMonitor::Handler(int sig){
  cout << "Signal: " << sig << endl;
  quit = true;
}

void ServerMonitor::Run(){
  while (!quit) sleep(1);
}

ServerMonitor::~ServerMonitor(){
  Close();
}

void ServerMonitor::ParseArgs(int* argc, char** argv){
  int num_args = *argc;
  for (int i = 1; i < num_args; i++){
    string arg = string(argv[i]);
    if (arg[0] == '-') {
      if (arg[1] == 'm'){
        if (++i < num_args && string(argv[i]).compare("disable"))
          EnableMachineLearning(1);
        else if (i < num_args && string(argv[i]).compare("enable"))
          EnableMachineLearning(0);
        else Usage();
      }
      else if (arg[1] == 'c') GetTimeClient();
      else if (arg[1] == 's') GetTimeServer();
      else if (arg[1] == 'h') Usage();
      else Usage();
    }
    else Usage();
  }
}

void ServerMonitor::Usage(){
  cout << "Usage: ServerMonitor [Options]" << endl;
  cout << "\t-m disable/enable : Disable/Enable machine learning." << endl;
  cout << "\t-c                : Use time from client." << endl;
  cout << "\t-s                : Use time from server." << endl;
  cout << "By default machine learning is enable and time is from client." << endl;
  exit(0);
}

void ServerMonitor::SetFilter(function<Data(Data)> filter) {
  this->filter       = filter;
  this->has_a_filter = true;
}

int ServerMonitor::EnablePersistence(string config_path) {
  if (!connectDatabase(config_path)){
    return EXIT_FAILURE;
  }

  this->is_persistence_enable = true;
  return EXIT_SUCCESS;
}

void ServerMonitor::EnableMachineLearning(int sensible) {
  if (sensible) {
    cout << "Machine learning is enable." << endl;
    this->is_machine_learning_enable = true;
  }
  else {
    cout << "Machine learning is disabled." << endl;
    this->is_machine_learning_enable = false;
  }
}

void ServerMonitor::GetTimeServer(){
  cout << "Getting time from server." << endl;
  this->is_time_client = false;
}

void ServerMonitor::GetTimeClient(){
  cout << "Getting time from client." << endl;
  this->is_time_client = true;
}

void ServerMonitor::Close() {
  closeConnection();
  repa.CloseRepa();
}
