#ifndef MESSAGE
#define MESSAGE

#include <msgpack.hpp>
#include <vector>
#include <string>

using namespace std;

template<class T> class message {
  public:
    vector<string> interests;
    vector<T> data;
    string prefix_address;
    uint32_t id;
    time_t time;
    MSGPACK_DEFINE(interests, data, prefix_address, id, time)
};

#endif
