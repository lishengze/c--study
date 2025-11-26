# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(base)

set("BIN_NAME" base)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O0")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. SRC_LIST)
aux_source_directory(class_test/ SRC_LIST)
aux_source_directory(func_test/ SRC_LIST)
aux_source_directory(key_word/ SRC_LIST)
aux_source_directory(online_test/ SRC_LIST)
aux_source_directory(stl_test/ SRC_LIST)
aux_source_directory(string_test/ SRC_LIST)


find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 
                                     chrono regex program_options unit_test_framework locale REQUIRED)

# message( BOOST_INCLUDEDIR)
                                     
include_directories(${BOOST_INCLUDEDIR})
include_directories("../include")

include_directories("../../component/pandora")
include_directories("../../component")
include_directories("/usr/local/boost_1_68/include")
link_directories("/usr/local/boost_1_68/lib")
link_directories("/opt/utrade/console/lib/pandora/")


# set(ALL_SRC ${MAIN_SRC})

add_executable(${BIN_NAME} ${SRC_LIST})

target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})

# target_link_libraries(${BIN_NAME} dl)
# target_link_libraries(${BIN_NAME} fmt)


