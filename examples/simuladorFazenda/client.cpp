#include "monitorAPI/include/client_monitor.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

ifstream* tempFile;
ifstream* humFile;
string temp;
string hum;

double GetTemperature() {
  getline(*tempFile, temp);
  istringstream iss(temp);
  string sub;
  double t = 0;
  while(sub != string("temperature")) 
    iss >> sub;
  iss >> t;
  return t;
}

double GetHumidity(){
  getline(*humFile, hum);
  istringstream iss(hum);
  string sub;
  double h = 0;
  while(sub != string("humidity")) 
    iss >> sub;
  iss >> h;
  return h;
}

int main(int argc, char **argv) {
  ClientMonitor monitor(&argc, argv);
  tempFile = new ifstream("temp_abelhas.txt");
  humFile  = new ifstream("hum_abelhas.txt");
  getline(*tempFile, temp); // throw away the header
  getline(*humFile , hum); // throw away the header

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);

  monitor.Run();

  return 0;
}
