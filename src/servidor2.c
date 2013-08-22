#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h> 
#include <errno.h>
#include <pthread.h>
#include <mysql.h>

#include "machineLearning.h"
#include "dataDAO.h"
#include "hdr/repa.h"
#include "hdr/linkedlist.h"

pthread_t thread;

bool terminated;

repa_sock_t sock;

int checkTemperature(char* prefix, char* data,char* time){
	Data* data1 = (Data*) malloc(sizeof(Data));
	data1->time = time;
	data1->fromNode = prefix;
	data1->temperature = atoi(data);
	return testData(data1);
}

char* getTime(){
	char *date;
	time_t t;
	time(&t);
	date = ctime(&t);
	sscanf(date,"%[^\n]",date);
	return date;
}

void* handle_message(void* param) {
	ssize_t read_len = 0;
	char *interest, *data;
	prefix_addr_t prefix_addr;
	char* prefix = (char*)malloc(sizeof(char)*255);

	terminated = false;
	while (!terminated) {
		interest = (char*)malloc(255*sizeof(char));
		data = (char*)malloc(1500*sizeof(char));
		read_len = repa_timed_recv(sock,interest, data, prefix_addr, (long int)1E9);
		if (read_len > 0) {
			char* time = getTime();
			repa_print_prefix(prefix_addr, prefix);
			int status;
			status = checkTemperature(prefix,data,time);
			saveData(prefix,data,time,status);
			printf("Message: \"%s\" Status: \"%d\" Prefix: %s\n",data,status,prefix);
		}

		// Free msg
		free(interest);
		free(data);
	}

	free(prefix);

	pthread_exit(NULL);
	return 0;
}

int main(void) {
	prefix_addr_t prefix;
	ssize_t data_len = 0;
	char cprefix[20];
	char *interest, *data;
	char *dummy = NULL;

	if (!connectDatabase()){
		return EXIT_FAILURE;
	}

	sock = repa_open();

	interest = (char*)malloc(255*sizeof(char));

	strcpy(interest, "server");
	repa_register_interest(sock,interest); 

	pthread_create(&thread, NULL, handle_message, NULL);
	prefix = repa_get_node_address();
	repa_print_prefix(prefix, cprefix);
	printf("NodePrefix: %d %s\n", prefix, cprefix);

	while(true) sleep(1);

	terminated = true;
	closeConnection();
	repa_close(sock);
	pthread_join(thread, NULL);
	exit(EXIT_SUCCESS);
}
