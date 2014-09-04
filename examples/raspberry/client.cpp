#include "monitorAPI/include/client_monitor.hpp"
#include "lib/dht22/dht22.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <bcm2835.h>

DHT22 dht(1);

double GetTemperature(){
    return dht.get_temperature();
}

double GetHumidity(){
    return dht.get_humidity();
}

int main(int argc, char **argv) {
  dht.init();

  ClientMonitor monitor(&argc, argv);

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);

  monitor.Run();

  return 0;
}
