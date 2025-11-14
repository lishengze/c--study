# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(boost_study)

set("BIN_NAME" boost_study)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT C:/dev/cplus/boost/1_72_0)
    set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    find_package(Boost COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)    

    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

    # windows 下使用 MinGW 编译器时，可能需要指定编译器选项
    #cmake -G"MinGW Makefiles" ../       
else()
    message(STATUS "------This is not a Windows system.-----")
    # 在这里添加针对非Windows系统的特定设置或操作
    set(BOOST_ROOT /usr/local/boost_1_73)
    find_package(Boost  COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
endif()


message("******* BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})


aux_source_directory(. MAIN_SRC)
###############################################################################################
INCLUDE_DIRECTORIES(".")
INCLUDE_DIRECTORIES("../component")

add_executable(${BIN_NAME} ${MAIN_SRC})

target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
target_link_libraries(${LIB_NAME} Threads::Threads)

