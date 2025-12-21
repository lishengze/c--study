#####################################################################################################################
cmake_minimum_required(VERSION 3.12)
project(sql)

set(BOOST_ROOT /usr/local/boost_1_68)

include_directories("../component")

link_directories("/opt/utrade/console/lib/quark")
link_directories("/opt/utrade/console/lib/pandora")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -pthread")

###############################################################################################################
find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
include_directories(${BOOST_INCLUDEDIR})
link_directories(${BOOST_INCLUDEDIR})

#####################################################################################################################
set("BIN_NAME" sql)

aux_source_directory(. OPU_SRC)


#####################################################################################################################
add_executable(${BIN_NAME} ${OPU_SRC})

#target_link_libraries(${BIN_NAME} zmq quark pandora log4cplus comm SQLiteCpp sqlite3 dl mysqlcppconn ${Boost_LIBRARIES} prometheus-cpp-pull prometheus-cpp-core lz)
target_link_libraries(${BIN_NAME} quark pandora )
target_link_libraries(${BIN_NAME} mysqlcppconn ${Boost_LIBRARIES})


#target_link_libraries(${BIN_NAME} zmq quark pandora log4cplus comm SQLiteCpp sqlite3 dl ${Boost_LIBRARIES})

