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

double getTime(char* data){
  double date;
  sscanf(data,"%*[^?]?time=%lf&%*[^\n]",&date);
  return date;
}

Data* parseData(char* data, char* prefix){
  Data* data1 = (Data*) malloc(sizeof(Data));
  data1->time = getTime(data);
  data1->nickname = (char*) malloc(strlen(data)*sizeof(char));
  data1->node = prefix;
  sscanf(data,"%*[^&]&value=%*d&nickname=%s",data1->nickname);
  sscanf(data,"%*[^&]&value=%d&%*s",&data1->temperature);
  return data1;
}

int checkTemperature(Data* data){
  return testData(data);
}

void updateListOfNodesOnline() {
  struct dllist *list = NULL;
  struct dll_node *lnode = NULL;
  char* prefix = (char*)malloc(sizeof(char)*255);

  dll_create(list);
  
  repa_get_nodes_in_network(sock, list);

  clearNodesOnline();
  
  repa_print_prefix(repa_get_node_address(),prefix);
  insertNodeOnline(prefix);

  for(lnode = list->head; lnode != NULL; lnode = lnode->next) {
    repa_print_prefix((prefix_addr_t) lnode->data, prefix);
    insertNodeOnline(prefix);
  }
  
  dll_destroy(list);
}

void* handle_message(void*) {
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

      repa_print_prefix(prefix_addr, prefix);
      Data* dataStruct = parseData(data, prefix);
      
      int status;
      status = checkTemperature(dataStruct);

      saveData(dataStruct,status);
      printf("Message: \"%s\" Status: \"%d\" Prefix: %s\n",data,status,dataStruct->nickname);
      
      updateListOfNodesOnline();
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
  char cprefix[20];
  char *interest;

  if (!connectDatabase()){
    return EXIT_FAILURE;
  }

  sock = repa_open();

  interest = (char*)malloc(255*sizeof(char));

  strcpy(interest, "server");
  repa_register_interest(sock,interest);

  pthread_create(&thread, NULL, handle_message, NULL);
  prefix = repa_get_node_address();
  printf("NodePrefix: %d %s\n", prefix, cprefix);

  while(true) sleep(1);

  terminated = true;
  closeConnection();
  repa_close(sock);
  pthread_join(thread, NULL);
  exit(EXIT_SUCCESS);
}
