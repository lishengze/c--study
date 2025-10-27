# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(client_strategy_test)

if ( NOT DEFINED BUILD_SUM_COUNT)
    message("######## test_strategy_client 独立编译 ######### ")

    set(BUILD_SUM_COUNT 0)      ## 用于统计整个编译的文件数目;

    ####################### 编译选项的通用性设置 ##########
    cmake_minimum_required(VERSION 3.10)
    if(POLICY CMP0177)
        cmake_policy(SET CMP0177 NEW) #解决 CMP0177 的告警问题;
    endif()


    set(CMAKE_CXX_STANDARD 11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto=8 -g")
    
endif()

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${WORK_MAIN_DIR}/share_comm/include)
include_directories(${WORK_MAIN_DIR}/share_comm)


AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR} STRAGY_MAIN_SRC)
AUX_SOURCE_DIRECTORY(${WORK_MAIN_DIR}/share_comm STRAGY_LIB_SRC)


SET(STRATEGY_ALL_SRC ${STRAGY_MAIN_SRC} ${STRAGY_LIB_SRC})

foreach(FILE ${STRATEGY_ALL_SRC})
    get_filename_component(PARENT_DIR ${FILE} DIRECTORY)
    set(DIR_STATS "${DIR_STATS};${PARENT_DIR}")
endforeach()

list(SORT DIR_STATS)
list(REMOVE_DUPLICATES DIR_STATS)

message(STATUS "${PROJECT_NAME}按目录统计的编译的 CPP 文件数目:")
foreach(DIR ${DIR_STATS})
    file(GLOB DIR_FILES "${DIR}/*.cpp")
    list(LENGTH DIR_FILES DIR_COUNT)
    message(STATUS "  ${DIR}: ${DIR_COUNT} 个文件")
endforeach()
list(LENGTH STRATEGY_ALL_SRC SOURCE_COUNT)
message(STATUS "${PROJECT_NAME}合计编译的 CPP 文件数目: ${SOURCE_COUNT}\n")

if ( DEFINED BUILD_SUM_COUNT)
    math(EXPR BUILD_SUM_COUNT "${BUILD_SUM_COUNT} + ${SOURCE_COUNT}")
    set(BUILD_SUM_COUNT ${BUILD_SUM_COUNT} PARENT_SCOPE)
endif()


ADD_EXECUTABLE(${PROJECT_NAME} ${STRATEGY_ALL_SRC} )

target_link_libraries(${PROJECT_NAME} rt)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${WORK_MAIN_DIR}/build)


