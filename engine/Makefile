all: main
CFLAGS = -std=c++14 -c -g -Wall -I /usr/include/boost -I /usr/include/pqxx 
LFLAGS = -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lpqxx
GCC = g++
EXE = ../bin/

main: main.o murmur3.o
	$(GCC) $(LFLAGS) murmur3.o main.o -o $(EXE)engine_main

main.o: main.cpp config.h db.h thread_pool.h article_processor.h types.h
	$(GCC) $(CFLAGS) main.cpp -o main.o

murmur3.o: murmur3.c murmur3.h
	$(GCC) $(CFLAGS) murmur3.c -o murmur3.o



clean:
	rm -rf *.o ../bin/main
