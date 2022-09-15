##
# Copyright (C) 2019 University of Rochester. All rights reserved.
# Licenced under the MIT licence. See LICENSE file in the project root for
# details. 
##

SRC = ./src
OBJ = ./
CXX = g++

WARNING_FLAGS:=-ftrapv -Wreturn-type -W -Wall \
-Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter

FLAGS = -O0 -g -fpermissive $(WARNING_FLAGS) -fno-omit-frame-pointer -fPIC #-DSHM_SIMULATING #-DDESTROY -DMEM_CONSUME_TEST
RALLOC_FLAGS = $(FLAGS) -DRALLOC -L.

R_CXXFLAGS = $(RALLOC_FLAGS) -ljemalloc -L. -lralloc 


# Ralloc by default
CXXFLAGS = $(R_CXXFLAGS)
ifeq ($(ALLOC),r)
	CXXFLAGS = $(R_CXXFLAGS)
endif

LIBS = -pthread -lstdc++ -latomic 

all: libralloc.a

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) -I $(SRC) -o $@ -c $^ $(CXXFLAGS)

libralloc.a: $(OBJECTS)
	ar -rcs $@ $^

clean:
	rm -f *.o
	rm -f libralloc.a
