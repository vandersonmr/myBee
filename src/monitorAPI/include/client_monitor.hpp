#include "../repaAPI/include/repa_api.hpp"
#include "../include/data.hpp"
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
    /**@brief Create a ClientMonitor
     * @param Name is a string used to identify this client on the server
     * @param Freq is the frequence that data is generated and sent to the server
     */
    ClientMonitor(string,int);
    void AddDataGenerator(string, function<double(void)>);
    void RmDataGenerator(string);
    void SetFreq(int);
    void Close();
};
