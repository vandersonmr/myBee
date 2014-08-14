#include "monitorAPI/include/client_monitor.hpp"
#include "lib/dht11.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

ClientMonitor *tempMonitor;
DHT11 dht(1);

int GetTemperature() {
    dht.read_data();
    while (dht.has_errors()){
        dht.read_data();
    }
    return dht.get_temperature();
}

int GetHumidity(){
    return dht.get_humidity();
}

void handler(int sig){
    cout << "Signal " << sig << endl;
    tempMonitor->Close();
}

int main(int argc, char **argv) {
  char* nickname = argv[argc-1];

    wiringPiSetup();

  if (nickname == 0 || argc == 1) {
    printf("Formato incorreto! Tente ./client nomeDoNode\n");
    exit(1);
  }

  ClientMonitor monitor(string(nickname), 4);

  tempMonitor = &monitor;

  monitor.AddDataGenerator("temperature", &GetTemperature);
  monitor.AddDataGenerator("humidity"   , &GetHumidity);

  signal(SIGINT, &handler);
  
  while(true) sleep(1);

  return 0;
}
