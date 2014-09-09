#include "../repaAPI/include/repa_api.hpp"
#include "../include/data.hpp"
#include <string>
#include <unordered_map>
#include <functional>

using namespace std;

#define TIMEOUT_TIME 30
#define MAX_MESSAGES 100

class ClientMonitor {
  private:
    int freq;
    bool is_running;
    uint32_t id;
    string node_name;
    bool is_ack_enable = false;
    unordered_map<uint32_t, message<Data>> message_list;
    RepaAPI<Data> repa_api;
    void ParseArgs(int*, char**);
    void Usage();
    map<string, function<double(void)>> data_generators;
    map<string, Type> data_generators_type;
    Data GetData(string, double);
    void GeneratorsRunner();
    void InitMonitor();
    void ResendLostMessages();
    void HandleMessage(message<Data>&);
    static void Handler(int);

  public:
    /**@brief Create a ClientMonitor
     * @param Name is a string used to identify this client on the server
     * @param Freq is the frequence that data is generated and sent to the server
     */
    ClientMonitor(string, int);
    ClientMonitor(int*, char**);
    ~ClientMonitor();
    void AddDataGenerator(string, function<double(void)>);
    void AddDataGenerator(string, Type  , function<double(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
    void EnableACK(bool);
    void Run();
};
