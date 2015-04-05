#!/bin/sh 

lan=wlan0

ifconfig $lan down; iwconfig $lan mode ad-hoc
iwconfig $lan essid repad
ifconfig $lan 192.168.0.200/24
while [ $? -gt 0 ]; do
    sleep 1
    ifconfig $lan 192.168.0.200/24
done
