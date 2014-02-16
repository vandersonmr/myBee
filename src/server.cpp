#include "monitorAPI/include/server_monitor.hpp"

Data filter(Data d) {
  cout << "\" Prefix: " <<
        d.nickname << " send a msg\n\n";
  return d;
} 

int main(void) {
  ServerMonitor server;
  server.EnablePersistence("config/db.conf");
  server.EnableMachineLearning(1);
  server.SetFilter(&filter);
  while(true) { sleep(1); };
}
