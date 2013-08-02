#include <stdio.h>
#include "dataDAO.h" 
#include <time.h>

void getConfData(char* server, char* user, char* pass, char* db){
	FILE *fp = fopen(PATH_CONF,"r");
	if( fp != -1){
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
	connect=mysql_init(NULL);

	if(connect == NULL){
		fprintf(stderr,"Inicialização no Mysql falhou.\n");
		return 0;
	}

	char *server = (char*)malloc(LINE_SIZE*sizeof(char));
	char *user = (char*)malloc(LINE_SIZE*sizeof(char));
	char *pass = (char*)malloc(LINE_SIZE*sizeof(char));
	char *db = (char*)malloc(LINE_SIZE*sizeof(char));

	getConfData(server,user,pass,db);
	
	connect = mysql_real_connect(connect,server,user,pass,db,0,NULL,0);

	if (strlen(server) == 0 && strlen(user) == 0 &&
			strlen(pass) == 0 && strlen(db) == 0){
		fprintf(stderr,"Dado vazio.\n");
		connect = NULL;
	}

	free(server);
	free(user);
	free(pass);
	free(db);

	if(connect == NULL){
		fprintf(stderr,"Impossível se conectar.\n");
		return 0;
	}

	return 1;
}


void saveData(char *prefix, char *data, int status){
	char query[LINE_SIZE];
	char *date;
	time_t t;
	time(&t);
	date = ctime(&t);
	sscanf(date,"%[^\n]",date);
	snprintf(query,LINE_SIZE,"INSERT INTO temperatures VALUES ('%s','%s','%s','%d')",prefix,
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
