#Copyright: 
# Author: lishengze

# Version Require
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

SET("BIN_DIR" ./bin)
SET("BIN_NAME" asio_study)

message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}, WIN32: ${WIN32}")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



if (${WIN32})
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT C:/dev/cplus/boost/1_72_share)
    # set(Boost_COMPILER "-mgw142")  # 这里根据实际情况设置合适的值
    # find_package(Boost COMPONENTS  filesystem system thread  REQUIRED)    

    set(CMAKE_C_COMPILER "C:/dev/cplus/w64devkit/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "C:/dev/cplus/w64devkit/bin/g++.exe")
    set(CMAKE_LINKER "C:/dev/cplus/w64devkit/bin/ld.exe")

    PROJECT(asio_study LANGUAGES CXX)

    if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
    endif()

    set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include/boost-1_72)
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)

    # set(Boost_NO_BOOST_CMAKE ON)      # 不能随意设置
    set(Boost_COMPILER "-mgw142")      # 匹配 mgw142 工具链
    set(Boost_TOOLSET "mingw") 
    set(Boost_THREADING "mt")          # 匹配多线程版本
    set(Boost_ADDRESS_MODEL 32)        # 32位库（根据实际修改为64）
    set(Boost_USE_STATIC_LIBS OFF)     # 使用动态库
    set(Boost_USE_SHARED_LIBS ON)
    set(Boost_USE_DEBUG_LIBS ON)       # 调试模式（发布模式改为 OFF）

    # find_package(Boost REQUIRED COMPONENTS system thread chrono date_time atomic)

    message("BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}, BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}")
    message("Boost_LIBRARIES: ${Boost_LIBRARIES}")

    set (LIB_LIST boost_system-mgw142-mt-x32-1_72
                boost_thread-mgw142-mt-x32-1_72
                 boost_chrono-mgw142-mt-x32-1_72 
                 boost_date_time-mgw142-mt-x32-1_72 
                boost_atomic-mgw142-mt-x32-1_72)
    
    # windows 下使用 MinGW 编译器时，可能需要指定编译器选项
    #cmake -G"MinGW Makefiles" ../       
else()
    message(STATUS "------This is not a Windows system.-----")
    PROJECT(asio_study LANGUAGES CXX)    
    set(BOOST_ROOT /usr/local/boost_1_73)
    FIND_PACKAGE(Boost  COMPONENTS  filesystem system thread  REQUIRED)

    set (LIB_LIST boost_system
                boost_thread
                 boost_chrono
                 boost_date_time
                boost_atomic)
endif()

# PROJECT(asio_study)

# SRC DIR
aux_source_directory(src DIR_SRCS)

# Include DIR
INCLUDE_DIRECTORIES(${BOOST_INCLUDEDIR})

# Lib DIR
LINK_DIRECTORIES(${BOOST_LIBRARYDIR})

# Set Exectuable
add_executable(${BIN_NAME} ${DIR_SRCS})

# Link Lib
TARGET_LINK_LIBRARIES(${BIN_NAME} ${LIB_LIST})



