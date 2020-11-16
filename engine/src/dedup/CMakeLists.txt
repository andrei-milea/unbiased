add_executable(dedup article_dedup.cpp)

target_compile_features(dedup PRIVATE cxx_std_17)

set_target_properties(dedup PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../bin/debug
                      RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../bin/release)

# add include directories for external libraries
target_include_directories(dedup PRIVATE /usr/local/include/mongocxx/v_noabi)
target_include_directories(dedup PRIVATE /usr/local/include/bsoncxx/v_noabi)
target_include_directories(dedup PRIVATE ../../extern/porter2_stemmer)
target_include_directories(dedup PRIVATE ${CURL_INCLUDE_DIRS})


# add include directories for local files
target_include_directories(dedup PRIVATE ../../include/common)
target_include_directories(dedup PRIVATE ../../include/dedup)

# add external libraries
target_link_libraries(dedup ${Boost_LIBRARIES})
target_link_libraries(dedup fmt::fmt)
target_link_libraries(dedup ${ZeroMQ_LIBRARY})

target_link_libraries(dedup ${CURL_LIBRARIES})
#target_link_libraries(dedup ${LIBMONGOCXX_LIBRARIES})
target_link_libraries(dedup /usr/local/lib/libmongocxx.so)
#target_link_libraries(dedup ${LIBBSONCXX_LIBRARIES})
target_link_libraries(dedup /usr/local/lib/libbsoncxx.so)
target_link_libraries(dedup ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)

# add all source files
FILE(GLOB common_sources ../../src/common/*.cpp)
target_sources(dedup PRIVATE ${common_sources})
FILE(GLOB dedup_sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
target_sources(dedup PRIVATE ${dedup_sources})

