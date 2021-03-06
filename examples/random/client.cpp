#include "monitorAPI/client_monitor.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

/* Randomize a temperature based on the sin function */

#define TYPE double

TYPE aux  = 0;
TYPE aux1 = 0;
TYPE aux2 = 0;

TYPE GetTemperature() {
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  TYPE temp = (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
  return temp;
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

int main(int argc, char **argv) {
  ClientMonitor<TYPE> monitor(&argc, argv);

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);
  monitor.AddDataGenerator("pressure"   , &GetPressure);

  monitor.SendMessageForEachGenerator(true);

  monitor.Run();

  return 0;
}
