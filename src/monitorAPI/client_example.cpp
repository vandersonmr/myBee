#include "include/client_monitor.hpp"
#include "../include/data.hpp"

/* Randomize a temperature based on the sin function */
time_t timeNow = 0;
double aux = 0;
int GetTemperature() {
  srand(time(NULL));
  time(&timeNow); // Represent the time/clock
  aux += 0.1;
  if (aux > 6.3) aux = 0;

  return (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
}

string nickname;

Data GetData() { 
  // In this case is generate a randomic temperature 
  Data data; 
  data.type     = string("temperature"); 
  data.value    = GetTemperature(); 
  data.time     = timeNow; 
  data.nickname = nickname; 

  return data; 
}

int main(void) {
  nickname = string("node1");

  ClientMonitor<Data> cm(nickname, 15); // 15 - 15 Segundos
  cm.AddDataGenerator("temperature",GetData);

  while(true); // Gera dados para sempre;
}
