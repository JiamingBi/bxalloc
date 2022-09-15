#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "usage: cache-scratch-single.sh <num threads>"
  echo ""
  echo "wraps a single run of cache-scratch with rss sampling"
  echo ""
  echo "example:"
  echo "  ./cache-scratch-single.sh 1 "
  exit 1
fi

if [[ $# -ne 2 ]]; then
  ALLOC="bx"
else
  ALLOC=$2
fi

BINARY=./cache-scratch_test
if [ "$ALLOC" == "je" ]; then
  BINARY="numactl --membind=2 "${BINARY}
fi

THREADS=$1

rm /tmp/cache-scratch
$BINARY $THREADS 1000 8 1000000 > /tmp/cache-scratch

while read line; do
  if [[ $line == *"Time elapsed"* ]]; then
    exec_time=$(echo $line | awk '{print $4}')
    break
  fi
done < /tmp/cache-scratch

echo "{ \"threads\": $THREADS , \"time\":  $exec_time , \"allocator\": $ALLOC}"
echo "$THREADS,$exec_time,$ALLOC" >> cache-scratch.csv
