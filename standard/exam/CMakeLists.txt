# copyright [2025]
# description: Learning C++
# update-date: 2021-09-25
# update-author: <李晟泽>
##############################################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(exam)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (${WIN32})
    message(STATUS "----- This is a Windows system. ----")

    # 设置 C 编译器
    set(CMAKE_C_COMPILER "gcc")
    # 设置 C++ 编译器
    set(CMAKE_CXX_COMPILER "g++")

    # 设置 C 编译器
    # set(CMAKE_C_COMPILER "C:/dev/cplus/2.3.0/w64devkit/bin/gcc")
    # # 设置 C++ 编译器
    # set(CMAKE_CXX_COMPILER "C:/dev/cplus/2.3.0/w64devkit/bin/g++")
        
    # 指定 MinGW 生成器（可选，命令行指定更灵活）
    set(CMAKE_GENERATOR "MinGW Makefiles")    
        
    # 在这里添加针对Windows系统的特定设置或操作
    # set(BOOST_ROOT C:/dev/cplus/boost/1_72_0)
    # set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    # find_package(Boost COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)    

    # set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    # set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

    #cmake -G "MinGW Makefiles" ../
else()
    message(STATUS "------This is not a Windows system.-----")
    # 在这里添加针对非Windows系统的特定设置或操作
    set(BOOST_ROOT /usr/local/boost_1_73)
    # FIND_PACKAGE(Boost  COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
endif()


# message("******* BOOST_INCLUDEDIR: ${boost_INCLUDE_DIRS}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

AUX_SOURCE_DIRECTORY(. SRC_FILES)

INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})

foreach(FILE ${SRC_FILES})
    get_filename_component(PARENT_DIR ${FILE} DIRECTORY)
    set(DIR_STATS "${DIR_STATS};${PARENT_DIR}")
endforeach()

list(SORT DIR_STATS)
list(REMOVE_DUPLICATES DIR_STATS)

message(STATUS "${PROJECT_NAME} 按目录统计的编译的 CPP 文件数目:")
foreach(DIR ${DIR_STATS})
    file(GLOB DIR_FILES "${DIR}/*.cpp")
    list(LENGTH DIR_FILES DIR_COUNT)
    message(STATUS "  ${DIR}: ${DIR_COUNT} 个文件")
endforeach()
list(LENGTH SRC_FILES SOURCE_COUNT)
message(STATUS "${PROJECT_NAME}  合计编译的 CPP 文件数目: ${SOURCE_COUNT}\n")
if ( DEFINED BUILD_SUM_COUNT)
    math(EXPR BUILD_SUM_COUNT "${BUILD_SUM_COUNT} + ${SOURCE_COUNT}")
    set(BUILD_SUM_COUNT ${BUILD_SUM_COUNT} PARENT_SCOPE)
endif()


ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})

