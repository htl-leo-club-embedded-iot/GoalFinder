#!/bin/bash

set -e 

FORCE=0
MONITOR=0

while getopts "fm" opt; do
  case $opt in
    f) FORCE=1 ;;
    m) MONITOR=1 ;;
    \?)
      echo "Usage: $0 [-f] [-m]"
      exit 1
      ;;
  esac
done

cd client/web || exit 1
npm run build

cd ../embedded || exit 1

if [ $FORCE -eq 1 ]; then
  pio run -t erase
  pio run -t clean
fi

pio run -t upload

pio run -t uploadfs

if [ $MONITOR -eq 1 ]; then
  pio device monitor
fi

cd ../..