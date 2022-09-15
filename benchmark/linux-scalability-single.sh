#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "usage: linux-scalability-single.sh <even num threads>"
  echo ""
  echo "wraps a single run of prod-con with rss sampling"
  echo ""
  echo "example:"
  echo "  ./linux-scalability-single.sh 2"
  exit 1
fi

if [[ $# -ne 2 ]]; then
  ALLOC="bx"
else
  ALLOC=$2
fi

BINARY=./linux-scalability_test
if [ "$ALLOC" == "je" ]; then
  BINARY="numactl --membind=2 "${BINARY}
fi

THREADS=$1

rm -f /tmp/linux-scalability
$BINARY 32 1000000 $THREADS  > /tmp/linux-scalability 

while read line; do
  if [[ $line == *"Time elapsed ="* ]]; then
    exec_time=$(echo $line | awk '{print $4}')
    break
  fi
done < /tmp/linux-scalability

echo "{ \"threads\": $THREADS , \"time\":  $exec_time , \"allocator\": $ALLOC}"
echo "$THREADS,$exec_time,$ALLOC" >> linux-scalability.csv
