#include <chrono>
#include <ctime>
#include <thread>
#include "include/client_monitor.hpp"

bool quit = false;

Data ClientMonitor::GetData(string type, double value){
  time_t timenow;
  Data data;
  data.type = type;
  data.value = value;
  time(&timenow);
  data.time = timenow;
  data.nickname = node_name;
  return data;
}

void ClientMonitor::GeneratorsRunner() {
  while (is_running) { 
    if (data_generators.size() == 0)
      continue;

    vector<Data> data;
    for (auto generator : data_generators) { 
      data.push_back(GetData(generator.first,generator.second()));
    }

    vector<string> interests = {"server"}; 

    message<Data> msg;
    msg.data      = data;
    msg.interests = interests;

    repa_api.SendMessage(msg);  

    this_thread::sleep_for(chrono::seconds(freq));
  }
}

ClientMonitor::ClientMonitor(int* argc, char** argv) {
  if (argc != NULL && argv != NULL) ParseArgs(argc, argv);
  else {
    cout << "Using default:" << endl;
    cout << "Name: default" << endl;
    cout << "Frequency: 1s" << endl;
    this->freq = 1;
    this->node_name  = "default"; 
  }

  this->is_running = true;

  vector<string> interests = {"client"};

  if (!repa_api.InitRepa(interests)) exit(1);
  
  signal(SIGINT, ClientMonitor::Handler);
}

void ClientMonitor::Handler(int sig){
  cout << "Signal: " << sig << endl;
  quit = true;
}

ClientMonitor::~ClientMonitor(){
  Close();
}

void ClientMonitor::Run(){
  thread (&ClientMonitor::GeneratorsRunner,this).detach();
  while (!quit) sleep(1);
}

void ClientMonitor::ParseArgs(int* argc, char** argv){
  int num_args = *argc;
  if (num_args <= 1) Usage();
  else {
    for (int i = 1; i < num_args; i++){
      string arg = string(argv[i]);
      if (arg[0] == '-') {
        if (arg[1] == 'i'){
          if (++i < num_args)
            this->freq = atoi(argv[i]);
          else Usage();
        }
        else if (arg[1] == 'n'){
          if (++i < num_args)
            this->node_name = string(argv[i]);
          else Usage();
        }
        else if (arg[1] == 'h') Usage();
      }
      else Usage();
    }
    if (this->node_name.empty()){
      cout << "Using default name." << endl;
      this->node_name = "default";
    }
    if (this->freq == 0){
      cout << "Using 1 second for frequency." << endl;
      this->freq = 1;
    }
  }
}

void ClientMonitor::Usage(){
  cout << "Usage: ClientMonitor [Options]" << endl;
  cout << "\t-i time : Interval time in seconds to send data." << endl;
  cout << "\t-n name : Client name." << endl;
  cout << "\t-h      : Display this message." << endl;
  exit(1);
}

void ClientMonitor::AddDataGenerator(string name, function<double(void)> callback) {
  data_generators[name] = callback;
}

void ClientMonitor::RmDataGenerator(string name) {
  data_generators.erase(name);
}

void ClientMonitor::SetFreq(int freq) {
  this->freq = freq;
}

void ClientMonitor::Close() {
  this->is_running = false;
  repa_api.CloseRepa();
}
