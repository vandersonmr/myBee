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

//void getParameters(char* request, char** name) {
  //data = request("");
//}

int main()
{
    printf("Content-Type: text/html;charset=us-ascii\n\n");
 
    if (!connectDatabase())
		  return EXIT_FAILURE;

    //char* name = (char*) malloc(sizeof(char)*255);

    char* request = getenv("QUERY_STRING");
    if(request == NULL) {
    } else {
      printf("%s\n",request); 
      //getParameters(request, name);

      //Data* data = getAllDataFrom(name);
    
      //printTemperatures(data);
    }

    closeConnection();  
    return 0;
}
