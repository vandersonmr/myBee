#include "repaAPI/include/repaAPI.hpp"
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

/* Generate the data that will be send to the server */
Data getData() {
  // In this case is generate a randomic temperature
  Data data;
  data.type     = string("temperature");
  data.value    = getTemperature();
  data.time     = timeNow;
  data.nickname = string(nickname);

  return data;
}

void sendMessageToServer(Data data) {
  vector<string> interests;
  interests.push_back("server");

  message<Data> msg;
  msg.data           = data;
  msg.interests      = interests;

  repaAPI.send_message(msg);

  printf("Message sent I: \"%s\" D: \"%f\"\n", interests.back().c_str(), data.value);

}

/* Stay in loop generating and sending data to server */
void generateData() {
  while (true) {
    Data data = getData();
    sendMessageToServer(data);
    sleep(1);
  }
}

int main(int argc, char **argv) {
  nickname = argv[argc-1];

  if (nickname == 0 || argc == 1) {
    printf("Formato incorreto! Tente ./client nomeDoNode\n");
    exit(1);
  }

  vector<string> interests;
  interests.push_back(string("client"));

  repaAPI.init_repa(interests);

  generateData();

  return 0;
}
