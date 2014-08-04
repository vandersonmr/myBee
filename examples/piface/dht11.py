#!/usr/bin/python

import pifacedigitalio as piface
from sys import argv
from time import sleep, time

HIGH = 1
LOW = 0

def getMicroTime():
    return time() * 1000000

class dht11:
    def __init__(self, port):
        self.port = port

    def getData(self):
        self._handshake()
        data = []
        for i in range(0, 5):
            value = 0
            for j in range(0, 8):
                _waitForSignal(HIGH, 60)
                micro_init = getMicroTime()
                _waitForSignal(LOW, 100)
                micro_now = getMicroTime()
                if (micro_now - micro_init > 40):
                    value |= 1 << (7 - j)
            data.push(value)
        checksum = data[4]
        if (data[1] + data[3] != checksum):
            print 'Checksum error'
            exit(1)
        h = data[1]
        t = data[3]
        return t, h

    # time is used as a timeout
    def _waitForSignal(self, signal, time):
        micro_init = getMicroTime()
        while piface.digital_read(self.port) != signal:
            micro_now = getMicroTime()
            if (micro_now - micro_init > time):
                print 'Timeout'
                exit(1)

    # init the communication between the piface and the sensor
    def _handshake(self):
        piface.digital_write(self.port, LOW)
        sleep(0.018) # 18ms
        piface.digital_write(self.port, HIGH)
        sleep(0.00004) # 40us
        _waitForSignal(HIGH, 90) # 90us
        _waitForSignal(LOW, 90) # 90us

if __name__ == '__main__':
    dht = dht11(int(argv[1]))
    temperature, humidity = dht.getData()
    print 'Temperature: %.2f\nHumidity: %.2f' % (temperature, humidity)
