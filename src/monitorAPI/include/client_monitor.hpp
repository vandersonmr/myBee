#include "../../repaAPI/include/repaAPI.hpp"
#include "../../include/data.hpp"
#include <string>
#include <map>
#include <functional>

using namespace std;

class ClientMonitor {
  private:
    int freq;
    bool is_running;
    string node_name;
    RepaAPI<Data> repa_api;
    map<string, function<double(void)>> data_generators;
    Data GetData(string, double);
    void GeneratorsRunner();
  public: 
    ClientMonitor(string,int);
    void AddDataGenerator(string, function<double(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
};
