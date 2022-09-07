# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(client)

SET(BIN_NAME client) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")

include_directories("../../public-files/public-func")


## Set CPP SRC
AUX_SOURCE_DIRECTORY(. MAIN_SRC)
AUX_SOURCE_DIRECTORY(../../public-files/public-func PUBLIC_SRC)

SET(ALL_SRC ${MAIN_SRC} ${PUBLIC_SRC})

ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})




