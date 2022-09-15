#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "usage: cache-thrash-single.sh <num threads>"
  echo ""
  echo "wraps a single run of cache-thrash with rss sampling"
  echo ""
  echo "example:"
  echo "  ./cache-thrash-single.sh 1 "
  exit 1
fi

if [[ $# -ne 2 ]]; then
  ALLOC="bx"
else
  ALLOC=$2
fi

BINARY=./cache-thrash_test
if [ "$ALLOC" == "je" ]; then
  BINARY="numactl --membind=2 "${BINARY}
fi

THREADS=$1

rm /tmp/cache-thrash
$BINARY $THREADS 5000 8 500000 > /tmp/cache-thrash

while read line; do
  if [[ $line == *"Time elapsed"* ]]; then
    exec_time=$(echo $line | awk '{print $4}')
    break
  fi
done < /tmp/cache-thrash

echo "{ \"threads\": $THREADS , \"time\":  $exec_time , \"allocator\": $ALLOC}"
echo "$THREADS,$exec_time,$ALLOC" >> cache-thrash.csv
