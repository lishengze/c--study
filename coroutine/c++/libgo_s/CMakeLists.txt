# author: lishengze
# description: libgo study
# update-date:
#   2021-11-12  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(base)

SET("BIN_NAME" base)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)

FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

INCLUDE_DIRECTORIES("../include")
INCLUDE_DIRECTORIES("../../../component")

LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


ADD_EXECUTABLE(${BIN_NAME} ${MAIN_SRC} )


TARGET_LINK_LIBRARIES(${BIN_NAME} libgo)
TARGET_LINK_LIBRARIES(${BIN_NAME} dl)
TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${BIN_NAME} pandora)
TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)
TARGET_LINK_LIBRARIES(${BIN_NAME} ssl crypto)


