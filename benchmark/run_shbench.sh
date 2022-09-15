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
make sh6bench_test ${ARGS}
rm -rf shbench.csv
echo "thread,exec_time,allocator" >> shbench.csv
for i in {1..3}
do
	for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20
	do
		rm -rf /mnt/pmem/*
		#sleep 1
		./shbench-single.sh $threads $ALLOC
	done
done
# SEDARGS="2,\$s/$/"
# SEDARGS=${SEDARGS}","${ALLOC}"/"
# echo $SEDARGS
# sed ${SEDARGS} -i shbench.csv
NAME="./data/shbench/shbench_"${ALLOC}".csv"
cp shbench.csv ${NAME}
