# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(read)

set("BIN_NAME" read)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. MAIN_SRC)


INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES("../include")

INCLUDE_DIRECTORIES("../../../component/pandora")
INCLUDE_DIRECTORIES("../../../component")


LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


set(ALL_SRC ${MAIN_SRC})

# generate bin
add_executable(${BIN_NAME} ${ALL_SRC})


