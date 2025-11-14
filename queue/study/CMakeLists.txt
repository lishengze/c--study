# author: lishengze
# description: 测试无锁队列
# update-date:
#   2025-09-08
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(study_queue)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -pthread  -flto")

include_directories(include)
# link_directories(dev_lib)



######################  统计编译文件数目  ######################
file(GLOB_RECURSE SRC_FILES "./*.cpp") #添加SRC子目录下全部源代码文件，排除特定文件;
list(FILTER SRC_FILES EXCLUDE REGEX "./CMakeFiles/.*")

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

add_executable(${PROJECT_NAME} ${SRC_FILES} )

target_link_libraries(${PROJECT_NAME} -lrt -pthread)

