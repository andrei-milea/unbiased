add_executable(clustering main.cpp)

add_executable(clustering_startup startup.cpp)

# add all source files
FILE(GLOB clustering_sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
FILE(GLOB common_sources ../../src/common/*.cpp)

get_filename_component(full_path_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ABSOLUTE)
get_filename_component(full_path_startup_cpp ${CMAKE_CURRENT_SOURCE_DIR}/startup.cpp ABSOLUTE)

list(REMOVE_ITEM clustering_sources "${full_path_main_cpp}")
list(REMOVE_ITEM clustering_sources "${full_path_startup_cpp}")

target_sources(clustering PRIVATE "${full_path_main_cpp}")
target_sources(clustering_startup PRIVATE "${full_path_startup_cpp}")

add_library(murmur3 ../../extern/murmur3/murmur3.c ../../extern/murmur3/murmur3.h)

set(EXECUTABLES "clustering" "clustering_startup")

foreach( executable ${EXECUTABLES})
    target_compile_features(${executable} PRIVATE cxx_std_17)

    set_target_properties(${executable} PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ../../bin/debug
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ../../bin/release)

    # add include directories for external libraries
    target_include_directories(${executable} PRIVATE ../../extern/murmur3)
    target_include_directories(${executable} PRIVATE ../../extern/porter2_stemmer)
    target_include_directories(${executable} PRIVATE /usr/local/include/mongocxx/v_noabi)
    target_include_directories(${executable} PRIVATE /usr/local/include/bsoncxx/v_noabi)

    target_include_directories(${executable} PRIVATE ${CURL_INCLUDE_DIRS})

    # add include directories for local files
    target_include_directories(${executable} PUBLIC ../../include/common)
    target_include_directories(${executable} PRIVATE ../../include/clustering)

    # add external libraries
    target_link_libraries(${executable} ${Boost_LIBRARIES})
    target_link_libraries(${executable} murmur3)
    target_link_libraries(${executable} ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
    target_link_libraries(${executable} fmt::fmt)
    target_link_libraries(${executable} ${ZeroMQ_LIBRARY})
    target_link_libraries(${executable} ${CURL_LIBRARIES})
    #target_link_libraries(${executable} ${LIBBSONCXX_LIBRARIES})
    target_link_libraries(${executable} /usr/local/lib/libbsoncxx.so)
    target_link_libraries(${executable} /usr/local/lib/libmongocxx.so)
    target_link_libraries(${executable} dlib::dlib)

    target_sources(${executable} PRIVATE ${common_sources})
    target_sources(${executable} PRIVATE ${clustering_sources})
endforeach( executable ${EXECUTABLES})

