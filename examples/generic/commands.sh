#!/bin/bash


toggle() {
  echo "tx 10 44 6B" | cec-client -s -d 1
}

status() {
  echo "pow 0" | cec-client -s -d 1
}

while ps -e | grep cec-client; do :; done
$1
