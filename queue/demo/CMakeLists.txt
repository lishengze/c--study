# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(demo)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto=8 -g")

include_directories(./)
include_directories(lib_src/include)
include_directories(lib_src)


AUX_SOURCE_DIRECTORY(. MAIN_SRC)
AUX_SOURCE_DIRECTORY(./lib_src LIB_SRC)
AUX_SOURCE_DIRECTORY(./test_src TEST_SRC)

SET(ALL_SRC ${MAIN_SRC} ${LIB_SRC} ${TEST_SRC})



ADD_EXECUTABLE(${PROJECT_NAME} ${ALL_SRC} )

add_subdirectory(test_strategy_client)

add_subdirectory(test_ute_server)


