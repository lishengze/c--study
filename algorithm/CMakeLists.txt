# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(algo)

SET("BIN_NAME" algo)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto")

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)
AUX_SOURCE_DIRECTORY(algo ALGO_SRC)
AUX_SOURCE_DIRECTORY(hash HASH_SRC)
AUX_SOURCE_DIRECTORY(map  MAP_SRC)
AUX_SOURCE_DIRECTORY(sort SORT_SRC)
AUX_SOURCE_DIRECTORY(tree TREE_SRC)
AUX_SOURCE_DIRECTORY(stack_list STACK_LIST_SRC)

FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})


LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
# LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


ADD_EXECUTABLE(${BIN_NAME} ${MAIN_SRC} ${ALGO_SRC} ${HASH_SRC} ${MAP_SRC} ${SORT_SRC} ${TREE_SRC} ${STACK_LIST_SRC})


TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)

