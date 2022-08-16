# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(read)

SET("BIN_NAME" read)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)


INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES("../include")

INCLUDE_DIRECTORIES("../../../component/pandora")
INCLUDE_DIRECTORIES("../../../component")


LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


SET(ALL_SRC ${MAIN_SRC})

# generate bin
ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})


