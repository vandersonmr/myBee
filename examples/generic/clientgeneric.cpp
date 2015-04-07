#include "monitorAPI/client_monitor.hpp"
#include "generic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#define TYPE Generic

uint16_t size = 0;

double aux = 0;

TYPE GetTemperature() {
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  double temp = (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
  TYPE t("temperature");
  t.setTemperature(temp);
  t.setType(0);
  return t;
}

TYPE GetHumidity(){
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  double humidity = (sin(aux)*8+26) + rand() % 2 - 1; // Rand add some noise
  TYPE t("humidity");
  t.setHumidity(humidity);
  t.setType(1);
  return t;
}

TYPE GetPressure(){
  srand(time(NULL));
  aux += 0.1;
  if (aux > 6.3) aux = 0;
  double pressure = (sin(aux)*12+26) + rand() % 2 - 1; // Rand add some noise
  TYPE t("pressure");
  t.setPressure(pressure);
  t.setType(2);
  return t;
}

TYPE GetGenericData() {
  while (true) {
    sleep(5);
    //if detect movement, report everybody!!!
    stringstream ss;
    ss << "generic-data-" << size;
    Generic data(ss.str());
    data.setType(-1);
    size++;
    return data;
  }
}

void RunCommand(TYPE data) {
  cout << data.getCommand() << endl;
  if (data.getCommand() == "toggle-tv1") {
    cout << "Toggling TV power button" << endl;
    system("./commands.sh 'toggle'");
  }
}

TYPE GetTVPower() {
  TYPE gen("tv power");
  gen.setType(4);
  FILE* fp = popen("./commands.sh 'status'", "r");
  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), fp) != NULL);
  sscanf(buffer, "%*[^:]: %s", buffer);
  gen.setStatus(string(buffer));
  pclose(fp);
  return gen;
}

int main(int argc, char **argv) {
  ClientMonitor<TYPE> monitor(&argc, argv);

  monitor.SendMessageForEachGenerator(true);

  //monitor.AddDataGenerator("generic data", 0, &GetGenericData);
  monitor.AddDataGenerator("temperature", 3, &GetTemperature);
  monitor.AddDataGenerator("humidity", 15, &GetHumidity);
  monitor.AddDataGenerator("pressure", 20, &GetPressure);

  monitor.AddDataGenerator("tv power", &GetTVPower);

  monitor.HandleServerMessages(&RunCommand);

  monitor.Run();

  return 0;
}
