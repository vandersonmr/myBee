#include "../repaAPI/include/repa_api.hpp"
#include "data.hpp"
#include "database/dataDAO.hpp"
#include "machineLearning/machineLearning.hpp"
#include <string>
#include <map>
#include <functional>

template <typename T>
class ServerMonitor {
  private:
    function<Data<T>(Data<T>)> filter;
    RepaAPI<Data<T>>        repa;

    DataDAO<T> dao;
    bool is_persistence_enable      = false;
    void UpdateListOfNodesOnline();

#ifdef ML
    MachineLearning<double> ml;
    int  CheckData(Data<T> data);
    bool is_machine_learning_enable = true;

  public:
    void EnableMachineLearning(int);
#endif

  private:

    bool has_a_filter               = false;
    bool is_running                 = false;
    bool is_time_client;

    void GetTime(Data<T>&);
    void ParseArgs(int*, char**);
    void Usage();

    void HandleMessage(message<Data<T>>);
    void InitMonitor();
    static void Handler(int);

  public: 
    ServerMonitor();
    ServerMonitor(int*, char**);
    ~ServerMonitor();
    void SetFilter(function<Data<T>(Data<T>)>);
    int  EnablePersistence(string);
    void Close();
    void Run();
    void GetTimeServer();
    void GetTimeClient();
};

static bool quit = false;

template <typename T>
void ServerMonitor<T>::HandleMessage(message<Data<T>> msg) {
  for (auto& data: msg.data){ 
    
    data.node = msg.prefix_address; 

    if (!is_time_client)
      GetTime(data);

    if (has_a_filter)
      data = filter(data);
      
    int status = 0;
      
#ifdef ML
    if (is_machine_learning_enable) 
      status = CheckData(data);
#endif

    if (is_persistence_enable) {
      dao.saveData(data, status);
    }
    
    UpdateListOfNodesOnline();

  }

  if (msg.has_ack) {
    string nickname = msg.data.back().nickname;
    msg.data.clear();
    msg.interests = {"client-" + nickname};
    repa.SendMessage(msg);
  }
}

template <typename T>
void ServerMonitor<T>::GetTime(Data<T>& data){
  time_t timenow;
  time(&timenow);
  data.time = timenow;
}

template <typename T>
ServerMonitor<T>::ServerMonitor(){
  InitMonitor();
}

template <typename T>
ServerMonitor<T>::ServerMonitor(int* argc, char** argv) {
  this->is_time_client = true;

  if (argc != NULL && argv != NULL) ParseArgs(argc, argv);
  else Usage();

  InitMonitor();
}

template <typename T>
void ServerMonitor<T>::InitMonitor(){
  std::vector<string> interests;
  interests.push_back(string("server"));

  if (!repa.InitRepa(interests)) exit(1);

  repa.GetMessage([this](message<Data<T>> msg) { HandleMessage(msg); });

  signal(SIGINT, ServerMonitor::Handler);

}

template <typename T>
void ServerMonitor<T>::Handler(int sig){
  cout << "Signal: " << sig << endl;
  quit = true;
}

template <typename T>
void ServerMonitor<T>::Run(){
  while (!quit) sleep(1);
}

template <typename T>
ServerMonitor<T>::~ServerMonitor(){
  Close();
}

template <typename T>
void ServerMonitor<T>::ParseArgs(int* argc, char** argv){
  int num_args = *argc;
  for (int i = 1; i < num_args; i++){
    string arg = string(argv[i]);
    if (arg[0] == '-') {
      if (arg[1] == 'c') GetTimeClient();
#ifdef ML
      else if (arg[1] == 'm'){
        if (++i < num_args && string(argv[i]).compare("disable"))
          EnableMachineLearning(1);
        else if (i < num_args && string(argv[i]).compare("enable"))
          EnableMachineLearning(0);
        else Usage();
      }
#endif
      else if (arg[1] == 's') GetTimeServer();
      else if (arg[1] == 'h') Usage();
      else Usage();
    }
    else Usage();
  }
}

template <typename T>
void ServerMonitor<T>::Usage(){
  cout << "Usage: ServerMonitor [Options]" << endl;
#ifdef ML
  cout << "\t-m disable/enable : Disable/Enable machine learning." << endl;
#endif
  cout << "\t-c                : Use time from client." << endl;
  cout << "\t-s                : Use time from server." << endl;
  exit(0);
}

template <typename T>
void ServerMonitor<T>::SetFilter(function<Data<T>(Data<T>)> filter) {
  this->filter       = filter;
  this->has_a_filter = true;
}

template <typename T>
int ServerMonitor<T>::EnablePersistence(string config_path) {
  if (!dao.connectDatabase(config_path)){
    return EXIT_FAILURE;
  }

  this->is_persistence_enable = true;
#ifdef ML
  ml.setDAO(this->dao);
#endif
  return EXIT_SUCCESS;
}

#ifdef ML
template <typename T>
void ServerMonitor<T>::EnableMachineLearning(int sensible) {
  if (sensible) {
    cout << "Machine learning is enable." << endl;
    this->is_machine_learning_enable = true;
  }
  else {
    cout << "Machine learning is disabled." << endl;
    this->is_machine_learning_enable = false;
  }
}

template <typename T>
int ServerMonitor<T>::CheckData(Data<T> data) {
  return ml.testData(data);
}
#endif

template <typename T>
void ServerMonitor<T>::UpdateListOfNodesOnline() {
  std::vector<string> nodes = repa.GetNodesOnline();
  dao.clearNodesOnline();
  for (string nodeOnline : nodes) {
    dao.insertNodeOnline(nodeOnline);
  }
}

template <typename T>
void ServerMonitor<T>::GetTimeServer(){
  cout << "Getting time from server." << endl;
  this->is_time_client = false;
}

template <typename T>
void ServerMonitor<T>::GetTimeClient(){
  cout << "Getting time from client." << endl;
  this->is_time_client = true;
}

template <typename T>
void ServerMonitor<T>::Close() {
  dao.closeConnection();
  repa.CloseRepa();
}
