#ifndef DATADAO_H
#define DATADAO_H
#include "mysql/mysql.h"
#define PATH_CONF "config/db.conf"
#define LINE_SIZE 255

typedef struct Data{
	char* fromNode;
	int temperature;
	int status;
	char* time;
}Data;

void getConfData(char*, char*, char*, char*);
extern int connectDatabase();
extern void saveData(char*,char*,char*,int);
int loadLastsDatas(Data*,int);
void closeConnection();

#endif
