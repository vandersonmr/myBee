#ifndef DATA
#define DATA

#include <string>
#include <msgpack.hpp>
#include <vector>
#include <map>

using namespace std;

enum Type { None = -1, Temperature = 0, Humidity = 1};

union Type_t {
  Type sensor;
  int  typeVal;
};

template <typename T>
class Data {
  public:
    string type;
    Type_t definedType;
    string node;
    T value;
    string nickname;
    int status;
    time_t time;
    MSGPACK_DEFINE(type, definedType.typeVal, node, value, nickname, status, time)
};

#endif
