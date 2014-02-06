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
int getTemperature() {
    srand(time(NULL));
    time(&timeNow); // Represent the time/clock
    aux += 0.1;
    if (aux > 6.3) aux = 0;

    return (sin(aux)*10+26) + rand() % 2 - 1; // Rand add some noise
}

int getHumidity(){
    return 0;
}

int getPressure(){
    return 0;
}

/* Generate the data that will be send to the server */
Data getData() {
    // In this case is generate a randomic temperature
    map<string,double> values;

    values["temperature"] = getTemperature();
    values["humidity"] = getHumidity();
    values["pressure"] = getPressure();
    
    Data data;
    data.type_value = values;
    data.time     = timeNow;
    data.nickname = string(nickname);

    printf("%.3f %.3f %.3f\n", values["temperature"], values["humidity"], 
            values["pressure"]);

    return data;
}

int main(int argc, char **argv) {
    nickname = argv[argc-1];

    if (nickname == 0 || argc == 1) {
        printf("Formato incorreto! Tente ./client nomeDoNode\n");
        exit(1);
    }

    ClientMonitor<Data> monitor(string(nickname),5);
    monitor.AddDataGenerator(string("hereIsTheFuck"),getData);

    while (true) sleep(1);

    return 0;
}
