#Copyright: 
# Author: lishengze

# Version Require
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

SET("BIN_DIR" ./bin)
SET("BIN_NAME" asio_study)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}, WIN32: ${WIN32}")

if (${WIN32})
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT C:/dev/cplus/boost/1_72_0)
    # set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    # find_package(Boost COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)    

    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

    # windows 下使用 MinGW 编译器时，可能需要指定编译器选项
    #cmake -G"MinGW Makefiles" ../       
else()
    message(STATUS "------This is not a Windows system.-----")
    # 在这里添加针对非Windows系统的特定设置或操作
    set(BOOST_ROOT /usr/local/boost_1_73)
    FIND_PACKAGE(Boost  COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
endif()

# Project
PROJECT(asio_study)

# SRC DIR
aux_source_directory(src DIR_SRCS)

# # 使用cmake find_package 查找boost库位置
# find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

# Include DIR
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# Lib DIR
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})

# Set Exectuable
add_executable(${BIN_NAME} ${DIR_SRCS})

# Link Lib
TARGET_LINK_LIBRARIES(${BIN_NAME} boost_system boost_thread boost_chrono boost_date_time boost_atomic)



