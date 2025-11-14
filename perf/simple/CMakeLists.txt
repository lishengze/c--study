# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(perf_ana)

set(BIN_NAME perf_ana)
set(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -pg -O1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## Set CPP SRC
aux_source_directory(. MAIN_SRC)

set(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

# add_library(${LIB_NAME} SHARED ${ALL_SRC})

add_executable(${BIN_NAME} ${ALL_SRC})




