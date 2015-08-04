#include <iostream>
#include "monitorAPI/server_monitor.hpp"
#include "generic.h"

#define TYPE Generic

string ip;
vector<string> emails = {"joao29a@gmail.com"};

string& operator+(string& str, const vector<string>& vet) {
  for (uint32_t i = 0; i < vet.size(); i++) {
    str += vet[i];
    if (i < vet.size() - 1)
      str += ",";
  }
  return str;
}

Data<TYPE> filter(Data<TYPE> d) {
  cout << " Prefix: " <<
    d.nickname << " send a msg " << d.type << 
    " Generic name: " << d.value << " Time: " << ctime(&d.time);
  if (d.value.getType() == 5 && d.value.getIP() != ip) {
    string message = "IP: " + d.value.getIP();
    string command = "echo '" + message + "' | mail -s 'IP Address' ";
    system((command + emails).c_str());
    ip = d.value.getIP();
  }
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
