CXXFLAGS = -Wno-sign-compare -O3 -c -std=c++14 -g -Wall -fPIC `python3 -m pybind11 --includes` -DDLIB_NO_GUI_SUPPORT -I /usr/include/boost -I /usr/local/include/mongocxx/v_noabi -I /usr/local/include/bsoncxx/v_noabi -I ../engine/utils/dlib -I ../engine -I ../engine/utils
LFLAGS = -shared -L/usr/local/lib64 -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization -lboost_date_time -lboost_locale -lpqxx -lmongocxx -lbsoncxx -lcurl -llapack -lpthread
GCC = g++
EXT=`python3-config --extension-suffix`

term_doc_mat.so: dlib.o bson_builder.o article_builder.o lsa.o mongodb.o vocabulary.o murmur3.o porter2_stemmer.o term_doc_mat.o
	$(GCC) $(LFLAGS) dlib.o article_builder.o bson_builder.o murmur3.o porter2_stemmer.o mongodb.o vocabulary.o lsa.o term_doc_mat.o -o term_doc_mat`python3-config --extension-suffix`

term_doc_mat.o: term_doc_mat_bind.cpp ../engine/utils/article_utils.h ../engine/lsa.h
	$(GCC) $(CXXFLAGS) term_doc_mat_bind.cpp -o term_doc_mat.o

article_builder.o: ../engine/article_builder.cpp ../engine/article_builder.h ../engine/bson_builder.h ../engine/vocabulary.h ../engine/article.h 
	$(GCC) $(CXXFLAGS) ../engine/article_builder.cpp -o article_builder.o

bson_builder.o: ../engine/bson_builder.cpp ../engine/bson_builder.h ../engine/vocabulary.h ../engine/article.h 
	$(GCC) $(CXXFLAGS) ../engine/bson_builder.cpp -o bson_builder.o

mongodb.o: ../engine/mongodb.cpp ../engine/mongodb.h ../engine/article.h ../engine/vocabulary.h
	$(GCC) $(CXXFLAGS) ../engine/mongodb.cpp -o mongodb.o

vocabulary.o: ../engine/vocabulary.cpp ../engine/vocabulary.h ../engine/article.h
	$(GCC) $(CXXFLAGS) ../engine/vocabulary.cpp -o vocabulary.o

lsa.o: ../engine/lsa.cpp ../engine/lsa.h ../engine/article.h ../engine/vocabulary.h
	$(GCC) $(CXXFLAGS) ../engine/lsa.cpp -o lsa.o

porter2_stemmer.o: ../engine/utils/stemmer/porter2_stemmer.h ../engine/utils/stemmer/porter2_stemmer.cpp
	$(GCC) $(CXXFLAGS) -c ../engine/utils/stemmer/porter2_stemmer.cpp -o porter2_stemmer.o

murmur3.o: ../engine/utils/murmur3.c ../engine/utils/murmur3.h
	$(GCC) $(CXXFLAGS) ../engine/utils/murmur3.c -o murmur3.o


SRC = ../all/source.cpp 
SRC += matrix2.cpp
SRC += matrix3.cpp
SRC += matrix4.cpp
SRC += matrix_chol.cpp
SRC += matrix.cpp
SRC += matrix_eig.cpp
SRC += matrix_lu.cpp
SRC += matrix_qr.cpp

dlib.o: ../engine/utils/dlib/dlib/all/source.cpp
	$(GCC) $(CXXFLAGS) -c ../engine/utils/dlib/dlib/all/source.cpp -o dlib.o

#dep:
#	@makedepend -- $(CXXFLAGS) -- $(SRC) 2> /dev/null


clean:
	rm -rf *.o term_doc_mat`python3-config --extension-suffix`
