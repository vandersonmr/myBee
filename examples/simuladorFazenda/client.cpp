#include "monitorAPI/client_monitor.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#define TYPE double

ifstream* tempFile;
ifstream* humFile;
string temp;
string hum;

TYPE GetTemperature() {
  getline(*tempFile, temp);
  istringstream iss(temp);
  string sub;
  TYPE t = 0;
  while(sub != string("temperature")) 
    iss >> sub;
  iss >> t;
  return t;
}

TYPE GetHumidity(){
  getline(*humFile, hum);
  istringstream iss(hum);
  string sub;
  TYPE h = 0;
  while(sub != string("humidity")) 
    iss >> sub;
  iss >> h;
  return h;
}

int main(int argc, char **argv) {
  ClientMonitor<TYPE> monitor(&argc, argv);
  tempFile = new ifstream("temp_abelhas.txt");
  humFile  = new ifstream("hum_abelhas.txt");
  getline(*tempFile, temp); // throw away the header
  getline(*humFile , hum); // throw away the header

  monitor.AddDataGenerator("temperature", Type::Temperature, &GetTemperature);
  monitor.AddDataGenerator("humidity"   , Type::Humidity   ,&GetHumidity);

  monitor.Run();

  return 0;
}
