#include "repaAPI/include/repaAPI.hpp"
#include "monitorAPI/include/client_monitor.hpp"
#include "data.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>

RepaAPI<Data> repaAPI;
char *nickname;

/* Randomize a temperature based on the sin function */
time_t timeNow = 0;
double aux = 0;

Data GetData(string type, double value){
    Data data;
    data.type = type;
    data.value = value;
    time(&timeNow);
    data.time = timeNow;
    data.nickname = string(nickname);
    return data;
}

Data GetTemperature() {
    srand(time(NULL));
    aux += 0.1;
    if (aux > 6.3) aux = 0;
    double temp = (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
    return GetData("temperature",temp);
}

Data GetHumidity(){
    double humidity = 0;
    return GetData("humidity",humidity);
}

Data GetPressure(){
    double pressure = 0;
    return GetData("pressure",pressure);
}

int main(int argc, char **argv) {
    nickname = argv[argc-1];

    if (nickname == 0 || argc == 1) {
        printf("Formato incorreto! Tente ./client nomeDoNode\n");
        exit(1);
    }

    ClientMonitor<Data> monitor("node",5);
    monitor.AddDataGenerator("temperature",&GetTemperature);
    monitor.AddDataGenerator("humidity",&GetHumidity);
    monitor.AddDataGenerator("pressure",&GetPressure);

    while (true) sleep(1);

    return 0;
}
