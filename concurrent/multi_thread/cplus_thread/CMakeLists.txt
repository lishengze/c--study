# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(thread)

SET("BIN_NAME" thread)
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto -O3")

# set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


AUX_SOURCE_DIRECTORY(./id ID_SRC)
AUX_SOURCE_DIRECTORY(. MAIN_SRC)

# FIND_PACKAGE(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 
#                                      chrono regex program_options unit_test_framework locale REQUIRED)
# INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# INCLUDE_DIRECTORIES("../../../component/pandora")
# INCLUDE_DIRECTORIES("../server")
# INCLUDE_DIRECTORIES("../include")
# INCLUDE_DIRECTORIES("../../proto")

######################  统计编译文件数目  ######################

set(SRC_FILES ${ID_SRC} ${MAIN_SRC})

foreach(FILE ${SRC_FILES})
    get_filename_component(PARENT_DIR ${FILE} DIRECTORY)
    set(DIR_STATS "${DIR_STATS};${PARENT_DIR}")
endforeach()

list(SORT DIR_STATS)
list(REMOVE_DUPLICATES DIR_STATS)

message(STATUS "${PROJECT_NAME}按目录统计的编译的 CPP 文件数目:")
foreach(DIR ${DIR_STATS})
    file(GLOB DIR_FILES "${DIR}/*.cpp")
    list(LENGTH DIR_FILES DIR_COUNT)
    message(STATUS "  ${DIR}: ${DIR_COUNT} 个文件")
endforeach()
list(LENGTH SRC_FILES SOURCE_COUNT)
message(STATUS "${PROJECT_NAME}合计编译的 CPP 文件数目: ${SOURCE_COUNT}\n")
if ( DEFINED BUILD_SUM_COUNT)
    math(EXPR BUILD_SUM_COUNT "${BUILD_SUM_COUNT} + ${SOURCE_COUNT}")
    set(BUILD_SUM_COUNT ${BUILD_SUM_COUNT} PARENT_SCOPE)
endif()

# LINK_DIRECTORIES("/usr/local/boost_1_68/lib")
# LINK_DIRECTORIES("../lib/")


# generate bin
ADD_EXECUTABLE(${BIN_NAME} ${SRC_FILES})


# TARGET_LINK_LIBRARIES(${BIN_NAME} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(${BIN_NAME} Threads::Threads)

