#Copyright: 
# Author: lishengze

# Version Require
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

SET("BIN_DIR" ./bin)
SET("BIN_NAME" asio_study)

# Project
PROJECT(asio_study)

# SRC DIR
aux_source_directory(src DIR_SRCS)

#设置需要的boost 组件
set(BOOST_COMPONENTS system python3 thread)

# 使用cmake find_package 查找boost库位置
find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# Set Pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include DIR
INCLUDE_DIRECTORIES(../public-files/public-class)
INCLUDE_DIRECTORIES(../public-files/public-func)

# Lib DIR
LINK_DIRECTORIES(../public-files/public-func)

# Set Exectuable
add_executable(${BIN_NAME} ${DIR_SRCS})

# Link Lib
TARGET_LINK_LIBRARIES(${BIN_NAME} func_math func_time)
TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)



