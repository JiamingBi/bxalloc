
SRC = ./src
OBJ = ./obj
CXX = g++

WARNING_FLAGS:=-ftrapv -Wreturn-type -W -Wall \
-Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter

FLAGS = -O3 -g -fpermissive $(WARNING_FLAGS) -fno-omit-frame-pointer -fPIC -ljemalloc 


# Ralloc by default
CXXFLAGS = $(R_CXXFLAGS)


LIBS = -pthread -lstdc++ -latomic -std=c++17


SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) -std=c++17 -I $(SRC) -o $@ -c $^ $(FLAGS) $(LIBS)


#libballoc.a: $(OBJECTS)
#	ar -rcs $@ $^

all:libbxalloc.so 

libbxalloc.so: $(OBJECTS)
	$(CXX) -shared $(OBJECTS) -o $@


clean:
	rm -rf ./obj/*
	rm -f libbxalloc.so
