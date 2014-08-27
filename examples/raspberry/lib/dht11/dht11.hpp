#ifndef _DHT11
#define _DHT11

#include "../dht/dht.hpp"

class DHT11: public DHT{
    public:
        DHT11(int);
        void read_data();

};

#endif
