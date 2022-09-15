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
make cache-scratch_test ${ARGS}
rm -rf cache-scratch.csv
echo "thread,exec_time,allocator" >> cache-scratch.csv
for i in {1..3}
do
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	do
		rm -rf /mnt/pmem/*
		./cache-scratch-single.sh $threads $ALLOC
	done
done
# SEDARGS="2,\$s/$/"
# SEDARGS=${SEDARGS}","${ALLOC}"/"
# sed ${SEDARGS} -i prod-con.csv
NAME="./data/cache-scratch/cache-scratch_"${ALLOC}".csv"
cp cache-scratch.csv ${NAME}
