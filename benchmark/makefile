##
# Copyright (C) 2019 University of Rochester. All rights reserved.
# Licenced under the MIT licence. See LICENSE file in the project root for
# details. 
##

SRC = ../src
OBJ = ../obj
CXX = g++

WARNING_FLAGS:=-ftrapv -Wreturn-type -W -Wall \
-Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter

FLAGS = -O3 -g -fpermissive $(WARNING_FLAGS) -fno-omit-frame-pointer -fPIC -std=c++17 #-DSHM_SIMULATING #-DDESTROY -DMEM_CONSUME_TEST
BXALLOC_FLAGS = $(FLAGS) -I../src -DBXALLOC -L../ -lbxalloc -Wl,-rpath=../ 
MAKALU_FLAGS = $(FLAGS) -I./ext_alloc/makalu/include -DMAKALU -L./ext_alloc/makalu/lib -lmakalu -Wl,-rpath=./ext_alloc/makalu/lib 
NVM_FLAGS = $(FLAGS) -I./ext_alloc/nvm_malloc/src -DNVMMALLOC -L./ext_alloc/nvm_malloc -lnvmmalloc -Wl,-rpath=./ext_alloc/nvm_malloc 
PMDK_FLAGS = $(FLAGS) -DPMDK -lpmemobj 

BX_CXXFLAGS = $(BXALLOC_FLAGS) -ljemalloc -L../ -lbxalloc -Wl,-rpath=../
MAK_CXXFLAGS = $(MAKALU_FLAGS) -ljemalloc -L./ext_alloc/makalu/lib -lmakalu -Wl,-rpath=./ext_alloc/makalu/lib 
JE_CXXFLAGS = $(FLAGS) -ljemalloc
NVM_CXXFLAGS = $(NVM_FLAGS) -L./ext_alloc/nvm_malloc -lnvmmalloc -Wl,-rpath=./ext_alloc/nvm_malloc 
PMDK_CXXFLAGS = $(PMDK_FLAGS) -ljemalloc

CXXFLAGS = $(BX_CXXFLAGS)
ifeq ($(ALLOC),bx)
	CXXFLAGS = $(BX_CXXFLAGS)
endif

ifeq ($(ALLOC),mak)
	CXXFLAGS = $(MAK_CXXFLAGS)
endif

ifeq ($(ALLOC),je)
	CXXFLAGS = $(JE_CXXFLAGS)
endif

ifeq ($(ALLOC),nvm)
	CXXFLAGS = $(NVM_CXXFLAGS) 
endif

ifeq ($(ALLOC),pmdk)
	CXXFLAGS = $(PMDK_CXXFLAGS)
endif

LIBS = -pthread -lstdc++ -latomic -std=c++17

all: benchmark_pm


benchmark_pm: threadtest_test larson_test prod-con_test cache-scratch_test cache-thrash_test linux-scalability_test fragment_test sh6bench_test

threadtest_test: ./bench/threadtest.cpp 
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 

larson_test: ./bench/larson.cpp 
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 

prod-con_test: ./bench/prod-con.cpp 
	$(CXX)  -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 

cache-scratch_test:./bench/cache-scratch.cpp
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 

cache-thrash_test:./bench/cache-thrash.cpp
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 
	
linux-scalability_test:./bench/linux-scalability.cpp
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS) 

fragment_test:./bench/bench-fragment.cpp
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS)

sh6bench_test:./bench/sh6bench.cpp
	$(CXX) -I $(SRC) -I ./bench -o $@ $^ $(CXXFLAGS) $(LIBS)
	

clean:
	rm -rf threadtest_test larson_test prod-con_test cache-scratch_test cache-thrash_test linux-scalability_test fragment_test sh6bench_test
	rm -rf /mnt/pmem/*
