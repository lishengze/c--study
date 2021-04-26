# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(server)

SET("BIN_NAME" server)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. MAIN_SRC)
AUX_SOURCE_DIRECTORY(test TEST_SRC)
AUX_SOURCE_DIRECTORY(../../proto/cpp PROTO_SRC)

FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# INCLUDE_DIRECTORIES("../../../component/pandora")
# INCLUDE_DIRECTORIES("../../../component")
INCLUDE_DIRECTORIES("../include")
INCLUDE_DIRECTORIES("../../proto")



LINK_DIRECTORIES("/usr/local/boost_1_68/lib")


ADD_EXECUTABLE(${BIN_NAME} ${MAIN_SRC} ${PROTO_SRC} ${TEST_SRC})


TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})
# TARGET_LINK_LIBRARIES(${BIN_NAME} pandora)
TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)
TARGET_LINK_LIBRARIES(${BIN_NAME} protobuf protoc protobuf-lite)
TARGET_LINK_LIBRARIES(${BIN_NAME} grpc++_reflection grpc++ gpr ssl crypto)

