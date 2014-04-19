#include "monitorAPI/include/client_monitor.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

ClientMonitor *tempMonitor;
/* Randomize a temperature based on the sin function */
double aux  = 0;
double aux1 = 0;
double aux2 = 0;

double GetTemperature() {
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  double temp = (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
  return temp;
}

double GetHumidity(){
  srand(time(NULL));
  aux1 += 0.1;
  if (aux1 > 6.3) aux1 = 0;
  double humidity = (sin(aux1)*8+26) + rand() % 2 - 1; // Rand add some noise
  return humidity;
}

double GetPressure(){
  srand(time(NULL));
  aux2 += 0.1;
  if (aux2 > 6.3) aux2 = 0;
  double pressure = (sin(aux2)*12+26) + rand() % 2 - 1; // Rand add some noise
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

  ClientMonitor monitor(string(nickname), 4);

  tempMonitor = &monitor;

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);
  monitor.AddDataGenerator("pressure"   , &GetPressure);

  signal(SIGINT, &handler);
  
  while(true) sleep(1);

  return 0;
}
