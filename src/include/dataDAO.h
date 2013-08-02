#ifndef DATADAO_H
#define DATADAO_H
#include "mysql/mysql.h"

#define PATH_CONF "src/database/db.conf"
#define LINE_SIZE 255

typedef struct Data{
	char* fromNode;
	int temperature;
	int status;
	char* time;
}Data;

MYSQL* connect;

void getConfData(char*, char*, char*, char*);
int connectDatabase();
void saveData(char*,char*,int);
int loadLastsDatas(Data*,int);
void closeConnection();

#endif
