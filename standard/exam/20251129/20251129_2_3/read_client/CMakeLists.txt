# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(read_client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g -pthread  -flto=8 ")

set(CMAKE_BUILD_TYPE Debug)

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${WORK_MAIN_DIR}/share_comm/include)
include_directories(${WORK_MAIN_DIR}/share_comm)


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} STRAGY_MAIN_SRC)
aux_source_directory(${WORK_MAIN_DIR}/share_comm STRAGY_LIB_SRC)

# message(${STRAGY_MAIN_SRC})
# message(${STRAGY_LIB_SRC})


set(STRATEGY_ALL_SRC ${STRAGY_MAIN_SRC} ${STRAGY_LIB_SRC})

add_executable(${PROJECT_NAME} ${STRATEGY_ALL_SRC} )

target_link_libraries(${PROJECT_NAME} rt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${WORK_MAIN_DIR}/cmake_build)


