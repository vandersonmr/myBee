#include "dht11.hpp"

static timeval_t get_time(){
    timeval_t tv;
    gettimeofday(&tv, NULL);
    return tv;
}

DHT11::DHT11(int pin){
    this->pin = pin;
}


void DHT11::init(){
    this->thr = new std::thread(&DHT11::read_data_non_stop, this);
}

bool DHT11::has_timeout(timeval_t& init, timeval_t& end, uint32_t max_time){
    uint32_t diff_secs = (end.tv_sec - init.tv_sec) * 1000000;
    if ((diff_secs + end.tv_usec) - init.tv_usec > max_time) return true;
    return false;
}

void DHT11::waitForSignal(int signal, uint32_t max_time){
    timeval_t init, end;
    init = get_time();
    while (digitalRead(this->pin) != signal){
        end = get_time();
        if (has_timeout(init, end, max_time)) return;
    }
}

void DHT11::close(){
    this->running = false;
    this->thr->join();
    delete this->thr;
}

void DHT11::handshake(){
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, LOW);
    delay(18);
    digitalWrite(this->pin, HIGH);
    delayMicroseconds(40);
    pinMode(this->pin, INPUT);
    waitForSignal(HIGH, 100);
    waitForSignal(LOW, 100);
}

void DHT11::read_data_non_stop(){
    while (running){
        read_data();
        sleep(INTERVAL_TIME);
    }
}

void DHT11::read_data(){ 
    handshake();
    uint8_t values[5];
    for (int i = 0; i < 5; i++){
        values[i] = 0;
        for (int j = 0; j < 8; j++){
            waitForSignal(HIGH, 60);
            timeval_t init = get_time();
            waitForSignal(LOW, 100);
            timeval_t end = get_time();
            if (has_timeout(init, end, 40))
                values[i] |= 1 << (7 - j);
        }
    }
    if (values[0] + values[2] != values[4]){
        this->error = 1;
    } else {
        this->temperature = values[2];
        this->humidity    = values[0];
        this->error = 0;
    }
}

int DHT11::get_temperature(){
    return this->temperature;
}

int DHT11::get_humidity(){
    return this->humidity;
}

bool DHT11::has_errors(){
    if (this->error != 0)
        return true;
    return false;
}
