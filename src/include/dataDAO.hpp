#ifndef DATADAO_H
#define DATADAO_H
#include <vector>
#include "mysql/mysql.h"
#include "data.hpp"
#include <string>
#define LINE_SIZE 255

vector<Data> loadLastsDatasByMinutes(int);
void getConfData(string,char*, char*, char*, char*);
extern int connectDatabase(string);
extern void saveData(Data,int);
vector<Data> loadLastsDatas(int,string);
vector<Data> loadLastsDatasByType(int,string,string);
void closeConnection();
int clearNodesOnline();
int insertNodeOnline(string);

#endif
