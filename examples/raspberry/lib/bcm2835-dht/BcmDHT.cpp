#include "BcmDHT.hpp"

// How to access GPIO registers from C-code on the Raspberry-Pi
// Example program
// 15-January-2012
// Dom and Gert

BcmDHT::BcmDHT(int type, int pin) {
    this->type = type;
    this->pin  = pin;
}

int BcmDHT::readDHT() {
    int counter = 0;
    int laststate = HIGH;
    int j=0;
    int data[5];
    // Set GPIO pin to output
    bcm2835_gpio_fsel(this->pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(this->pin, HIGH);
    usleep(500000); // 500 ms
    bcm2835_gpio_write(this->pin, LOW);
    usleep(20000);
    bcm2835_gpio_fsel(this->pin, BCM2835_GPIO_FSEL_INPT);
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;
    // wait for pin to drop?
    while (bcm2835_gpio_lev(this->pin) == 1) {
        usleep(1);
    }
    // read data!
    for (int i=0; i< MAXTIMINGS; i++) {
        counter = 0;
        while ( bcm2835_gpio_lev(this->pin) == laststate) {
            counter++;
            //nanosleep(1); // overclocking might change this?
            if (counter == 1000)
                break;
        }
        laststate = bcm2835_gpio_lev(this->pin);
        if (counter == 1000) break;
        if ((i>3) && (i%2 == 0)) {
            // shove each bit into the storage bytes
            data[j/8] <<= 1;
            if (counter > 200)
                data[j/8] |= 1;
            j++;
        }
    }
    if ((j >= 39) &&
            (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
        // yay!
        if (this->type == DHT11) {
            this->temperature = data[2];
            this->humidity    = data[0];
        }
        if (this->type == DHT22) {
            float f, h;
            h = data[0] * 256 + data[1];
            h /= 10;
            f = (data[2] & 0x7F)* 256 + data[3];
            f /= 10.0;
            if (data[2] & 0x80) f *= -1;
            this->temperature = f;
            this->humidity    = h;
        }
        return 1;
    }
    return 0;
}

void BcmDHT::init() {
    bcm2835_init();
    this->thr = new std::thread(&BcmDHT::readData, this);
}

void BcmDHT::close() {
    this->running = false;
    this->thr->join();
    delete this->thr;
}

void BcmDHT::readData() {
    while (this->running) {
        while (!readDHT());
        sleep(INTERVAL_TIME);
    }
}

double BcmDHT::getTemperature(){
    return this->temperature;
}

double BcmDHT::getHumidity(){
    return this->humidity;
}
