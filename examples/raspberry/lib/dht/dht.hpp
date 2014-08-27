#ifndef _DHT
#define _DHT

#include <wiringPi.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>

typedef struct timeval timeval_t;

#define INTERVAL_TIME 1

class DHT{
    protected:
        int pin, error;
        double temperature, humidity;
        void waitForSignal(int, uint32_t);
        void handshake();
        bool has_timeout(timeval_t&, timeval_t&, uint32_t);
        std::thread* thr;
        void read_data_non_stop();
        bool running = true;

    public:
        DHT(int);
        void init();
        void close();
        bool has_errors();
        virtual void read_data();
        double get_humidity();
        double get_temperature();
};

#endif
