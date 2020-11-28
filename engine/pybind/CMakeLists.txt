#add_library(term_doc_mat_bind SHARED term_doc_mat_bind.cpp)
pybind11_add_module(term_doc_mat_bind term_doc_mat_bind.cpp)

# add all source files
#FILE(GLOB bind_sources ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
FILE(GLOB clustering_sources ${CMAKE_SOURCE_DIR}/src/clustering/*.cpp)
FILE(GLOB common_sources ${CMAKE_SOURCE_DIR}/src/common/*.cpp)

get_filename_component(full_path_main_cpp ../src/clustering/main.cpp ABSOLUTE)
get_filename_component(full_path_startup_cpp ../src/clustering/startup.cpp ABSOLUTE)
list(REMOVE_ITEM clustering_sources "${full_path_main_cpp}")
list(REMOVE_ITEM clustering_sources "${full_path_startup_cpp}")
#message(STATUS "${clustering_sources}=${${clustering_sources}}")
#message(STATUS "Andrei")
#message(STATUS "${full_path_main_cpp}=${${full_path_main_cpp}}")

#add_library(murmur3 ../extern/murmur3/murmur3.c ../extern/murmur3/murmur3.h)

#target_compile_features(term_doc_mat_bind PRIVATE cxx_std_17)

# add include directories for external libraries
target_include_directories(term_doc_mat_bind PRIVATE ../extern/murmur3)
target_include_directories(term_doc_mat_bind PRIVATE ../extern/porter2_stemmer)
target_include_directories(term_doc_mat_bind PRIVATE /usr/local/include/mongocxx/v_noabi)
target_include_directories(term_doc_mat_bind PRIVATE /usr/local/include/bsoncxx/v_noabi)

target_include_directories(term_doc_mat_bind PRIVATE ${CURL_INCLUDE_DIRS})

# add include directories for local files
target_include_directories(term_doc_mat_bind PRIVATE ../include/common)
target_include_directories(term_doc_mat_bind PRIVATE ../include/clustering)

# add external libraries
target_link_libraries(term_doc_mat_bind PRIVATE ${Boost_LIBRARIES})
target_link_libraries(term_doc_mat_bind PRIVATE murmur3)
target_link_libraries(term_doc_mat_bind PRIVATE ${porter2-stemmer_BINARY_DIR}/libporter2-stemmer.a)
target_link_libraries(term_doc_mat_bind PRIVATE fmt::fmt)
target_link_libraries(term_doc_mat_bind PRIVATE ${ZeroMQ_LIBRARY})
target_link_libraries(term_doc_mat_bind PRIVATE ${CURL_LIBRARIES})
target_link_libraries(term_doc_mat_bind PRIVATE /usr/local/lib/libbsoncxx.so)
target_link_libraries(term_doc_mat_bind PRIVATE /usr/local/lib/libmongocxx.so)
target_link_libraries(term_doc_mat_bind PRIVATE dlib::dlib)

target_sources(term_doc_mat_bind PRIVATE ${common_sources})
target_sources(term_doc_mat_bind PRIVATE ${clustering_sources})

