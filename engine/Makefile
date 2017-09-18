all: main
CFLAGS = -std=c++14 -c -g -Wall -I /usr/include/boost -I /usr/include/pqxx 
LFLAGS = -lboost_thread -lboost_date_time -lpqxx
GCC = g++
EXE = ../bin/

main: main.o
	$(GCC) $(LFLAGS) main.o -o $(EXE)engine_main

main.o: main.cpp config.h db.h
	$(GCC) $(CFLAGS) main.cpp -o main.o

clean:
	rm -rf *.o ../bin/main
