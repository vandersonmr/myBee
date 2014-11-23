#include <iostream>
#include "monitorAPI/server_monitor.hpp"
#include "generic.h"

#define TYPE string

Data<TYPE> filter(Data<TYPE> d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << 
    " Generic name: " << d.value << " Time: " << ctime(&d.time);
  return d;
}

int main(int argc, char** argv) {
  ServerMonitor<TYPE> server(&argc, argv);
  server.EnablePersistence("../../config/db.conf");
  server.SetFilter(&filter);
  server.Run();
  return 0;
}
