#include <iostream>
#include "monitorAPI/server_monitor.hpp"

Data<double> filter(Data<double> d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << endl;
  return d;
}

int main(int argc, char** argv) {
  ServerMonitor<double> server(&argc, argv);
  server.EnablePersistence("../config/db.conf");
  server.SetFilter(&filter);
  server.Run();
  return 0;
}
