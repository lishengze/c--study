# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(demo)

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread  -flto")

include_directories(./)


######################  统计编译文件数目  ######################
file(GLOB_RECURSE SRC_FILES "./*.cpp") #添加SRC子目录下全部源代码文件，排除特定文件;
list(FILTER SRC_FILES EXCLUDE REGEX "./CMakeFiles/.*")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES} )


