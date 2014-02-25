#include "../../repaAPI/include/repa_api.hpp"
#include "../../include/data.hpp"
#include "../../include/dataDAO.hpp"
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

    int  CheckData(Data data);
    void UpdateListOfNodesOnline();
    void HandleMessage(message<Data>);
  public: 
    ServerMonitor();
    void SetFilter(function<Data(Data)>);
    int  EnablePersistence(string);
    void EnableMachineLearning(int);
    void Close();
};
