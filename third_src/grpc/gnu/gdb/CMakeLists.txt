# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(test_gdb)

set(BIN_NAME test_gdb)
set(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json


# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3 -g")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")


## Set CPP SRC
aux_source_directory(. MAIN_SRC)

set(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

# add_library(${LIB_NAME} SHARED ${ALL_SRC})

add_executable(${BIN_NAME} ${ALL_SRC})



