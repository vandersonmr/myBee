#include "dht22.hpp"
#include <stdlib.h>

DHT22::DHT22(int pin) : DHT(pin){}

void DHT22::read_data(){
    handshake();
    
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0;
    uint8_t values[5];

    for (int i = 0; i < MAXTIMINGS; i++) {
        counter = 0;
        while (digitalRead(this->pin) == laststate) {
            counter++;
            delayMicroseconds(1);
            if (counter == 255) {
                break;
            }
        }
        laststate = digitalRead(this->pin);

        if (counter == 255) break;

        // ignore first 3 transitions
        if ((i >= 4) && (i % 2 == 0)) {
            // shove each bit into the storage bytes
            values[j / 8] <<= 1;
            if (counter > 16)
                values[j / 8] |= 1;
            j++;
        }
    }

    // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
    // print it out if data is good
    if ((j >= 40) 
            && (values[4] == ((values[0] + values[1] + values[2] + values[3]) & 0xFF))){
        float t, h;
        h = (float)values[0] * 256 + (float)values[1];
        h /= 10;
        t = (float)(values[2] & 0x7F)* 256 + (float)values[3];
        t /= 10.0;

        this->temperature = (double)t;
        this->humidity    = (double)h;
        this->error = 0;

        printf("Humidity = %.2f %% Temperature = %.2f *C \n", h, t );
    }
    else{
        printf("Data error\n");
        this->error = 1;
    }
}
