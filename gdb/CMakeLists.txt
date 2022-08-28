# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(test_gdb)

SET(BIN_NAME test_gdb)
SET(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json


# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3 -g")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


## Set CPP SRC
AUX_SOURCE_DIRECTORY(. MAIN_SRC)

SET(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

# add_library(${LIB_NAME} SHARED ${ALL_SRC})

ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})



