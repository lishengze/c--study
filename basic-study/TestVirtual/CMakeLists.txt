# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(TestVirtual)

SET("BIN_NAME" TestVirtual)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json


AUX_SOURCE_DIRECTORY(. MAIN_SRC)


SET(ALL_SRC ${MAIN_SRC})

# generate bin
ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})


