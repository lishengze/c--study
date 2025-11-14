# copyright [2018] [wxblockchain]
# author: daniel.bian
# description: galaxy project main CMakeLists.txt
# update-date:
#   2019-01-04  init version
##############################################################################################
cmake_minimum_required(VERSION 3.12)
project(preprocess_study)

set("BIN_NAME" preprocess_study)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT C:/dev/cplus/boost/1_72_0)
    set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    find_package(Boost COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)    

    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

    # windows 下使用 MinGW 编译器时，可能需要指定编译器选项
    #cmake -G"MinGW Makefiles" ../       
else()
    message(STATUS "------This is not a Windows system.-----")
    # 在这里添加针对非Windows系统的特定设置或操作
    set(BOOST_ROOT /usr/local/boost_1_73)
    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)    
    find_package(Boost  COMPONENTS date_time filesystem system thread serialization math_tr1 chrono regex program_options unit_test_framework locale REQUIRED)
endif()


message("******* BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})


aux_source_directory(. MAIN_SRC)

# set(SOURCE_FILES business_processor.cpp)
# foreach(source_file ${SOURCE_FILES})
#     get_filename_component(base_name ${source_file} NAME_WE)
#     add_custom_target(
#         preprocess_${base_name}
#         COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} -E ${source_file} -o ${base_name}.i
#         DEPENDS ${source_file}
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     )
# endforeach()

# add_executable(${BIN_NAME} ${MAIN_SRC})

# # 添加预处理构建配置
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -E")  # -E 选项仅预处理不编译
# add_custom_target(
#     preprocess
#     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target preprocess_study
#     COMMENT "Generating preprocessed files..."
# )

# target_link_libraries(${BIN_NAME} ${Boost_LIBRARIES})
# target_link_libraries(${LIB_NAME} Threads::Threads)


# CMakeLists.txt
# 查找所有 C++ 源文件
file(GLOB_RECURSE ALL_CXX_SOURCE_FILES *.cpp *.cc)

# 添加预处理目标
add_custom_target(
    preprocess_all
    COMMENT "Generating preprocessed files..."
)

foreach(source_file ${ALL_CXX_SOURCE_FILES})
    get_filename_component(abs_file ${source_file} ABSOLUTE)
    get_filename_component(base_name ${source_file} NAME_WE)
    get_filename_component(dir_name ${source_file} DIRECTORY)
    
    # 创建输出目录
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/preprocessed/${dir_name})
    
    # 添加预处理命令
    add_custom_command(
        TARGET preprocess_all
        COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS} -E ${abs_file}  
                    -o ${CMAKE_BINARY_DIR}/preprocessed/${dir_name}/${base_name}.i -I ${BOOST_INCLUDEDIR}
        DEPENDS ${abs_file}
        VERBATIM
    )
endforeach()


