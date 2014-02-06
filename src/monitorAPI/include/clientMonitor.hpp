#include "../../repaAPI/include/repaAPI.hpp"
#include <string>
#include <map>
#include <functional>

using namespace std;

template<class T> class ClientMonitor {
  private:
    int freq;
    bool isRunning;
    RepaAPI<T> repaAPI;
    vector<function<T()>> dataGenerators;
    void generators_runner();
  public: 
    ClientMonitor(int);
    void add_data_generator(function<T()>);
    void rm_data_generator(string);
    void set_freq(int);
    void close();
};
#include "clientMonitor.tpp"
