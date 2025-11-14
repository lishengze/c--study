# author: lishengze
# description: kafka basic test
# update-date:
#   2021-11-12  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(producer)

set("BIN_NAME" producer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O0 -pthread  -flto -O3")

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. MAIN_SRC)

find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

INCLUDE_DIRECTORIES("../include")
INCLUDE_DIRECTORIES("../../../component")

LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


add_executable(${BIN_NAME} ${MAIN_SRC} ${PROTO_SRC})


target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
target_link_libraries(${BIN_NAME} pandora)
target_link_libraries(${BIN_NAME} Threads::Threads)
target_link_libraries(${BIN_NAME} ssl crypto)
target_link_libraries(${BIN_NAME} rdkafka rdkafka++)

