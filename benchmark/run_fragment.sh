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
make fragment_test ${ARGS}

function rand(){
  min=$1
  max=$(($2-$min+1))
  num=$(date +%s%N)
  echo $(($num%$max+$min))
}

num=0;
ops=0;
for i in {1..1}
do
    #rnd=$(rand 1 20480)
    for rnd in 8 65 130 178 298 630 740 850 950 1050 1150 2250 3350 4092 5555 6540 7640 8530 9748 10028
    do
	    rm -rf /mnt/pmem/*
        rm -f /tmp/fragment
	    ./fragment_test $rnd > /tmp/fragment
        while read line; do
            if [[ $line == "used rate 0 is"* ]]; then
                ops=$(echo $line | awk '{print $5}')
                echo $ops
                break
            fi
        done < /tmp/fragment
        num=$[$num+$ops]
    done
done
k=20000000
#echo $num
var1=$(echo "scale=6;$num / $k"|bc)
echo "$var1"


#bx  .968788
#mak .918290
#nvm .672873