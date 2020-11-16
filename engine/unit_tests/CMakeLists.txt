FILE(GLOB common_sources ../src/common/*.cpp)

set(TEST_DIRECTORY "../../bin/tests")


# files for test_article_parser
#################################
add_executable(test_article_parser test_article_parser.cpp)

target_include_directories(test_article_parser PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_article_parser PRIVATE ../extern/murmur3)
target_include_directories(test_article_parser PUBLIC ../include/common)
target_include_directories(test_article_parser PUBLIC ../include/clustering)

target_link_libraries(test_article_parser murmur3)
target_link_libraries(test_article_parser ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_article_parser ${Boost_LIBRARIES})
target_link_libraries(test_article_parser fmt::fmt)

target_sources(test_article_parser PRIVATE ../src/clustering/article_parser.cpp)
target_sources(test_article_parser PRIVATE ../src/common/vocabulary.cpp)

add_test(NAME article_parser_test COMMAND test_article_parser WORKING_DIRECTORY "../../bin/tests")
#################################


# files for test_bu_clustering
#################################
add_executable(test_bu_clustering test_bu_clustering.cpp)

target_include_directories(test_bu_clustering PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_bu_clustering PRIVATE ../extern/murmur3)

target_include_directories(test_bu_clustering PUBLIC ../include/common)
target_include_directories(test_bu_clustering PUBLIC ../include/clustering)
target_include_directories(test_bu_clustering PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_bu_clustering PRIVATE ../extern/murmur3)
target_include_directories(test_bu_clustering PRIVATE ../extern/dlib)

target_link_libraries(test_bu_clustering ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_bu_clustering ${Boost_LIBRARIES})
target_link_libraries(test_bu_clustering dlib::dlib)
target_link_libraries(test_bu_clustering fmt::fmt)

target_sources(test_bu_clustering PRIVATE ../src/common/vocabulary.cpp)
target_sources(test_bu_clustering PRIVATE ../src/clustering/bu_clustering.cpp)
target_sources(test_bu_clustering PRIVATE ../src/clustering/article_parser.cpp)
target_sources(test_bu_clustering PRIVATE ../src/clustering/lsa.cpp)
target_sources(test_bu_clustering PRIVATE ../src/clustering/lsa.cpp)

add_test(NAME bu_clustering_test COMMAND test_bu_clustering WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# files for test_lsa
#################################
add_executable(test_lsa test_lsa.cpp)

target_include_directories(test_lsa PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_lsa PRIVATE ../extern/murmur3)

target_include_directories(test_lsa PUBLIC ../include/common)
target_include_directories(test_lsa PUBLIC ../include/clustering)
target_include_directories(test_lsa PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_lsa PRIVATE ../extern/murmur3)
target_include_directories(test_lsa PRIVATE ../extern/dlib)

target_link_libraries(test_lsa ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_lsa ${Boost_LIBRARIES})
target_link_libraries(test_lsa dlib::dlib)
target_link_libraries(test_lsa fmt::fmt)

target_sources(test_lsa PRIVATE ../src/common/vocabulary.cpp)
target_sources(test_lsa PRIVATE ../src/clustering/article_parser.cpp)
target_sources(test_lsa PRIVATE ../src/clustering/lsa.cpp)

add_test(NAME lsa_test COMMAND test_lsa WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# files for test_lsh_deduplication
#################################
add_executable(test_lsh_deduplication test_lsh_deduplication.cpp)

target_include_directories(test_lsh_deduplication PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_lsh_deduplication PRIVATE ../extern/murmur3)

target_include_directories(test_lsh_deduplication PUBLIC ../include/common)
target_include_directories(test_lsh_deduplication PUBLIC ../include/dedup)
target_include_directories(test_lsh_deduplication PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_lsh_deduplication PRIVATE ../extern/murmur3)
target_include_directories(test_lsh_deduplication PRIVATE /usr/local/include/mongocxx/v_noabi)
target_include_directories(test_lsh_deduplication PRIVATE /usr/local/include/bsoncxx/v_noabi)

target_link_libraries(test_lsh_deduplication ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_lsh_deduplication ${Boost_LIBRARIES})
target_link_libraries(test_lsh_deduplication fmt::fmt)
target_link_libraries(test_lsh_deduplication ${ZeroMQ_LIBRARY})
target_link_libraries(test_lsh_deduplication ${CURL_LIBRARIES})
target_link_libraries(test_lsh_deduplication /usr/local/lib/libbsoncxx.so)
target_link_libraries(test_lsh_deduplication /usr/local/lib/libmongocxx.so)

target_sources(test_lsh_deduplication PRIVATE ../src/common/mongodb_helper.cpp)
target_sources(test_lsh_deduplication PRIVATE ../src/common/bson_builder.cpp)
target_sources(test_lsh_deduplication PRIVATE ../src/common/mongodb.cpp)

add_test(NAME lsh_deduplication_test COMMAND test_lsh_deduplication WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# files for test_minhash
#################################
add_executable(test_minhash test_minhash.cpp)

target_include_directories(test_minhash PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_minhash PRIVATE ../extern/murmur3)

target_include_directories(test_minhash PUBLIC ../include/common)
target_include_directories(test_minhash PUBLIC ../include/dedup)
target_include_directories(test_minhash PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_minhash PRIVATE ../extern/murmur3)
target_include_directories(test_minhash PUBLIC ../include/clustering)

target_link_libraries(test_minhash ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_minhash ${Boost_LIBRARIES})
target_link_libraries(test_minhash fmt::fmt)
target_link_libraries(test_minhash murmur3)

add_test(NAME minhash_test COMMAND test_minhash WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# files for test_mongodb
#################################
add_executable(test_mongodb test_mongodb.cpp)

target_include_directories(test_mongodb PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_mongodb PRIVATE ../extern/murmur3)

target_include_directories(test_mongodb PUBLIC ../include/common)
target_include_directories(test_mongodb PUBLIC ../include/clustering)
target_include_directories(test_mongodb PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_mongodb PRIVATE ../extern/murmur3)
target_include_directories(test_mongodb PRIVATE /usr/local/include/mongocxx/v_noabi)
target_include_directories(test_mongodb PRIVATE /usr/local/include/bsoncxx/v_noabi)

target_link_libraries(test_mongodb ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_mongodb ${Boost_LIBRARIES})
target_link_libraries(test_mongodb fmt::fmt)
target_link_libraries(test_mongodb ${CURL_LIBRARIES})
target_link_libraries(test_mongodb /usr/local/lib/libbsoncxx.so)
target_link_libraries(test_mongodb /usr/local/lib/libmongocxx.so)

target_sources(test_mongodb PRIVATE ../src/common/mongodb_helper.cpp)
target_sources(test_mongodb PRIVATE ../src/common/bson_builder.cpp)
target_sources(test_mongodb PRIVATE ../src/common/mongodb.cpp)
target_sources(test_mongodb PRIVATE ../src/clustering/article_parser.cpp)
target_sources(test_mongodb PRIVATE ../src/common/vocabulary.cpp)

add_test(NAME mongodb_test COMMAND test_mongodb WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# files for test_vocabulary
#################################
add_executable(test_vocabulary test_vocabulary.cpp)

target_include_directories(test_vocabulary PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_vocabulary PRIVATE ../extern/murmur3)

target_include_directories(test_vocabulary PUBLIC ../include/common)
target_include_directories(test_vocabulary PRIVATE ../extern/porter2_stemmer)
target_include_directories(test_vocabulary PRIVATE ../extern/murmur3)

target_link_libraries(test_vocabulary ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(test_vocabulary ${Boost_LIBRARIES})
target_link_libraries(test_vocabulary fmt::fmt)

target_sources(test_vocabulary PRIVATE ../src/common/vocabulary.cpp)

add_test(NAME vocabulary_test COMMAND test_vocabulary WORKING_DIRECTORY ${TEST_DIRECTORY})
#################################


# add tests target
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS test_vocabulary test_mongodb test_minhash test_article_parser test_bu_clustering test_lsa test_lsh_deduplication)

