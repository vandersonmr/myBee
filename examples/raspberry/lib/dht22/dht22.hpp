#ifndef _DHT22
#define _DHT22

#include "../dht/dht.hpp"

#define MAXTIMINGS 100

class DHT22: public DHT{
    public:
        DHT22(int);
        void read_data();

};

#endif
