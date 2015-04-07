#!/bin/bash


toggle() {
  echo "tx 10 44 6B" | cec-client -s -d 1
}

status() {
  echo "pow 0" | cec-client -s -d 1
}

timenow=$(date +%s)
while ps -e | grep cec-client; do 
  let tim=$(date +%s)-$timenow; 
  if [ $tim -ge 10 ]; then
    pkill cec-client;
  fi;
done
$1
