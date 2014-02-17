#include "repaAPI/include/repa_api.hpp"
#include "monitorAPI/include/client_monitor.hpp"
#include "data.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

/* Randomize a temperature based on the sin function */
double aux = 0;

double GetTemperature() {
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  double temp = (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
  return temp;
}

double GetHumidity(){
  double humidity = 0;
  return humidity;
}

double GetPressure(){
  double pressure = 0;
  return pressure;
}

int main(int argc, char **argv) {
  char* nickname = argv[argc-1];

  if (nickname == 0 || argc == 1) {
    printf("Formato incorreto! Tente ./client nomeDoNode\n");
    exit(1);
  }

  ClientMonitor monitor(string(nickname), 5);

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);
  monitor.AddDataGenerator("pressure"   , &GetPressure);

  while (true) sleep(1);

  return 0;
}
