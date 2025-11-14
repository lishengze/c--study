# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(test_cmake)

set(BIN_NAME test_cmake)
set(LIB_NAME libtest_cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread  -flto -O3")


## Set CPP SRC
aux_source_directory(. MAIN_SRC)
aux_source_directory(func FUNC_SRC)
set(ALL_SRC ${MAIN_SRC} ${FUNC_SRC})

add_library(${LIB_NAME} SHARED ${ALL_SRC})
add_executable(${BIN_NAME} ${ALL_SRC})


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


