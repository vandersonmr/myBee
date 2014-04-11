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
    MSGPACK_DEFINE(interests, data, prefix_address)
};

#endif
