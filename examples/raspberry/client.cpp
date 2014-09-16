#include "monitorAPI/include/client_monitor.hpp"
#include "lib/bcm2835-dht/BcmDHT.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <bcm2835.h>

BcmDHT dht(22, 18, 7);

double GetTemperature(){
    return dht.getTemperature();
}

double GetHumidity(){
    return dht.getHumidity();
}

int main(int argc, char **argv) {
  dht.init();

  ClientMonitor monitor(&argc, argv);

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);

  monitor.Run();

  return 0;
}
