#ifndef BCM_DHT
#define BCM_DHT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <bcm2835.h>
#include <unistd.h>
#include <thread>

#define DHT11 11
#define DHT22 22
#define MAXTIMINGS 100
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define INTERVAL_TIME 10

class BcmDHT{
    private:
        double temperature, humidity;
        int type, pin, pin_v;
        std::thread *thr;
        bool running = true;
        int readDHT();
        void readData();

    public:
        BcmDHT(int, int, int);
        void init();
        void close();
        double getTemperature();
        double getHumidity();
};

#endif
