#!/bin/sh 

ifconfig wlan0 down; iwconfig wlan0 mode ad-hoc
iwconfig wlan0 essid repad
ifconfig wlan0 192.168.0.200/24
while [ $? -gt 0 ]; do
    sleep 1
    ifconfig wlan0 192.168.0.200/24
done
