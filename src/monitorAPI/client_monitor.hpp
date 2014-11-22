#include "../repaAPI/include/repa_api.hpp"
#include "data.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;

#define TIMEOUT_TIME 30
#define MAX_MESSAGES 100

template <typename T>
class ClientMonitor {
  private:
    int freq;
    bool is_running;
    uint32_t id;
    string node_name;
    bool is_ack_enable = false;
    unordered_map<uint32_t, message<Data<T>>> message_list;
    RepaAPI<Data<T>> repa_api;
    void ParseArgs(int*, char**);
    void Usage();
    map<string, function<T(void)>> data_generators;
    map<string, Type> data_generators_type;
    Data<T> GetData(string, T);
    void GeneratorsRunner();
    void InitMonitor();
    void ResendLostMessages();
    void HandleMessage(message<Data<T>>&);
    static void Handler(int);

  public:
    /**@brief Create a ClientMonitor
     * @param Name is a string used to identify this client on the server
     * @param Freq is the frequence that data is generated and sent to the server
     */
    ClientMonitor(string, int);
    ClientMonitor(int*, char**);
    ~ClientMonitor();
    void AddDataGenerator(string, function<T(void)>);
    void AddDataGenerator(string, Type  , function<T(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
    void EnableACK(bool);
    void Run();
};

static bool quit = false;

template <typename T>
Data<T> ClientMonitor<T>::GetData(string type, T value){
  time_t timenow;
  Data<T> data; 
  data.type = type;
  data.definedType.sensor = Type::None;
  data.value = value;
  time(&timenow);
  data.time     = timenow;
  data.nickname = node_name;
  return data;
}

template <typename T>
void ClientMonitor<T>::GeneratorsRunner() {
  while (is_running) { 
    if (data_generators.size() == 0)
      continue;

    this_thread::sleep_for(chrono::seconds(freq));

    vector<Data<T>> data;
    for (auto generator : data_generators) { 
      Data<T> d = GetData(generator.first, generator.second());
      d.definedType.sensor = data_generators_type[generator.first];
      data.push_back(d);
    }
    
    vector<string> interests = {"server"};

    time_t time_now;
    time(&time_now);
    message<Data<T>> msg;
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

template <typename T>
ClientMonitor<T>::ClientMonitor(string node_name, int freq) {
  this->freq = freq;
  this->node_name = node_name;
  InitMonitor();
}

template <typename T>
ClientMonitor<T>::ClientMonitor(int* argc, char** argv) {
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

template <typename T>
void ClientMonitor<T>::InitMonitor() {
  this->is_running = true;
  this->id = 0;

  vector<string> interests = {"client", "client-" + this->node_name};

  if (!repa_api.InitRepa(interests)) exit(1);

  signal(SIGINT, ClientMonitor::Handler);
}

template <typename T>
void ClientMonitor<T>::Handler(int sig){
  cout << "Signal: " << sig << endl;
  quit = true;
}

template <typename T>
ClientMonitor<T>::~ClientMonitor(){
  Close();
}

template <typename T>
void ClientMonitor<T>::HandleMessage(message<Data<T>>& msg) {
  message_list.erase(msg.id % MAX_MESSAGES);
  cout << "Message " << msg.id << " has been received." << endl;
}

template <typename T>
void ClientMonitor<T>::Run(){
  thread (&ClientMonitor::GeneratorsRunner,this).detach();
  if (this->is_ack_enable) {
    repa_api.GetMessage([this](message<Data<T>> msg) { HandleMessage(msg); });
    thread(&ClientMonitor::ResendLostMessages, this).detach();
  }
  while (!quit) sleep(1);
}

template <typename T>
void ClientMonitor<T>::ResendLostMessages() {
  while (is_running) {
    for (auto& value: message_list) {
      message<Data<T>> msg = value.second;
      uint32_t time_diff = time(NULL) - msg.time;
      if (time_diff > TIMEOUT_TIME) {
        msg.time = time(NULL);
        repa_api.SendMessage(msg);
      }
    }
    this_thread::sleep_for(chrono::seconds(1));
  }
}

template <typename T>
void ClientMonitor<T>::EnableACK(bool value) {
  this->is_ack_enable = value;
}

template <typename T>
void ClientMonitor<T>::AddDataGenerator(string name, 
    function<T(void)> callback) {

  data_generators[name]      = callback;
  data_generators_type[name] = Type::None; 
}

template <typename T>
void ClientMonitor<T>::AddDataGenerator(string name, Type type, 
    function<T(void)> callback) {

  data_generators[name]      = callback;
  data_generators_type[name] = type; 
}

template <typename T>
void ClientMonitor<T>::RmDataGenerator(string name) {
  data_generators.erase(name);
}

template <typename T>
void ClientMonitor<T>::SetFreq(int freq) {
  this->freq = freq;
}

template <typename T>
void ClientMonitor<T>::Close() {
  this->is_running = false;
  repa_api.CloseRepa();
}

template <typename T>
void ClientMonitor<T>::ParseArgs(int* argc, char** argv){
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

template <typename T>
void ClientMonitor<T>::Usage(){
  cout << "Usage: ClientMonitor [Options]" << endl;
  cout << "\t-i time : Interval time in seconds to send data." << endl;
  cout << "\t-n name : Client name." << endl;
  cout << "\t-a      : Enable ACK to resend messages to the server." << endl;
  cout << "\t-h      : Display this message." << endl;
  exit(0);
}
