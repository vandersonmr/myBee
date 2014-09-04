#include "../repaAPI/include/repa_api.hpp"
#include "../include/data.hpp"
#include <string>
#include <map>
#include <functional>

using namespace std;

extern bool quit;

class ClientMonitor {
  private:
    int freq;
    bool is_running;
    string node_name;
    RepaAPI<Data> repa_api;
    void ParseArgs(int*, char**);
    void Usage();
    map<string, function<double(void)>> data_generators;
    Data GetData(string, double);
    void GeneratorsRunner();

  public: 
    ClientMonitor(int*, char**);
    ~ClientMonitor();
    void AddDataGenerator(string, function<double(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
    void Run();
    static void Handler(int);
};
