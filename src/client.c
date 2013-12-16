#include "hdr/repa.h"
#include "hdr/linkedlist.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

repa_sock_t sock;

/* Randomize a temperature based on the sin function */
double timeNow = 0;
int getTemperature() {
	srand(time(NULL));
	timeNow += 0.1; // Represent the time/clock
  
  if (timeNow > 6.3) // 6.3 ~= 2pi
    timeNow = 0;

	return (sin(timeNow)*10+26) + rand() % 5 - 1; // Rand add some noise
}

/* Generate the data that will be send to the server */
void getData(char* data) {
  // In this case is generate a randomic temperature
  int temperature = getTemperature();

	sprintf(data,"temperature?time=%f&value=%d",
          timeNow, temperature); 
          // TimeNow is the same used to generate the temperature
}

void sendMessageToServer(char* data) {
	char* interest = (char*)malloc(255*sizeof(char));
	strcpy(interest,"server"); 

	size_t data_len = strlen(data);

	if (data_len > 0) {
		repa_send(sock, interest, data, data_len, 0);
		printf("Message sent I: \"%s\" D: \"%s\"\n", interest, data);
	}

	free(interest);
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
int main() {
	char *interest;

	sock = repa_open();

	//Aloca espaço para registrar endereço e dados para transmissão
	interest = (char*)malloc(255*sizeof(char));

	strcpy(interest,"client");
	repa_register_interest(sock,interest); // Registra o interesse

  generateData();

	free(interest);
	//repa_close(sock);
	return 0;
}
