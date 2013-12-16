#include <stdio.h>
#include "dataDAO.h" 
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

MYSQL* connection;

void getConfData(char* server, char* user, char* pass, char* db){
	FILE *fp = fopen(PATH_CONF,"r");
	if(fp != NULL){
		char *data, *type, *temp;
		data = (char*)malloc(LINE_SIZE*sizeof(char));
		type = (char*)malloc(LINE_SIZE*sizeof(char));
		temp = (char*)malloc(LINE_SIZE*sizeof(char));

		while (fgets(data,LINE_SIZE,fp) != NULL){
			sscanf(data,"%[^=]=\"%[^\"]",type,temp);
			if (strcmp(type,"SERVER") == 0)
				memcpy(server,temp,LINE_SIZE);
			else if (strcmp(type,"USER") == 0)
				memcpy(user,temp,LINE_SIZE);
			else if (strcmp(type,"PASSWORD") == 0)
				memcpy(pass,temp,LINE_SIZE);
			else if (strcmp(type,"DATABASE") == 0)
				memcpy(db,temp,LINE_SIZE);
		}
		fclose(fp);
		free(data);
		free(type);
		free(temp);
	}else{
		printf("Arquivo não encontrado");
		exit(1);
	}
}

int connectDatabase(){
	connection=mysql_init(NULL);

	if(connection == NULL){
		fprintf(stderr,"Inicialização no Mysql falhou.\n");
		return 0;
	}

	char *server = (char*)malloc(LINE_SIZE*sizeof(char));
	char *user = (char*)malloc(LINE_SIZE*sizeof(char));
	char *pass = (char*)malloc(LINE_SIZE*sizeof(char));
	char *db = (char*)malloc(LINE_SIZE*sizeof(char));

	getConfData(server,user,pass,db);
	
	connection = mysql_real_connect(connection,server,user,pass,db,0,NULL,0);

	if (strlen(server) == 0 && strlen(user) == 0 &&
			strlen(pass) == 0 && strlen(db) == 0){
		fprintf(stderr,"Dado vazio.\n");
		connection = NULL;
	}

	free(server);
	free(user);
	free(pass);
	free(db);

	if(connection == NULL){
		fprintf(stderr,"Impossível se conectar.\n");
		return 0;
	}

	return 1;
}


void saveData(char *prefix, char *data,double time2, int status){
	char query[LINE_SIZE];
  char *date;
  
  time_t t;
  time(&t);
  date = ctime(&t);
  sscanf(date,"%[^\n]",date);
	
  int temperature;
  sscanf(data, "%*[^&]&value=%d",&temperature);

  snprintf(query,LINE_SIZE,"INSERT INTO temperatures VALUES ('%s','%s','%d','%d')",
          prefix,date,temperature,status);
	
  if (mysql_query(connection,query)) //return true if get an error.
		printf("%s %lf\n",mysql_error(connection),time2);
	
}

int load(Data** data,char* query){
	MYSQL_RES *res_set;
	MYSQL_ROW row;

	mysql_query(connection,query);

	res_set = mysql_store_result(connection);

	unsigned int numrows = mysql_num_rows(res_set);
	(*data) = (Data*) malloc(sizeof(Data)*numrows);	

	int i=0;
	while ((row = mysql_fetch_row(res_set)) != NULL){
		(*data)[i].temperature = atoi((char*)row[2]);
		(*data)[i].status = atoi((char*)row[3]);
		(*data)[i].fromNode = (char*) row[0];
		(*data)[i].time = atof((char*) row[1]);
		i++;
	}

	return numrows;	
}

int loadLastsDatas(Data** data,int q, char* prefix){
	char* queryWithOutQ = (char *) "select * from temperatures where Prefix like '%s' order by Date desc limit 0,%d;";
	char query[100];
    snprintf(query,100,queryWithOutQ,prefix,q-1);
	return load(data,query);
}

int loadLastsDatasByMinutes(Data** data,int minutes){
  char* queryWithOutQ = (char *) "select * from (select *,str_to_date(Date,'%%a %%b %%e %%H:%%i:%%s %%Y') as Time from temperatures) as t where t.Time > NOW() - INTERVAL %d MINUTE ORDER BY Time DESC;";
	char query[200];
        snprintf(query,200,queryWithOutQ,minutes-1);
	return load(data,query);
}

void closeConnection(){
	mysql_close(connection);
}
