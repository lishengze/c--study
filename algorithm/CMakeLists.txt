# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(algo)

set("BIN_NAME" algo)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT D:/dev/boost/build/1_72)
    set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    find_package(Boost COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)    

    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
else()
    message(STATUS "------This is not a Windows system.-----")
    # 在这里添加针对非Windows系统的特定设置或操作
    set(BOOST_ROOT /usr/local/boost_1_68)
    find_package(Boost  COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
endif()

message("******* BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. MAIN_SRC)
aux_source_directory(algo ALGO_SRC)
aux_source_directory(hash HASH_SRC)
aux_source_directory(map  MAP_SRC)
aux_source_directory(sort SORT_SRC)
aux_source_directory(tree TREE_SRC)
aux_source_directory(stack_list STACK_LIST_SRC)

INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})

add_executable(${BIN_NAME} ${MAIN_SRC} ${ALGO_SRC} ${HASH_SRC} ${MAP_SRC} ${SORT_SRC} ${TREE_SRC} ${STACK_LIST_SRC})


target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
target_link_libraries(${BIN_NAME} Threads::Threads)

