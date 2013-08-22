#include "hdr/repa.h"
#include "hdr/linkedlist.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

repa_sock_t sock;

int last = 35;
double i = 0;
void getTemperature(char* data){
	srand(time(NULL));
	i += 0.1;
	last = (sin(i)*10+26) + rand() % 5 - 1;
	sprintf(data,"%d", last);
	strcat(data," graus celcius.");
}

void sendMessage(char* data){
	char* interest = (char*)malloc(255*sizeof(char));
	strcpy(interest,"server");
	size_t data_len = strlen(data);
	if (data_len > 0) {
		repa_send(sock, interest, data, data_len, 0);
		printf("Message sent I: \"%s\" D: \"%s\"\n", interest, data);
	}
	free(interest);
}

int main(){
	char *interest,*data;

	sock = repa_open();

	//Aloca espaço para registrar endereço e dados para transmissão
	interest = (char*)malloc(255*sizeof(char));
	data = (char*)malloc(1500*sizeof(char));

	strcpy(interest,"client");
	repa_register_interest(sock,interest); // Registra o interesse

	while (true){
		getTemperature(data);
		sendMessage(data);
		sleep(5);
	}
	free(interest);
	free(data);
	repa_close(sock);
	return 0;
}
