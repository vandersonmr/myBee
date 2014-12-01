#ifndef DATADAO_H
#define DATADAO_H
#include <vector>
#include "mysql/mysql.h"
#include "../data.hpp"
#include <string>
#include <sstream>
#define LINE_SIZE 255

template <typename T>
class DataDAO {
  private:
    MYSQL* connection;

  public:
    int connectDatabase(string);
    void saveData(Data<T>, int);
    vector<Data<T>> loadLastsDatasByMinutes(int);
    void getConfData(string,char*, char*, char*, char*);
    vector<Data<T>> load(char*);
    void run_mysql_query(char*);
    vector<Data<T>> loadLastsDatas(int,string);
    vector<Data<T>> loadLastsDatasByType(int,string,string);
    void closeConnection();
    int clearNodesOnline();
    int insertNodeOnline(string);
};

template <typename T>
void DataDAO<T>::getConfData(string conf_path, char* server, char* user, 
    char* pass, char* db){
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
    printf("File not found.\n");
    exit(1);
  }
}

template <typename T>
int DataDAO<T>::connectDatabase(string conf_path) {
  connection = mysql_init(NULL);

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

template <typename T>
void DataDAO<T>::saveData(Data<T> data, int status){
  char query[LINE_SIZE];
  char *date;

  date = ctime(&data.time);
  sscanf(date,"%[^\n]",date);

  stringstream ss;
  ss << data.value;

  snprintf(query,LINE_SIZE,"INSERT INTO data VALUES ('%s','%s','%s','%s','%d','%s')",
      data.nickname.c_str(), date, data.type.c_str(), ss.str().c_str(), status, 
      (char*) data.node.c_str());

  run_mysql_query(query);

}

template <typename T>
void DataDAO<T>::run_mysql_query(char* query){
  if (mysql_query(connection, query)){
    printf("%s\n", mysql_error(connection));
    exit(1);
  }
}

template <typename T>
vector<Data<T>> DataDAO<T>::load(char* query){
  MYSQL_RES *res_set;
  MYSQL_ROW row;

  run_mysql_query(query);

  res_set = mysql_store_result(connection);
  vector<Data<T>> result; 

  while ((row = mysql_fetch_row(res_set)) != NULL){
    Data<T> data;

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

template <typename T>
vector<Data<T>> DataDAO<T>::loadLastsDatasByType(int q, string prefix, string type){
  char* queryWithOutQ = const_cast<char*>("select * from data where Prefix like '%s' and Type like '%s' order by Date desc limit 0,%d;");
  char query[200];
  snprintf(query,200,queryWithOutQ,prefix.c_str(),type.c_str(),q-1);
  return load(query);
}

template <typename T>
int DataDAO<T>::clearNodesOnline() {
  char* query = const_cast<char*>("delete from nodesOnline;");
  return mysql_query(connection, query);
}

template <typename T>
int DataDAO<T>::insertNodeOnline(string prefix) {
  char* queryWithoutPrefix =  const_cast<char*>("insert into nodesOnline values ('%s')");
  char query[200];
  snprintf(query,200, queryWithoutPrefix, prefix.c_str());
  return mysql_query(connection, query);
}

template <typename T>
void DataDAO<T>::closeConnection() {
  mysql_close(connection);
}

#endif
