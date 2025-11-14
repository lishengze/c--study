# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(server)

set(BIN_NAME server) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")

include_directories("../../public-files/public-func")


## Set CPP SRC
aux_source_directory(. MAIN_SRC)
aux_source_directory(../../public-files/public-func PUBLIC_SRC)

set(ALL_SRC ${MAIN_SRC} ${PUBLIC_SRC})

add_executable(${BIN_NAME} ${ALL_SRC})




