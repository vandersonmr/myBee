#include <string>
#include <msgpack.hpp>
#include <vector>
#include <map>

using namespace std;

class Data{
  public:
    map<string,double> type_value;
    string node;
    string nickname;
    int status;
    time_t time;
    MSGPACK_DEFINE(type_value,node,nickname,status,time);
};
