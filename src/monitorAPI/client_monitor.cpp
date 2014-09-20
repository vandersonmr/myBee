#include <chrono>
#include <ctime>
#include <thread>
#include "include/client_monitor.hpp"

static bool quit = false;

Data ClientMonitor::GetData(string type, double value){
  time_t timenow;
  Data data; 
  data.type = type;
  data.definedType.sensor = Type::None;
  data.value = value;
  time(&timenow);
  data.time     = timenow;
  data.nickname = node_name;
  return data;
}

void ClientMonitor::GeneratorsRunner() {
  while (is_running) { 
    if (data_generators.size() == 0)
      continue;

    this_thread::sleep_for(chrono::seconds(freq));
    
    vector<Data> data;
    for (auto generator : data_generators) { 
      Data d = GetData(generator.first, generator.second());
      d.definedType.sensor = data_generators_type[generator.first];
      data.push_back(d);
    }

    vector<string> interests = {"server"}; 

    time_t time_now;
    time(&time_now);
    message<Data> msg;
    msg.data      = data;
    msg.interests = interests;
    msg.time      = time_now;
    msg.has_ack   = this->is_ack_enable;

    if (msg.has_ack) {
      msg.id = this->id;
      message_list[this->id++ % MAX_MESSAGES] = msg;
    }

    repa_api.SendMessage(msg);  
  }
}

ClientMonitor::ClientMonitor(string node_name, int freq) {
  this->freq = freq;
  this->node_name = node_name;
  InitMonitor();
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
  InitMonitor();
}

void ClientMonitor::InitMonitor() {
  this->is_running = true;
  this->id = 0;

  vector<string> interests = {"client", "client-" + this->node_name};

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

void ClientMonitor::HandleMessage(message<Data>& msg) {
  message_list.erase(msg.id % MAX_MESSAGES);
  cout << "Message " << msg.id << " has been received." << endl;
}

void ClientMonitor::Run(){
  thread (&ClientMonitor::GeneratorsRunner,this).detach();
  if (this->is_ack_enable) {
    repa_api.GetMessage([this](message<Data> msg) { HandleMessage(msg); });
    thread(&ClientMonitor::ResendLostMessages, this).detach();
  }
  while (!quit) sleep(1);
}

void ClientMonitor::ResendLostMessages() {
  while (is_running) {
    for (auto& value: message_list) {
      message<Data> msg = value.second;
      uint32_t time_diff = time(NULL) - msg.time;
      if (time_diff > TIMEOUT_TIME) {
        msg.time = time(NULL);
        repa_api.SendMessage(msg);
      }
    }
    this_thread::sleep_for(chrono::seconds(1));
  }
}

void ClientMonitor::EnableACK(bool value) {
  this->is_ack_enable = value;
}

void ClientMonitor::AddDataGenerator(string name, 
                                              function<double(void)> callback) {

  data_generators[name]      = callback;
  data_generators_type[name] = Type::None; 
}

void ClientMonitor::AddDataGenerator(string name, Type type, 
                                              function<double(void)> callback) {

  data_generators[name]      = callback;
  data_generators_type[name] = type; 
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
        else if (arg[1] == 'a') EnableACK(true);
        else Usage();
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
  cout << "\t-a      : Enable ACK to resend messages to the server." << endl;
  cout << "\t-h      : Display this message." << endl;
  exit(0);
}
