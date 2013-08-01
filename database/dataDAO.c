#include <stdio.h>
#include "dataDAO.h" 
#include <time.h>

int connectDatabase(char* user, char* pass,char* db, char* server){
	connect=mysql_init(NULL);

	if(!connect){
		fprintf(stderr,"Inicialização no Mysql falhou.");
		return 1;
	}

	connect=mysql_real_connect(connect,server,user,pass,db,0,NULL,0);

	if(!connect){
		fprintf(stderr,"Impossível se conectar.");
		return 1;
	}

	return 0;
}


void saveData(char *prefix, char *data, int status){
	char query[255];
	char *date;
	time_t t;
	time(&t);
	date = ctime(&t);
	sscanf(date,"%[^\n]",date);
	snprintf(query,255,"INSERT INTO temperatures VALUES ('%s','%s','%s','%d')",prefix,
			date,data,status);
	if (mysql_query(connect,query)){ //return true if get an error.
		printf("%s\n",mysql_error(connect));
	}
}


int loadLastsDatas(Data* data,int q){
	MYSQL_RES *res_set;
	MYSQL_ROW row;

	char* queryWithOutQ = "select * from temperatures order by Date desc limit 0,%d;";
	char query[60];
	snprintf(query,60,queryWithOutQ,q-1);
	mysql_query(connect,query);

	res_set = mysql_store_result(connect);

	unsigned int numrows = mysql_num_rows(res_set);

	int i=0;
	while ((row = mysql_fetch_row(res_set)) != NULL){
		data[i].temperature = atoi((char*)row[2]);
		data[i].status = atoi((char*)row[3]);
		data[i].fromNode = (char*) row[0];
		data[i].time = (char*) row[1];
		i++;
	}

	return numrows;
}

void closeConnection(){
	mysql_close(connect);
}
