#####################################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(sql)

set(BOOST_ROOT /usr/local/boost_1_68)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -pthread")

###############################################################################################################
FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_INCLUDEDIR})

#####################################################################################################################
SET("BIN_NAME" sql)

AUX_SOURCE_DIRECTORY(. OPU_SRC)


#####################################################################################################################
ADD_EXECUTABLE(${BIN_NAME} ${OPU_SRC})

#TARGET_LINK_LIBRARIES(${BIN_NAME} zmq quark pandora log4cplus comm SQLiteCpp sqlite3 dl mysqlcppconn ${Boost_LIBRARIES} prometheus-cpp-pull prometheus-cpp-core lz)
TARGET_LINK_LIBRARIES(${BIN_NAME} mysqlcppconn ${Boost_LIBRARIES})
#TARGET_LINK_LIBRARIES(${BIN_NAME} zmq quark pandora log4cplus comm SQLiteCpp sqlite3 dl ${Boost_LIBRARIES})

