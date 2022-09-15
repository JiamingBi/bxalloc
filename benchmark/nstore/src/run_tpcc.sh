#!/bin/bash
# Tracing 
# ./nstore -x400000 -k40000 -w -t -p0.4 -e4 -n1
# No Tracing
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -a -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -w -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -s -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -c -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -m -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x40000 -k100000 -l -t -p0.8 -e4
sudo rm -rf /mnt/pmem/*