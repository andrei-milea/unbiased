all: test_vocabulary test_article_builder test_mongodb test_minhash test_lsa
CFLAGS = -std=c++14 -c -g -Wall -DDLIB_NO_GUI_SUPPORT -I /usr/include/boost -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi -I ../utils/dlib
LFLAGS = -L/usr/local/lib64 -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization -lboost_date_time -lboost_locale -lpqxx -lmongocxx -lbsoncxx -lcurl -llapack -lpthread
GCC = g++
EXE = ../../bin/tests/

test_vocabulary: test_vocabulary.o porter2_stemmer.o
	$(GCC) $(LFLAGS) porter2_stemmer.o test_vocabulary.o -o $(EXE)test_vocabulary

test_article_builder: test_article_builder.o porter2_stemmer.o mongodb.o murmur3.o
	$(GCC) $(LFLAGS) porter2_stemmer.o mongodb.o murmur3.o test_article_builder.o -o $(EXE)test_article_builder

test_mongodb: test_mongodb.o mongodb.o murmur3.o porter2_stemmer.o
	$(GCC) $(LFLAGS) murmur3.o porter2_stemmer.o mongodb.o test_mongodb.o -o $(EXE)test_mongodb

test_lsa: dlib.o test_lsa.o lsa.o mongodb.o murmur3.o porter2_stemmer.o
	$(GCC) $(LFLAGS) dlib.o murmur3.o porter2_stemmer.o mongodb.o lsa.o test_lsa.o -o $(EXE)test_lsa

test_minhash: test_minhash.o murmur3.o
	$(GCC) $(LFLAGS) murmur3.o test_minhash.o -o $(EXE)test_minhash

test_vocabulary.o: test_vocabulary.cpp ../vocabulary.h
	$(GCC) $(CFLAGS) test_vocabulary.cpp -o test_vocabulary.o

test_article_builder.o: test_article_builder.cpp ../article_builder.h ../lsh_deduplication.h ../utils/article_utils.h
	$(GCC) $(CFLAGS) test_article_builder.cpp -o test_article_builder.o

test_mongodb.o: test_mongodb.cpp ../utils/article_utils.h
	$(GCC) $(CFLAGS) test_mongodb.cpp -o test_mongodb.o

test_lsa.o: test_lsa.cpp ../utils/article_utils.h ../lsa.cpp ../lsa.h
	$(GCC) $(CFLAGS) test_lsa.cpp -o test_lsa.o

test_minhash.o: test_minhash.cpp ../minhash.h
	$(GCC) $(CFLAGS) test_minhash.cpp -o test_minhash.o

mongodb.o: ../mongodb.cpp ../mongodb.h ../article.h ../vocabulary.h
	$(GCC) $(CFLAGS) ../mongodb.cpp -o mongodb.o

lsa.o: ../lsa.cpp ../lsa.h ../article.h ../vocabulary.h
	$(GCC) $(CFLAGS) ../lsa.cpp -o lsa.o

porter2_stemmer.o: ../utils/stemmer/porter2_stemmer.h ../utils/stemmer/porter2_stemmer.cpp
	$(GCC) $(CFLAGS) -c ../utils/stemmer/porter2_stemmer.cpp -o porter2_stemmer.o

murmur3.o: ../utils/murmur3.c ../utils/murmur3.h
	$(GCC) $(CFLAGS) ../utils/murmur3.c -o murmur3.o


SRC = ../all/source.cpp 
SRC += matrix2.cpp
SRC += matrix3.cpp
SRC += matrix4.cpp
SRC += matrix_chol.cpp
SRC += matrix.cpp
SRC += matrix_eig.cpp
SRC += matrix_lu.cpp
SRC += matrix_qr.cpp

dlib.o: ../utils/dlib/dlib/all/source.cpp
	$(GCC) $(CFLAGS) -c ../utils/dlib/dlib/all/source.cpp -o dlib.o

#dep:
#	@makedepend -- $(CFLAGS) -- $(SRC) 2> /dev/null


clean:
	rm -rf *.o $(EXE)test_vocabulary $(EXE)test_article_builder $(EXE)test_mongodb $(EXE)test_minhash $(EXE)test_lsa
