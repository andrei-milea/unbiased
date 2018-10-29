all: engine
CFLAGS = -std=c++14 -c -g -Wall -DDLIB_NO_GUI_SUPPORT -I /usr/include/boost -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi -I utils/dlib

LFLAGS = -L/usr/local/lib64 -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lboost_serialization -lboost_locale -lpqxx -lmongocxx -lbsoncxx -lcurl -lpthread -llapack
GCC = g++
EXE = ../bin/

engine: engine.o murmur3.o mongodb.o porter2_stemmer.o dlib.o lsa.o
	$(GCC) $(LFLAGS) porter2_stemmer.o murmur3.o mongodb.o engine.o dlib.o lsa.o -o $(EXE)engine_main

engine.o: engine.cpp config.h pqldb.h thread_pool.h entry_processor.h
	$(GCC) $(CFLAGS) engine.cpp -o engine.o

lsa.o: lsa.cpp lsa.h article.h vocabulary.h
	$(GCC) $(CFLAGS) lsa.cpp -o lsa.o

mongodb.o: mongodb.cpp mongodb.h article.h vocabulary.h
	$(GCC) $(CFLAGS) mongodb.cpp -o mongodb.o

murmur3.o: utils/murmur3.c utils/murmur3.h
	$(GCC) $(CFLAGS) utils/murmur3.c -o murmur3.o

porter2_stemmer.o: utils/stemmer/porter2_stemmer.h utils/stemmer/porter2_stemmer.cpp
	$(GCC) $(CFLAGS) -c utils/stemmer/porter2_stemmer.cpp -o porter2_stemmer.o

SRC = ../all/source.cpp 
SRC += matrix2.cpp
SRC += matrix3.cpp
SRC += matrix4.cpp
SRC += matrix_chol.cpp
SRC += matrix.cpp
SRC += matrix_eig.cpp
SRC += matrix_lu.cpp
SRC += matrix_qr.cpp

dlib.o: utils/dlib/dlib/all/source.cpp
	$(GCC) $(CFLAGS) -c utils/dlib/dlib/all/source.cpp -o dlib.o

clean:
	rm -rf *.o ../bin/engine
