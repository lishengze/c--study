# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g -pthread  -Wreorder")

set(CMAKE_BUILD_TYPE Debug)

# 设置安装前缀为 build 目录
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/build")

add_subdirectory(market_engine)
add_subdirectory(trade_engine)
add_subdirectory(strategy_impl1)


