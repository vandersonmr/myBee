#include "../repaAPI/include/repa_api.hpp"
#include "data.hpp"
#include "database/dataDAO.hpp"
#include "machineLearning.h"
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>

template <typename T>
class ServerMonitor {
  private:
    function<Data<T>(Data<T>)> filter;
    RepaAPI<Data<T>>        repa;

    DataDAO<T> dao;
    bool is_persistence_enable      = false;
    void UpdateListOfNodesOnline();
    int fd_tcp, port;
    struct sockaddr_in server_tcp;
    void AcceptTCP();
    void ReceiveTCPMessage(int);
    function<void(Data<T>&, string)> PackValueFunction;
    void SendMessage(string, string);

#ifdef ML
    int  CheckData(Data<T> data);
    bool is_machine_learning_enable = true;

  public:
    /**@brief Enable the machine learning algorithms to be applied on the
     * received data. Note that for this to work it must be enabled during
     * compilation time.
     * @param sensible 0 to disable and any number different from 0 to enable.
     */
    void EnableMachineLearning(int);
#endif

  private:

    bool has_a_filter               = false;
    bool is_running                 = false;
    bool is_time_client             = false;

    void GetTime(Data<T>&);
    void ParseArgs(int*, char**);
    void Usage();

    void HandleMessage(message<Data<T>>);
    void InitMonitor();
    static void Handler(int);

  public:
    /**@brief Creates a ServerMonitor instance.
     */
    ServerMonitor();

    /**@brief Creates a ServerMonitor instance, but it parses the commands passed
     * on command line. Useful to set some variables during execution.
     * @param argc the amount of arguments passed.
     * @param argv an array containing all the arguments.
     */
    ServerMonitor(int*, char**);
    ~ServerMonitor();
    void SetFilter(function<Data<T>(Data<T>)>);
    int  EnablePersistence(string);
    void EnableTCP(int);
    void Close();
    void Run();

    /**@brief Set the time to be taken from the server. When the message
     * is received it discards the time from the client and use the one from
     * the server.
     * This is useful to define which time will be saved on database.
     * @see SetTimeClient()
     */
    void SetTimeServer();

    /**@brief It uses the time received from the client. 
     * @see SetTimeServer()
     */
    void SetTimeClient();

    void TCPPackFunction(function<void(Data<T>&, string)>);
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
    if (is_machine_learning_enable) { 
      status = CheckData(data);
    }
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
      if (arg[1] == 'c') SetTimeClient();
#ifdef ML
      else if (arg[1] == 'm'){
	if (++i < num_args && string(argv[i]).compare("disable"))
	  EnableMachineLearning(1);
	else if (i < num_args && string(argv[i]).compare("enable"))
	  EnableMachineLearning(0);
	else Usage();
      }
#endif
      else if (arg[1] == 's') SetTimeServer();
      else if (arg[1] == 'h') Usage();
      else if (string(argv[i]) == "-tcp") {
	if (i + 1 == num_args || argv[i + 1][0] == '-')
	  Usage();
	else
	  EnableTCP(atoi(argv[++i]));
      }
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
  cout << "\t-tcp [port]       : Enable TCP connection. Insert port." << endl;
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
  return testData(data, dao);
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
void ServerMonitor<T>::SetTimeServer(){
  cout << "Getting time from server." << endl;
  this->is_time_client = false;
}

template <typename T>
void ServerMonitor<T>::SetTimeClient(){
  cout << "Getting time from client." << endl;
  this->is_time_client = true;
}

template <typename T>
void ServerMonitor<T>::Close() {
  dao.closeConnection();
  repa.CloseRepa();
  if (fd_tcp >= 0) close(fd_tcp);
}

template <typename T>
void ServerMonitor<T>::ReceiveTCPMessage(int fd) {
  char buffer[1024];
  while (!quit) {
    int recv_val = recv(fd, buffer, 1024, 0);
    if (recv_val < 0)
      cerr << "Error receiving message" << endl;
    else if (recv_val == 0) {
      cout << "Connection closed" << endl;
      close(fd);
      return;
    }
    else {
      string word;
      vector<string> elems;

      stringstream ss;

      ss << string(buffer);

      while (getline(ss, word, '&')) elems.push_back(word);

      SendMessage(elems[0], elems[1]);

    }
  }
}



template <typename T>
void ServerMonitor<T>::SendMessage(string nickname, string value) {
  vector<Data<T>> datas;
  Data<T> data;
  time_t timenow;
  time(&timenow);

  data.type = string("command");
  data.definedType.sensor = Type::None;
  PackValueFunction(data, value);
  data.nickname = nickname;
  data.time = timenow;
  datas.push_back(data);

  vector<string> interests = {nickname};

  message<Data<T>> msg;
  msg.data = datas;
  msg.interests = interests;
  msg.time = timenow;
  msg.has_ack = false;

  repa.SendMessage(msg);
}

template <typename T>
void ServerMonitor<T>::AcceptTCP() {
  while (!quit) {
    thread (&ServerMonitor::ReceiveTCPMessage, this, 
	accept(fd_tcp, NULL, NULL)).detach();
  }
}

template <typename T>
void ServerMonitor<T>::EnableTCP(int port) {
  if ((fd_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    cerr << "Error opening socket" << endl;
    return;
  }

  server_tcp.sin_family	     = AF_INET;
  server_tcp.sin_port	     = htons(port);
  server_tcp.sin_addr.s_addr = INADDR_ANY;

  int yes = 1;

  if (setsockopt(fd_tcp, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    cerr << "Error setting socket reuse." << endl;
    return;
  }

  if (bind(fd_tcp, (struct sockaddr *) &server_tcp, sizeof(server_tcp)) < 0) {
    cerr << "Error binding socket" << endl;
    return;
  }

  listen(fd_tcp, 5);

  cout << "Listening on port " << port << endl;

  thread(&ServerMonitor::AcceptTCP, this).detach();

}

template <typename T>
void ServerMonitor<T>::TCPPackFunction(function<void(Data<T>&, string)> callback) {
  PackValueFunction = callback;
}
