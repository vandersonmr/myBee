#ifndef DATADAO_H
#define DATADAO_H
#include <vector>
#include "mysql/mysql.h"
#include "data.hpp"
#define PATH_CONF "config/db.conf"
#define LINE_SIZE 255

vector<Data> loadLastsDatasByMinutes(int);
void getConfData(char*, char*, char*, char*);
extern int connectDatabase();
extern void saveData(Data,int);
vector<Data> loadLastsDatas(int,string);
vector<Data> loadLastsDatasByType(int,string,string);
void closeConnection();
int clearNodesOnline();
int insertNodeOnline(string);

#endif
