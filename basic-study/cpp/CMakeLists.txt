# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(base)

SET("BIN_NAME" base)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O0")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
AUX_SOURCE_DIRECTORY(class_test/ SRC_LIST)
AUX_SOURCE_DIRECTORY(func_test/ SRC_LIST)
AUX_SOURCE_DIRECTORY(key_word/ SRC_LIST)
AUX_SOURCE_DIRECTORY(online_test/ SRC_LIST)
AUX_SOURCE_DIRECTORY(stl_test/ SRC_LIST)
AUX_SOURCE_DIRECTORY(string_test/ SRC_LIST)


FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 
                                     chrono regex program_options unit_test_framework locale REQUIRED)

                                     
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
INCLUDE_DIRECTORIES("../include")

INCLUDE_DIRECTORIES("../../component/pandora")
INCLUDE_DIRECTORIES("../../component")
LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
LINK_DIRECTORIES("/opt/utrade/console/lib/pandora/")


# SET(ALL_SRC ${MAIN_SRC})

ADD_EXECUTABLE(${BIN_NAME} ${SRC_LIST})

TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})

# TARGET_LINK_LIBRARIES(${BIN_NAME} dl)
# TARGET_LINK_LIBRARIES(${BIN_NAME} fmt)


