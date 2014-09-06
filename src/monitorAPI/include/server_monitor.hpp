#include "../repaAPI/include/repa_api.hpp"
#include "../include/data.hpp"
#include "../include/dataDAO.hpp"
#include <string>
#include <map>
#include <functional>

using namespace std;

class ServerMonitor {
  private:
    function<Data(Data)> filter;
    RepaAPI<Data>        repa;

    bool is_persistence_enable      = false;
    bool has_a_filter               = false;
    bool is_machine_learning_enable = false;
    bool is_running                 = false;
    bool is_time_client;

    int  CheckData(Data data);
    void GetTime(Data&);
    void ParseArgs(int*, char**);
    void Usage();

    void UpdateListOfNodesOnline();
    void HandleMessage(message<Data>);
    void InitMonitor();
    static void Handler(int);

  public: 
    ServerMonitor();
    ServerMonitor(int*, char**);
    ~ServerMonitor();
    void SetFilter(function<Data(Data)>);
    int  EnablePersistence(string);
    void EnableMachineLearning(int);
    void Close();
    void Run();
    void GetTimeServer();
    void GetTimeClient();
};
