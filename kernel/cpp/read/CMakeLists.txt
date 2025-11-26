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


include_directories(${BOOST_INCLUDEDIR})
include_directories("../include")

include_directories("../../../component/pandora")
include_directories("../../../component")


link_directories("/opt/utrade/console/lib/pandora/")


set(ALL_SRC ${MAIN_SRC})

# generate bin
add_executable(${BIN_NAME} ${ALL_SRC})


