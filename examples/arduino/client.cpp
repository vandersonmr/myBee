#include "monitorAPI/include/client_monitor.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#define TYPE double

ClientMonitor *tempMonitor;
/* Randomize a temperature based on the sin function */
TYPE aux  = 0;
TYPE aux1 = 0;
TYPE aux2 = 0;

TYPE GetTemperature() {
  FILE *arduino = fopen("/dev/ttyUSB0", "r");
  char buffer[1024];
  fgets(buffer, 1024, arduino);
  float temp;
  sscanf(buffer, "%f", &temp);
  return (TYPE) temp;
}

TYPE GetHumidity(){
  srand(time(NULL));
  aux1 += 0.1;
  if (aux1 > 6.3) aux1 = 0;
  TYPE humidity = (sin(aux1)*8+26) + rand() % 2 - 1; // Rand add some noise
  return humidity;
}

TYPE GetPressure(){
  srand(time(NULL));
  aux2 += 0.1;
  if (aux2 > 6.3) aux2 = 0;
  TYPE pressure = (sin(aux2)*12+26) + rand() % 2 - 1; // Rand add some noise
  return pressure;
}

void handler(int sig){
    cout << "Signal " << sig << endl;
    tempMonitor->Close();
}

int main(int argc, char **argv) {
  char* nickname = argv[argc-1];

  if (nickname == 0 || argc == 1) {
    printf("Formato incorreto! Tente ./client nomeDoNode\n");
    exit(1);
  }

  ClientMonitor<TYPE> monitor(string(nickname), 600);

  tempMonitor = &monitor;

  monitor.AddDataGenerator("temperature", &GetTemperature);
  //monitor.AddDataGenerator("humidity"   , &GetHumidity);
  //monitor.AddDataGenerator("pressure"   , &GetPressure);

  signal(SIGINT, &handler);
  
  while(true) sleep(1);

  return 0;
}
