# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(server_ute_test)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto=8 -g")

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${WORK_MAIN_DIR}/lib_src/include)
include_directories(${WORK_MAIN_DIR}/lib_src)


AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} UTE_MAIN_SRC)
AUX_SOURCE_DIRECTORY(${WORK_MAIN_DIR}/lib_src UTE_LIB_SRC)

SET(UTE_ALL_SRC ${UTE_MAIN_SRC} ${UTE_LIB_SRC})

ADD_EXECUTABLE(${PROJECT_NAME} ${UTE_ALL_SRC} )


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${WORK_MAIN_DIR}/build)