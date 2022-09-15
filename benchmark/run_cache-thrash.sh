#!/bin/bash
if [[ $# -ne 1 ]]; then
  ALLOC="bx"
else
  ALLOC=$1
fi
ARGS="ALLOC="
ARGS=${ARGS}${ALLOC}
echo $ARGS

make clean
make cache-thrash_test ${ARGS}
rm -rf cache-thrash.csv
echo "thread,exec_time,allocator" >> cache-thrash.csv
for i in {1..3}
do
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	do
		rm -rf /mnt/pmem/*
		./cache-thrash-single.sh $threads $ALLOC
	done
done
# SEDARGS="2,\$s/$/"
# SEDARGS=${SEDARGS}","${ALLOC}"/"
# sed ${SEDARGS} -i prod-con.csv
NAME="./data/cache-thrash/cache-thrash_"${ALLOC}".csv"
cp cache-thrash.csv ${NAME}
