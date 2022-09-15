CXX = g++

LIBS = -pthread -lstdc++ -latomic -std=c++17

main:
	$(CXX) main.cpp -o $@ -I../src -L../ -lbxalloc   -Wl,-rpath=../ $(LIBS)


 clean:
	rm -rf main
