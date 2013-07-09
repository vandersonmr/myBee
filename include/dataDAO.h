#ifndef DATADAO_H
#define DATADAO_H
#include "mysql/mysql.h"

typedef struct Data{
	char* fromNode;
	int temperature;
	char* time;
}Data;

MYSQL* connect;

int connectDatabase(char*,char*,char*,char*);
void saveData(char*,char*);
int loadLastsDatas(Data*,int);
void closeConnection();

#endif
