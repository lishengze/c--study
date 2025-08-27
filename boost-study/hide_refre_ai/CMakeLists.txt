# CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)
project(FileChecker LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 关键配置：隐藏所有符号，仅显式标记的符号可见
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC/Clang 编译器：全局隐藏符号，包括内联函数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    message("\nGCC/Clang compiler detected. Hiding all symbols except explicitly marked ones.\n")

    # target_link_options(FileChecker PRIVATE "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export_symbols.map")

elseif(MSVC)
    # MSVC 编译器：默认隐藏未标记的符号
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    message("\nMSVC compiler detected. Hiding all symbols except explicitly marked ones.\n")
endif()

set(BOOST_ROOT /usr/local/boost_1_73_static_pic)

set(Boost_USE_STATIC_LIBS ON)  # 强制使用静态 Boost 库
# set(Boost_USE_STATIC_RUNTIME ON)  # 可选：静态链接 C++ 运行时

# 查找 Boost.Filesystem 组件（要求1.73版本）
find_package(Boost 1.73 REQUIRED COMPONENTS filesystem)

# 创建动态链接库
add_library(FileChecker SHARED
    file_checker.cpp
    file_checker.h
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_link_options(FileChecker PRIVATE "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export_symbols.map")
#   message(${CMAKE_CURRENT_SOURCE_DIR}/export_symbols.map)
  set(MY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/export_symbols.map)

    if(NOT EXISTS ${MY_DIR})    
         message(FATAL_ERROR "DIR: ${MY_DIR} DOES NOT EXIST")
    endif()  
    message(STATUS "  ${MY_DIR}")  
endif()

# 定义导出宏（仅在编译库时需要）
target_compile_definitions(FileChecker PRIVATE FILE_CHECKER_EXPORTS)

# 链接 Boost.Filesystem（PRIVATE 确保Boost符号不被传递导出）
target_link_libraries(FileChecker PRIVATE Boost::filesystem)

# 导出头文件目录，方便使用者包含
target_include_directories(FileChecker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 可选：添加测试程序
add_executable(checker_test main.cpp)
target_link_libraries(checker_test PRIVATE FileChecker)
