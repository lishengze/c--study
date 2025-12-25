# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g -pthread  -flto=8  -Wreorder")

set(CMAKE_BUILD_TYPE Debug)

include_directories(./)
include_directories(share_comm/include)
include_directories(share_comm)


aux_source_directory(. MAIN_SRC)
aux_source_directory(./share_comm LIB_SRC)
aux_source_directory(./test_src TEST_SRC)

set(ALL_SRC ${MAIN_SRC} ${LIB_SRC} ${TEST_SRC})



add_executable(${PROJECT_NAME} ${ALL_SRC} )
target_link_libraries(${PROJECT_NAME} rt)


add_subdirectory(test_strategy_client)

add_subdirectory(test_ute_server)


