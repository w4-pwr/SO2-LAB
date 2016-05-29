CCFLAGS = -lncurses -pthread -Wno-write-strings

all: main

main: main.cpp
	g++ main.cpp -o main $(CCFLAGS) 

test: all
	./main 

clean:
	rm -rf *.o main
