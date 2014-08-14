#ifndef _DHT11
#define _DHT11

#include <wiringPi.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

typedef struct timeval timeval_t;

class DHT11{
    private:
        int pin, error, temperature, humidity;
        void waitForSignal(int, uint32_t);
        void handshake();
        bool has_timeout(timeval_t&, timeval_t&, uint32_t);

    public:
        DHT11(int);
        bool has_errors();
        void read_data();
        int get_humidity();
        int get_temperature();
};

#endif
