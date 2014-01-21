#include "repaAPI/include/repaAPI.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>


RepaAPI<string> repaAPI;
char *nickname;

/* Randomize a temperature based on the sin function */
double timeNow = 0;
int getTemperature() {
	srand(time(NULL));
	timeNow += 0.1; // Represent the time/clock
  
  if (timeNow > 6.3) // 6.3 ~= 2pi
    timeNow = 0;

	return (sin(timeNow)*10+26) + rand() % 2 - 1; // Rand add some noise
}

/* Generate the data that will be send to the server */
void getData(char* data) {
  // In this case is generate a randomic temperature
  int temperature = getTemperature();

	sprintf(data,"temperature?time=%f&value=%d&nickname=%s",
          timeNow, temperature, nickname); 
          // TimeNow is the same used to generate the temperature
}

void sendMessageToServer(char* data) {
	size_t data_len = strlen(data);

	if (data_len > 0) {
    vector<string> interests;
    interests.push_back("server");
  
    message<string> msg;
    msg.data = string(data);
    msg.interests = interests;

    repaAPI.send_message(msg);

		printf("Message sent I: \"%s\" D: \"%s\"\n", interests.back().c_str(), data);
	}

}

/* Stay in loop generating and sending data to server */
void generateData() {
  char* data = (char*)malloc(1500*sizeof(char));
	while (true) {
		getData(data);
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

	//repa_close(sock);
	return 0;
}
