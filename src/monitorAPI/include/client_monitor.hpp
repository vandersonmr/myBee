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
    string name;
    RepaAPI<Data> repa_api;
    map<string, function<Data(void)>> data_generators;
    void GeneratorsRunner();
  public: 
    ClientMonitor(string,int);
    void AddDataGenerator(string, function<Data(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
};
