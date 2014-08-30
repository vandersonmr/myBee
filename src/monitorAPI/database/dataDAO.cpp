#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "dataDAO.hpp" 
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

MYSQL* connection;

void getConfData(string conf_path, char* server, char* user, char* pass, char* db){
  FILE *fp = fopen(conf_path.c_str(), "r");
  if(fp != NULL){
    char *data, *type, *temp;
    data = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));
    type = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));
    temp = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));

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

int connectDatabase(string conf_path) {
  connection=mysql_init(NULL);

  if(connection == NULL){
    fprintf(stderr,"Inicialização no Mysql falhou.\n");
    return 0;
  }

  char *server = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));
  char *user   = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));
  char *pass   = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));
  char *db     = static_cast<char*>(malloc(LINE_SIZE*sizeof(char)));

  getConfData(conf_path,server,user,pass,db);

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



void saveData(Data data, int status){
  char query[LINE_SIZE];
  char *date;

  date = ctime(&data.time);
  sscanf(date,"%[^\n]",date);

  snprintf(query,LINE_SIZE,"INSERT INTO data VALUES ('%s','%s','%s','%.2f','%d','%s')",
      data.nickname.c_str(), date, data.type.c_str(), data.value, status, 
      data.node.c_str());

  run_mysql_query(query);

}

void run_mysql_query(char* query){
    if (mysql_query(connection, query)){
        printf("%s\n", mysql_error(connection));
        exit(1);
    }
}

vector<Data> load(char* query){
  MYSQL_RES *res_set;
  MYSQL_ROW row;


  run_mysql_query(query);

  res_set = mysql_store_result(connection);
  vector<Data> result; 

  while ((row = mysql_fetch_row(res_set)) != NULL){
    Data data;

    data.type     = const_cast<char*>(row[2]);
    data.value    = atof(const_cast<char*>(row[3]));
    data.status   = atoi(const_cast<char*>(row[4]));
    data.nickname = const_cast<char*>(row[0]);
    data.time     = atof( const_cast<char*>(row[1]));
    data.node     = const_cast<char*>(row[5]);

    result.push_back(data);
  }

  return result;	
}

vector<Data> loadLastsDatasByType(int q, string prefix, string type){
  char* queryWithOutQ = const_cast<char*>("select * from data where Prefix like '%s' and Type like '%s' order by Date desc limit 0,%d;");
  char query[200];
  snprintf(query,200,queryWithOutQ,prefix.c_str(),type.c_str(),q-1);
  return load(query);
}

int clearNodesOnline() {
  char* query = const_cast<char*>("delete from nodesOnline;");
  return mysql_query(connection, query);
}

int insertNodeOnline(string prefix) {
  char* queryWithoutPrefix =  const_cast<char*>("insert into nodesOnline values ('%s')");
  char query[200];
  snprintf(query,200, queryWithoutPrefix, prefix.c_str());
  return mysql_query(connection, query);
}

void closeConnection() {
  mysql_close(connection);
}
