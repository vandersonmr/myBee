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
    vector<string> interests = {"server"};
    bool is_ack_enable = false;
    bool send_message_for_each_generator = false;
    unordered_map<uint32_t, message<Data<T>>> message_list;
    RepaAPI<Data<T>> repa_api;
    void ParseArgs(int*, char**);
    void Usage();
    map<string, function<T(void)>> data_generators;
    map<string, Type> data_generators_type;
    map<string, int> data_generators_freq;
    Data<T> GetData(string, T);
    void GeneratorsRunner();
    void GeneratorRunner(string);
    void InitMonitor();
    void ResendLostMessages();
    void HandleMessage(message<Data<T>>&);
    message<Data<T>> CreateMessage(vector<Data<T>>);
    function<void(T)>* filter_server_message = nullptr;
    static void Handler(int);

  public:
    /**@brief Create a ClientMonitor instance.
     * @param node_Name is a string used to identify this client on the server
     * @param freq is the frequence in seconds that data is generated 
     * and sent to the server
     */
    ClientMonitor(string, int);
    
    /**@brief Create a ClientMonitor instance. This can be used to automatically
     * initiate the function to parse the command line arguments. It is used like
     * a helper showing which commands the user can pass.
     * @param argc the amount of arguments passed on command line.
     * @param argv array containing the arguments passed to the program.
     */
    ClientMonitor(int*, char**);

    /**@brief Destroys the instance and close the communication.
     * @see Close()
     */
    ~ClientMonitor();

    /**@brief Add a generator to get data to send to the server. The amount of
     * time is specified by the freq
     * @see SetFreq()
     * @param name the name of the data generator.
     * @param callback the function used to generate the data.
     */
    void AddDataGenerator(string, function<T(void)>);

    /**@see AddDataGenerator(string, function<T(void>)
     * @param freq the time in seconds which will generate the data to send
     * to server
     */
    void AddDataGenerator(string, int, function<T(void)>);

    /**@see AddDataGenerator(string, function<T(<void>)
     * @param type the type of the generator in which the data is been generated.
     * For example, if the data is the type of temperature, set the type like this
     * Type::Temperature.
     */
    void AddDataGenerator(string, Type  , function<T(void)>);

    /**@brief Remove a generator given the name of the specific generator.
     * @param name name of the generator.
     */
    void RmDataGenerator(string);

    /**@brief By default it is sent one message for all the generators. This
     * method gives the possibility to send a message for each generator. It
     * is important when the interval time for each generator is different.
     * @param val true to send for each or false to send one message for all.
     */
    void SendMessageForEachGenerator(bool);

    /**@brief The interval time that all the generators will be getting
     * the data.
     * @param freq interval time in seconds.
     */
    void SetFreq(int);

    /**@brief Set the interest which all the messages will be sent.
     * @param interest an array containing all the interest. By default
     * the interest is already set on "server".
     */
    void SetInterest(vector<string>);

    /**@brief Close the repa communication and the main flux is unblocked
     * from the Run() method
     */
    void Close();

    /**@brief If after 30 seconds there is not a response from the server
     * that the message has arrived, it resends it. You can change the time
     * on the TIMEOUT_TIME variable
     * @param value true to enable and false to disable.
     */
    void EnableACK(bool);

    /**@brief This method is used for the execution not terminate. After
     * everything has been initialized, it left threads generating data,
     * so the main flux needs to be blocked to not terminate.
     */
    void Run();

    /**@brief Apply some filter for the messages received from the server.
     * If you don't want to do anything with the messages received from server,
     * just leave this alone.
     * @param callback the function that will be executed on the particular message.
     * It works like a filter
     */
    void HandleServerMessages(function<void(T)>);
};

static bool quit = false;

template <typename T>
void ClientMonitor<T>::SetInterest(vector<string> interests) {
  for (string node: interests)
    this->interests.push_back(node);
}

template <typename T>
void ClientMonitor<T>::SendMessageForEachGenerator(bool val) {
  send_message_for_each_generator = val;
}

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

    repa_api.SendMessage(CreateMessage(data));
  }
}

template <typename T>
message<Data<T>> ClientMonitor<T>::CreateMessage(vector<Data<T>> data) {
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

  return msg;
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
void ClientMonitor<T>::GeneratorRunner(string name) {
  int freq = this->freq;

  if (data_generators_freq.find(name) != data_generators_freq.end())
    freq = data_generators_freq[name];

  while (is_running) {
    this_thread::sleep_for(chrono::seconds(freq));

    vector<Data<T>> data;
    Data<T> d = GetData(name, data_generators[name]());
    d.definedType.sensor = data_generators_type[name];
    data.push_back(d);

    repa_api.SendMessage(CreateMessage(data));
  }
}

template <typename T>
void ClientMonitor<T>::Run(){
  if (!send_message_for_each_generator)
    thread (&ClientMonitor::GeneratorsRunner, this).detach();
  else {
    for (auto& generator: data_generators)
      thread(&ClientMonitor::GeneratorRunner, this, generator.first).detach();
  }
  if (this->is_ack_enable) {
    repa_api.GetMessage([this](message<Data<T>> msg) { HandleMessage(msg); });
    thread(&ClientMonitor::ResendLostMessages, this).detach();
  }
  if (filter_server_message != nullptr) {
    repa_api.GetMessage([this](message<Data<T>> msg) 
	{ (*filter_server_message)(msg.data.back().value); });
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
void ClientMonitor<T>::AddDataGenerator(string name, int freq,
    function<T(void)> callback) {

  data_generators[name]      = callback;
  data_generators_type[name] = Type::None; 
  data_generators_freq[name] = freq; 
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

template <typename T>
void ClientMonitor<T>::HandleServerMessages(function<void(T)> callback) {
   filter_server_message = &callback;
}
