#include "monitorAPI/include/client_monitor.hpp"
#include "lib/dht22/dht22.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <bcm2835.h>

ClientMonitor *tempMonitor;
DHT22 dht(1);

double GetTemperature(){
    return dht.get_temperature();
}

double GetHumidity(){
    return dht.get_humidity();
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

  dht.init();

  ClientMonitor monitor(string(nickname), 60);

  tempMonitor = &monitor;

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);

  signal(SIGINT, &handler);
  
  while(true){
    sleep(1);
  }

  return 0;
}
