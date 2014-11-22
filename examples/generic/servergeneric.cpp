#include <iostream>
#include "monitorAPI/server_monitor.hpp"
#include "generic.h"

#define TYPE Generic

Data<TYPE> filter(Data<TYPE> d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << 
    " Generic name: " << d.value.getName() << endl;
  return d;
}

int main(int argc, char** argv) {
  ServerMonitor<TYPE> server(&argc, argv);
  server.SetFilter(&filter);
  server.Run();
  return 0;
}
