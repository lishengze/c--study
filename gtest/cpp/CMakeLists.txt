# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(testg)

SET("PROJECT_NAME" testg)
set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread  -flto -O3 -ggdb -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-access-control")

AUX_SOURCE_DIRECTORY(. MAIN_SRC)

SET(ALL_SRC ${MAIN_SRC})

# generate bin
ADD_EXECUTABLE(${PROJECT_NAME} ${ALL_SRC})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} gtest gmock)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} gcov)

# 将我们下载好的gcov对应的cmake包引入项目中
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build)

# # if判断语句是为了确保编译器是GNUCXX（即g++）以支持gcov功能
# if(CMAKE_COMPILER_IS_GNUCXX)

# 	# 引入CodeCoverage功能
#     include(CodeCoverage)
    
#     # 我的理解是：
#     # NAME后指定加入gcov功能的可执行文件名
#     # EXECUTABLE后指定原始可执行文件（也就是要进行分析的那个可执行文件）名字
#     # DEPENDENCIES也是原始可执行文件名字
#     setup_target_for_coverage_lcov(NAME ${PROJECT_NAME}_coverage EXECUTABLE ${PROJECT_NAME} DEPENDENCIES ${PROJECT_NAME})
    
# endif()




