# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(TestVirtual)

set("BIN_NAME" TestVirtual)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json


aux_source_directory(. MAIN_SRC)


set(ALL_SRC ${MAIN_SRC})

# generate bin
add_executable(${BIN_NAME} ${ALL_SRC})


