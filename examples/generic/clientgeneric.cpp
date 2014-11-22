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

TYPE GetGenericData() {
  stringstream ss;
  ss << "generic-data-" << size;
  Generic data(ss.str(), size);
  size++;
  return data;
}

int main(int argc, char **argv) {
  ClientMonitor<TYPE> monitor(&argc, argv);

  monitor.AddDataGenerator("generic data", &GetGenericData);

  monitor.Run();

  return 0;
}
