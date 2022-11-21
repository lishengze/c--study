# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(perf_ana)

SET(BIN_NAME perf_ana)
SET(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -pg -O1")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## Set CPP SRC
AUX_SOURCE_DIRECTORY(. MAIN_SRC)

SET(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

# add_library(${LIB_NAME} SHARED ${ALL_SRC})

ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})

TARGET_LINK_LIBRARIES(${BIN_NAME} benchmark)



