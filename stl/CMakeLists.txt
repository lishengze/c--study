# copyright [2025] [GTHT]
# author: lsz
# description: stl study CMakeLists.txt
# update-date:
#   2025年12月8日  init version
##############################################################################################

message("\n+++++++++++++++++++++++++ STL [START] +++++++++++++++++++++++++")

cmake_minimum_required(VERSION 3.12)
project(stl)

set("BIN_NAME" stl)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -pthread  -flto")

# set(BOOST_ROOT /usr/local/boost_1_68)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

aux_source_directory(. SRC_FILES)
###############################################################################################
include_directories(".")
include_directories("../component")
include_directories("../cpp/include")

# find_package(Boost 1.68.0 COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
# include_directories(${BOOST_INCLUDEDIR})


# link_directories("/usr/local/boost_1_68/lib")


add_executable(${BIN_NAME} ${SRC_FILES})

# target_link_libraries(${BIN_NAME} pandora)

target_link_libraries(${LIB_NAME} Threads::Threads)

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


message("\n+++++++++++++++++++++++++ STL [END] +++++++++++++++++++++++++")
