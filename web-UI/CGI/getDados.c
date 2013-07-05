#include <stdio.h>
#include "dataDAO.h"
#include <stdlib.h>
#define SERVER "localhost"
#define USER "root"
#define PASSWORD "123456"
#define DATABASE "monitorAbelhas"

char* getStatus(Data* temperatures,int numrows){
    char* status = malloc(100*sizeof(char));
    status = "OK";
    return status;
}

void printTemperatures(Data* temperature,int numrows){
    int i;
    for(i=0; i < numrows; i++){
        printf("%s & %s & %d<br>\n",
	    	temperature[i].fromNode,
            temperature[i].time,
            temperature[i].temperature); 
    }
 
}

int main()
{
    printf("Content-Type: text/html;charset=us-ascii\n\n");
 
    int connected = connectDatabase(USER,PASSWORD,DATABASE,SERVER);

    Data* data = malloc(sizeof(Data)*75);
    int numrows;
    if(connected == 0){
	numrows = loadLastsDatas(data,75);
    }

    printf("%s <br>\n",getStatus(data,numrows));
    
    printTemperatures(data,numrows);
    
    closeConnection();  
    return 0;
}
