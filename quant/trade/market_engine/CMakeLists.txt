# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(market)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g")

set(CMAKE_BUILD_TYPE Debug)

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

message("work_main_dir: ${WORK_MAIN_DIR}")

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

set(install_dir ${WORK_MAIN_DIR}/build)
message("install_dir: ${install_dir}")


install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${install_dir})

# 使用相对路径，会自动结合 CMAKE_INSTALL_PREFIX
# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ".")

# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})