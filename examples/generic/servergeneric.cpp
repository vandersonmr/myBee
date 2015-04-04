#include <iostream>
#include "monitorAPI/server_monitor.hpp"
#include "generic.h"

#define TYPE Generic

Data<TYPE> filter(Data<TYPE> d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << 
    " Generic name: " << d.value << " Time: " << ctime(&d.time);
  return d;
}

void PackValue(Data<TYPE>& data, string value) {
  TYPE gen("command");
  gen.setType(3);
  gen.setCommand(value);
  data.value = gen;
}

int main(int argc, char** argv) {
  ServerMonitor<TYPE> server(&argc, argv);
  server.EnablePersistence("../../config/db.conf");
  server.SetFilter(&filter);
  server.TCPPackFunction(&PackValue);
  server.Run();
  return 0;
}
