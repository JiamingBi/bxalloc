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
make linux-scalability_test ${ARGS}
rm -rf linux-scalability.csv
echo "thread,exec_time,allocator" >> linux-scalability.csv
for i in {1..3}
do
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	do
		rm -rf /mnt/pmem/*
		./linux-scalability-single.sh $threads $ALLOC
	done
done
# SEDARGS="2,\$s/$/"
# SEDARGS=${SEDARGS}","${ALLOC}"/"
# sed ${SEDARGS} -i prod-con.csv
NAME="./data/linux-scalability/linux-scalability_"${ALLOC}".csv"
cp linux-scalability.csv ${NAME}
