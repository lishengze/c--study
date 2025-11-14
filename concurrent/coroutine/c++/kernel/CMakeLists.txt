#####################################################################################################################
cmake_minimum_required(VERSION 3.12)
project(kernel)
set("BIN_NAME" kernel)

set(BOOST_ROOT /usr/local/boost_1_68)

INCLUDE_DIRECTORIES("./")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

###############################################################################################################
find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_INCLUDEDIR})

#####################################################################################################################


aux_source_directory(. OPU_SRC)


#####################################################################################################################
add_executable(${BIN_NAME} ${OPU_SRC})



