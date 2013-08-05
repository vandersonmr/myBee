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

#define LOW_TEMPERATURE 30
#define HIGH_TEMPERATURE 45

pthread_t thread;

bool terminated; 

int checkTemperature(char* data){
	return testData(NULL);
}

void* handle_message(void* param) {
	ssize_t read_len = 0;
	char *interest, *data;
	prefix_addr_t prefix_addr;
	char prefix[20];

	terminated = false;
	while (!terminated) {
		interest = (char*)malloc(255*sizeof(char));
		data = (char*)malloc(1500*sizeof(char));
		read_len = repa_timed_recv(interest, data, prefix_addr, (long int)1E9);
		if (read_len > 0) {
			repa_print_prefix(prefix_addr, prefix);
			int status;
			status = checkTemperature(data);
			saveData(prefix,data,status);
			printf("Message: \"%s\" Status: \"%d\" Prefix: %s\n",data,status,prefix);
		}

		// Free msg
		free(interest);
		free(data);
	}

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

	if (repa_open() < 0) {
		printf("Error open repa (Error No: %d \"%s\").\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	interest = (char*)malloc(255*sizeof(char));

	strcpy(interest, "server");
	repa_register_interest(interest); 
		
	pthread_create(&thread, NULL, handle_message, NULL);
	prefix = repa_node_address();
	repa_print_prefix(prefix, cprefix);
	printf("NodePrefix: %d %s\n", prefix, cprefix);

	while(true);

	terminated = true;
	closeConnection();
	repa_close();
	pthread_join(thread, NULL);
	exit(EXIT_SUCCESS);
}
