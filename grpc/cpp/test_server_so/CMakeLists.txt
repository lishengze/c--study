# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(test_spi)

set("BIN_NAME" test_spi)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. MAIN_SRC)
aux_source_directory(../../proto/cpp PROTO_SRC)

find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 
                                     chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# INCLUDE_DIRECTORIES("../../../component/pandora")
INCLUDE_DIRECTORIES("../server")
INCLUDE_DIRECTORIES("../include")
INCLUDE_DIRECTORIES("../../proto")



LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
LINK_DIRECTORIES("../lib/")


set(ALL_SRC ${MESSAGER_SRC}  ${MAIN_SRC} ${TEST_SRC})

# generate bin
add_executable(${BIN_NAME} ${ALL_SRC})

# generate the library
# ADD_LIBRARY(${PROJECT_NAME} SHARED ${ALL_SRC})


target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
target_link_libraries(${BIN_NAME} server)
target_link_libraries(${BIN_NAME} Threads::Threads)
target_link_libraries(${BIN_NAME} protobuf protoc protobuf-lite)
target_link_libraries(${BIN_NAME} grpc++_reflection grpc++ gpr ssl crypto)

