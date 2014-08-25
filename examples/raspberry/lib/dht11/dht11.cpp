#include "dht11.hpp"

DHT11::DHT11(int pin) : DHT(pin){}

void DHT11::read_data(){
    handshake();
    waitForSignal(HIGH, 100);
    waitForSignal(LOW, 100);
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
        this->humidity = values[0];
        this->error = 0;
    }
}
