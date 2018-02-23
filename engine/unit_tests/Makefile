all: test_vocabulary test_article_builder test_mongodb
CFLAGS = -std=c++14 -c -g -Wall -I /usr/include/boost -I /usr/include/pqxx -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi 
LFLAGS = -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization -lboost_date_time -lboost_locale -lpqxx -lmongocxx -lbsoncxx -lcurl
GCC = g++
EXE = ../../bin/tests/

test_vocabulary: test_vocabulary.o porter2_stemmer.o
	$(GCC) $(LFLAGS) porter2_stemmer.o test_vocabulary.o -o $(EXE)test_vocabulary

test_article_builder: test_article_builder.o porter2_stemmer.o mongodb.o murmur3.o
	$(GCC) $(LFLAGS) porter2_stemmer.o mongodb.o murmur3.o test_article_builder.o -o $(EXE)test_article_builder

test_mongodb: test_mongodb.o mongodb.o
	$(GCC) $(LFLAGS) mongodb.o test_mongodb.o -o $(EXE)test_mongodb

test_vocabulary.o: test_vocabulary.cpp ../vocabulary.h
	$(GCC) $(CFLAGS) test_vocabulary.cpp -o test_vocabulary.o

test_article_builder.o: test_article_builder.cpp ../article_builder.h ../lsh_deduplication.h
	$(GCC) $(CFLAGS) test_article_builder.cpp -o test_article_builder.o

test_mongodb.o: test_mongodb.cpp
	$(GCC) $(CFLAGS) test_mongodb.cpp -o test_mongodb.o

mongodb.o: ../mongodb.cpp ../mongodb.h ../article.h ../vocabulary.h
	$(GCC) $(CFLAGS) ../mongodb.cpp -o mongodb.o

porter2_stemmer.o: ../utils/stemmer/porter2_stemmer.h ../utils/stemmer/porter2_stemmer.cpp
	$(GCC) $(CFLAGS) -c ../utils/stemmer/porter2_stemmer.cpp -o porter2_stemmer.o

murmur3.o: ../utils/murmur3.c ../utils/murmur3.h
	$(GCC) $(CFLAGS) ../utils/murmur3.c -o murmur3.o


clean:
	rm -rf *.o $(EXE)test_vocabulary $(EXE)test_article_builder
