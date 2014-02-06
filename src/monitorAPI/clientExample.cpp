#include "include/ClientMonitor.hpp"

/* Randomize a temperature based on the sin function */
time_t timeNow = 0;
double aux = 0;
int getTemperature() {
  srand(time(NULL));
  time(&timeNow); // Represent the time/clock
  aux += 0.1;
  if (aux > 6.3) aux = 0;

  return (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
}


Data getData() { 
  // In this case is generate a randomic temperature 
  Data data; 
  data.type     = string("temperature"); 
  data.value    = getTemperature(); 
  data.time     = timeNow; 
  data.nickname = string(nickname); 

  return data; 
}

void main(void) {
  ClientMonitor cm(string("node1"), 15); // 15 - 15 Segundos
  cm.add_data_generator(getData);

  while(true); // Gera dados para sempre;
}
