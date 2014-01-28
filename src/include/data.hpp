#include <string>
#include <msgpack.hpp>

using namespace std;

class Data{
  public:
    string type;
    string node;
    string nickname;
    double value;
    int status;
    time_t time;
    MSGPACK_DEFINE(type,node,nickname,value,status,time);
};
