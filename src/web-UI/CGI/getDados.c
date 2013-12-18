#include <stdio.h>
#include "dataDAO.h"
#include <stdlib.h>

void printTemperatures(Data* temperature,int numrows) {
  int i;
	for(i=0; i < numrows; i++) {
        printf("%s & %lf & %d & %d<br>\n",
	    	      temperature[i].nickname,
              temperature[i].time,
              temperature[i].temperature,
			        temperature[i].status); 
    }
 
}

int main()
{
    printf("Content-Type: text/html;charset=us-ascii\n\n");
 
    if (!connectDatabase())
		  return EXIT_FAILURE;

    Data* data;
    int numrows;
    
    numrows = loadLastsDatasByMinutes(&data,2);

    printTemperatures(data,numrows);
    
    closeConnection();  
    return 0;
}
