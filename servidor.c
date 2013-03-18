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

#include "hdr/repa.h"
#include "hdr/linkedlist.h"

pthread_t thread;

bool terminated;

FILE *f;
void saveData(char* addr,char *data){
	f = fopen(addr,"a+");
	fprintf(f,"%s\n",data);
	fclose(f);		
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
			saveData(prefix,data);
		}

		// Free msg
		free(interest);
		free(data);
	}

	pthread_exit(NULL);
	return 0;
}

void sendReqToClients(char *req){
	char *data = req;
	int data_len = strlen(req);
	char *interest = (char*)malloc(255*sizeof(char));
	strcpy(interest,"client");
        if (data_len > 0) {
                if (repa_send(interest, data, data_len, 0) < 0) {
                        printf("CHATERROR: send message (Error No: %d \"%s\").\n", errno, strerror(errno));
                        exit(EXIT_FAILURE);
                }
                printf("Message sent I: \"%s\" D: \"%s\"\n", interest, data);
        }

}

int main(void) {
	prefix_addr_t prefix;
	ssize_t data_len = 0;
	char cprefix[20];
	char *interest, *data;
	char *dummy = NULL;

	if (repa_open() < 0) {
		printf("Error open repa (Error No: %d \"%s\").\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	interest = (char*)malloc(255*sizeof(char));
	data = (char*)malloc(1500*sizeof(char));

	strcpy(interest, "server");
	repa_register_interest(interest); 
		
	pthread_create(&thread, NULL, handle_message, NULL);
	prefix = repa_node_address();
	repa_print_prefix(prefix, cprefix);
	printf("NodePrefix: %d %s\n", prefix, cprefix);

	char *req = malloc(sizeof(char)*11);
	strcpy(req,"temperatura");

	while(true){
		sendReqToClients(req);
		sleep(10);
	}

	terminated = true;
	repa_close();
	pthread_join(thread, NULL);
	exit(EXIT_SUCCESS);
}
