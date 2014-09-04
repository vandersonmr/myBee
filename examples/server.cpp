#include <iostream>
#include "monitorAPI/include/server_monitor.hpp"

Data filter(Data d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << endl;
  return d;
}

int main(int argc, char** argv) {
  ServerMonitor server(&argc, argv);
  server.EnablePersistence("../config/db.conf");
  server.SetFilter(&filter);
  server.Run();
  return 0;
}
