#ifndef _DHT11
#define _DHT11

#include <wiringPi.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

typedef struct timeval timeval_t;

#define INTERVAL_TIME 1

class DHT11{
    private:
        int pin, error, temperature, humidity;
        void waitForSignal(int, uint32_t);
        void handshake();
        bool has_timeout(timeval_t&, timeval_t&, uint32_t);
        std::thread* thr;
        void read_data_non_stop();
        bool running = true;

    public:
        DHT11(int);
        void init();
        void close();
        bool has_errors();
        void read_data();
        int get_humidity();
        int get_temperature();
};

#endif
