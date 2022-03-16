#####################################################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(kernel)
SET("BIN_NAME" kernel)

set(BOOST_ROOT /usr/local/boost_1_68)

INCLUDE_DIRECTORIES("./")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

###############################################################################################################
FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_INCLUDEDIR})

#####################################################################################################################


AUX_SOURCE_DIRECTORY(. OPU_SRC)


#####################################################################################################################
ADD_EXECUTABLE(${BIN_NAME} ${OPU_SRC})



