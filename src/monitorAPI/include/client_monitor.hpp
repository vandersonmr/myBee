#include "../../repaAPI/include/repaAPI.hpp"
#include <string>
#include <map>
#include <functional>

using namespace std;

template<class T> class ClientMonitor {
  private:
    int freq;
    bool is_running;
    string name;
    RepaAPI<T> repa_api;
    map<string, function<T(void)>> data_generators;
    void GeneratorsRunner();
  public: 
    ClientMonitor(string,int);
    void AddDataGenerator(string, function<T(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
};
#include "client_monitor.tpp"
