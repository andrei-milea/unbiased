add_executable(clustering clustering.cpp)

target_compile_features(clustering PRIVATE cxx_std_17)


set_target_properties(clustering PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../bin/debug
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../bin/release)

add_library(murmur3 ../../extern/murmur3/murmur3.c ../../extern/murmur3/murmur3.h)

# add include directories for external libraries
target_include_directories(clustering PRIVATE ../../extern/dlib)
target_include_directories(clustering PRIVATE ../../extern/porter2_stemmer)
target_include_directories(clustering PRIVATE ../../extern/murmur3)
target_include_directories(clustering PRIVATE /usr/local/include/mongocxx/v_noabi)
target_include_directories(clustering PRIVATE /usr/local/include/bsoncxx/v_noabi)

target_include_directories(clustering PRIVATE ${CURL_INCLUDE_DIRS})

# add include directories for local files
target_include_directories(clustering PUBLIC ../../include/common)
target_include_directories(clustering PRIVATE ../../include/clustering)

# add external libraries
target_link_libraries(clustering ${Boost_LIBRARIES})
target_link_libraries(clustering murmur3)
target_link_libraries(clustering ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(clustering fmt::fmt)
target_link_libraries(clustering ${ZeroMQ_LIBRARY})
target_link_libraries(clustering ${CURL_LIBRARIES})
#target_link_libraries(clustering ${LIBBSONCXX_LIBRARIES})
target_link_libraries(clustering /usr/local/lib/libbsoncxx.so)
target_link_libraries(clustering ${LIBMONGOCXX_LIBRARY_PATH})

target_link_libraries(clustering /usr/local/lib/libmongocxx.so)
target_link_libraries(clustering dlib::dlib)

# add all source files
FILE(GLOB clustering_sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
FILE(GLOB common_sources ../../src/common/*.cpp)
target_sources(clustering PRIVATE ${common_sources})
target_sources(clustering PRIVATE ${clustering_sources})




