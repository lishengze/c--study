message("\n+++++++++++++++++++++++++ C--Study CMake [START] +++++++++++++++++++++++++")

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON) ## 解决可能出现的 RPATH 问题;

if ( NOT DEFINED BUILD_SUM_COUNT)
    message("######## C--Study 独立编译 ######### ")

    set(WORK_MAIN ${CMAKE_CURRENT_SOURCE_DIR})

    set(BUILD_SUM_COUNT 0)      ## 用于统计整个编译的文件数目;

    ####################### 编译选项的通用性设置 ##########
    cmake_minimum_required(VERSION 3.10)
    if(POLICY CMP0177)
        cmake_policy(SET CMP0177 NEW) #解决 CMP0177 的告警问题;
    endif()

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)    
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)  

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    
endif()

if (${WIN32})
    message(STATUS "----- This is a Windows system. ----")
    # 在这里添加针对Windows系统的特定设置或操作
    set(BOOST_ROOT C:/dev/cplus/boost/1_72_share)

    set(CMAKE_C_COMPILER "C:/dev/cplus/w64devkit/bin/gcc.exe")
    set(CMAKE_CXX_COMPILER "C:/dev/cplus/w64devkit/bin/g++.exe")
    set(CMAKE_LINKER "C:/dev/cplus/w64devkit/bin/ld.exe")

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
    
    # windows 下使用 MinGW 编译器时，可能需要指定编译器选项
    #cmake -G"MinGW Makefiles" ../       
else()
    message(STATUS "------This is not a Windows system.-----")
    project(asio_study LANGUAGES CXX)    
    set(BOOST_ROOT /usr/local/boost_1_73)
endif()


project(WORK_MAIN)


# #添加子项目 - 按照顺序;
add_subdirectory(asio)  
add_subdirectory(concurrent/coroutine/c++/libgo_s)
add_subdirectory(concurrent/multi_thread/cplus_thread)

message("整个工程当前 共编译 ${BUILD_SUM_COUNT} 个文件")

message("\n+++++++++++++++++++++++++ MAIN CMake [END] +++++++++++++++++++++++++")
