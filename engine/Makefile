all: engine
CFLAGS = -std=c++14 -c -g -Wall -I /usr/include/boost -I /usr/include/pqxx -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi
LFLAGS = -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lboost_locale -lpqxx -lmongocxx -lbsoncxx
GCC = g++
EXE = ../bin/

engine: engine.o murmur3.o mongodb.o
	$(GCC) $(LFLAGS) murmur3.o mongodb.o engine.o -o $(EXE)engine_main

engine.o: engine.cpp config.h pqldb.h thread_pool.h entry_processor.h
	$(GCC) $(CFLAGS) engine.cpp -o engine.o

mongodb.o: mongodb.cpp mongodb.h article.h vocabulary.h
	$(GCC) $(CFLAGS) mongodb.cpp -o mongodb.o

murmur3.o: utils/murmur3.c utils/murmur3.h
	$(GCC) $(CFLAGS) utils/murmur3.c -o murmur3.o



clean:
	rm -rf *.o ../bin/engine
