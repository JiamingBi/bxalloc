#!/bin/bash
# On skylake
# Tracing
# ./nstore -x8000000 -k100000 -w -y -p0.8 -e4 -n1
# No tracing
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x2000 -k5000 -a -y -p0.9 -q0.1 -e8 
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x2000 -k5000 -w -y -p0.9 -q0.1 -e8
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x2000 -k5000 -s -y -p0.9 -q0.1 -e8
sudo rm -rf /mnt/pmem/*
sudo ./nstore -x2000 -k5000 -m -y -p0.9 -q0.1 -e8
