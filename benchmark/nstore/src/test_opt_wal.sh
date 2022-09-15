#!/bin/bash
# On skylake
# Tracing
# ./nstore -x8000000 -k100000 -w -y -p0.8 -e4 -n1
# No tracing
sudo rm -rf /mnt/pmem/*
echo "****************"
echo "pm"
echo "****************"
sudo ./nstore -x20000 -k50000 -w -y -p0.9 -q0.1 -e8 -P1
sudo rm -rf /mnt/pmem/*
echo "****************"
echo "ralloc"
echo "****************"
sudo ./nstore -x20000 -k50000 -w -y -p0.9 -q0.1 -e8 -P2
sudo rm -rf /mnt/pmem/*
echo "****************"
echo "makalu_alloc"
echo "****************"
sudo ./nstore -x20000 -k50000 -w -y -p0.9 -q0.1 -e8 -P3
sudo rm -rf /mnt/pmem/*l
echo "****************"
echo "nvmmalloc"
echo "****************"
sudo ./nstore -x20000 -k50000 -w -y -p0.9 -q0.1 -e8 -P4
sudo rm -rf /mnt/pmem/*
echo "****************"
echo "pmdk"
echo "****************"
sudo ./nstore -x20000 -k50000 -w -y -p0.9 -q0.1 -e8 -P5
sudo rm -rf /mnt/pmem/*
