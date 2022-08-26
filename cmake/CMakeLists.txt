# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(test_cmake)

SET(BIN_NAME test_cmake)
SET(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")


## Set CPP SRC
AUX_SOURCE_DIRECTORY(. MAIN_SRC)
AUX_SOURCE_DIRECTORY(func FUNC_SRC)
SET(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

add_library(${LIB_NAME} SHARED ${ALL_SRC})
ADD_EXECUTABLE(${BIN_NAME} ${ALL_SRC})


set(CMAKE_INSTALL_PREFIX "/home/lsz/Code/Git/study/c--study/cmake/test_des")


# Test Install

# install dynamic lib
install(TARGETS ${LIB_NAME} LIBRARY DESTINATION lib)

# install bin
install(TARGETS ${BIN_NAME} RUNTIME DESTINATION bin)

# install header directory
install(DIRECTORY func DESTINATION include 
        PATTERN "./*.h" )

# install header file


