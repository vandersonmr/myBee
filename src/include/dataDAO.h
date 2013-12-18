#ifndef DATADAO_H
#define DATADAO_H
#include "mysql/mysql.h"
#define PATH_CONF "config/db.conf"
#define LINE_SIZE 255

typedef struct Data{
	char* node;
  char* nickname;
	int temperature;
	int status;
	double time;
}Data;

int loadLastsDatasByMinutes(Data**,int);
void getConfData(char*, char*, char*, char*);
extern int connectDatabase();
extern void saveData(Data*,int);
int loadLastsDatas(Data**,int,char*);
void closeConnection();

#endif
