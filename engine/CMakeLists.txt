cmake_minimum_required(VERSION 3.18)
project(unbiased VERSION 1.0
                  DESCRIPTION "News Clustering"
                  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)

enable_testing()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(doc)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem thread date_time serialization locale)

find_package(spdlog REQUIRED)

find_package(fmt REQUIRED)

find_package(bsoncxx REQUIRED)

find_package(mongocxx REQUIRED)

#find_package(icu REQUIRED)

#find_package(libcurl REQUIRED)



## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

pkg_check_modules(CURL libcurl REQUIRED)

add_subdirectory(extern/dlib)

add_subdirectory(extern/porter2_stemmer)

add_subdirectory(src/dedup)

add_subdirectory(src/clustering)

add_subdirectory(unit_tests)

#[[
FOR DEBUGGING
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
]]


#add_executable(zmq_broker zmq_broker.cpp)

