#include "dht.hpp"

static timeval_t get_time(){
  timeval_t tv;
  gettimeofday(&tv, NULL);
  return tv;
}

DHT::DHT(int pin){
  this->pin = pin;
}


void DHT::init(){
  wiringPiSetup();
  this->thr = new std::thread(&DHT::read_data_non_stop, this);
}

bool DHT::has_timeout(timeval_t& init, timeval_t& end, uint32_t max_time){
  uint32_t diff_secs = (end.tv_sec - init.tv_sec) * 1000000;
  if ((diff_secs + end.tv_usec) - init.tv_usec > max_time) return true;
  return false;
}

void DHT::waitForSignal(int signal, uint32_t max_time){
  timeval_t init, end;
  init = get_time();
  while (digitalRead(this->pin) != signal){
    end = get_time();
    if (has_timeout(init, end, max_time)) return;
  }
}

void DHT::close(){
  this->running = false;
  this->thr->join();
  delete this->thr;
}

void DHT::handshake(){
  pinMode(this->pin, OUTPUT);
  digitalWrite(this->pin, LOW);
  delay(18);
  digitalWrite(this->pin, HIGH);
  delayMicroseconds(40);
  pinMode(this->pin, INPUT);
}

void DHT::read_data_non_stop(){
  uint8_t counter = 0;
  while (running){
    read_data();
    sleep(INTERVAL_TIME);
    if (counter == 60) {
      counter = 0;
      wiringPiSetup();
    }
    counter++;
  }
}

void DHT::read_data(){}

double DHT::get_temperature(){
  return this->temperature;
}

double DHT::get_humidity(){
  return this->humidity;
}

bool DHT::has_errors(){
  if (this->error != 0)
    return true;
  return false;
}
