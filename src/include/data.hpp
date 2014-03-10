#ifndef DATA
#define DATA

#include <string>
#include <msgpack.hpp>
#include <vector>
#include <map>

using namespace std;

class Data{
  public:
    string type;
    string node;
    double value;
    string nickname;
    int status;
    time_t time;
    MSGPACK_DEFINE(type,node,value,nickname,status,time)
};

#endif
