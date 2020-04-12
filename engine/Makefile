all: engine zmq_broker article_dedup
CFLAGS = -Wno-sign-compare -std=c++17 -c -g -Wall -DDLIB_NO_GUI_SUPPORT -I /usr/include/boost -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi -I utils/dlib -I /usr/include -I /home/spdlog/spdlog/include

LFLAGS = -L/usr/local/lib64 -lboost_system -lboost_filesystem -lboost_thread -lboost_date_time -lboost_serialization -lboost_locale -lpqxx -lmongocxx -lbsoncxx -lcurl -lpthread -llapack -lzmq -lspdlog
GCC = g++
EXE = ../bin/

engine: engine.o vocabulary.o pipeline.o bu_clustering.o murmur3.o mongodb.o porter2_stemmer.o bson_builder.o article_parser.o dlib.o lsa.o
	$(GCC) $(LFLAGS) vocabulary.o pipeline.o bu_clustering.o porter2_stemmer.o murmur3.o mongodb.o engine.o bson_builder.o article_parser.o dlib.o lsa.o -o $(EXE)engine_main

zmq_broker: zmq_broker.o
	$(GCC) $(LFLAGS) zmq_broker.o -o $(EXE)zmq_broker

article_dedup: article_dedup.o mongodb_helper.o bson_builder.o mongodb.o
	$(GCC) $(LFLAGS) article_dedup.o mongodb_helper.o bson_builder.o mongodb.o -o $(EXE)article_dedup

engine.o: engine.cpp config.h
	$(GCC) $(CFLAGS) engine.cpp -o engine.o

zmq_broker.o: zmq_broker.cpp
	$(GCC) $(CFLAGS) zmq_broker.cpp -o zmq_broker.o

article_dedup.o: article_dedup.cpp article_dedup.h zmq_helper.h minhash.h lsh_deduplication.h config.h
	$(GCC) $(CFLAGS) article_dedup.cpp -o article_dedup.o

pipeline.o: pipeline.cpp pipeline.h article.h article_parser.h vocabulary.h
	$(GCC) $(CFLAGS) pipeline.cpp -o pipeline.o

bu_clustering.o: bu_clustering.cpp clustering.h lsa.h vocabulary.h
	$(GCC) $(CFLAGS) bu_clustering.cpp -o bu_clustering.o

lsa.o: lsa.cpp lsa.h article.h vocabulary.h
	$(GCC) $(CFLAGS) lsa.cpp -o lsa.o

vocabulary.o: vocabulary.cpp vocabulary.h
	$(GCC) $(CFLAGS) vocabulary.cpp -o vocabulary.o

article_parser.o: article_parser.cpp article_parser.h vocabulary.h article.h 
	$(GCC) $(CFLAGS) article_builder.cpp -o article_builder.o

bson_builder.o: bson_builder.cpp bson_builder.h vocabulary.h article.h
	$(GCC) $(CFLAGS) bson_builder.cpp -o bson_builder.o

mongodb.o: mongodb.cpp mongodb.h article.h vocabulary.h
	$(GCC) $(CFLAGS) mongodb.cpp -o mongodb.o

mongodb_helper.o: mongodb_helper.h mongodb_helper.cpp
	$(GCC) $(CFLAGS) mongodb_helper.cpp -o mongodb_helper.o

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
	rm -rf *.o ../bin/engine ../bin/zmq_broker ../bin/article_dedup
