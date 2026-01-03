cmake_minimum_required(VERSION 3.16)
project(alpha101_pure_cpp)

# C++11及以上即可，推荐C++17（优化vector性能）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 量化低延迟编译优化（关键，必须开启）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 低延迟编译参数（适配高频场景）
set(BASE_CXX_FLAGS
    -O3                    # 最高优化级别
    # -march=native          # 自动适配CPU架构（AVX2/AVX512）
    # -ffast-math            # 浮点优化，不影响因子单调性
    # -fno-rtti -fno-exceptions # 禁用冗余开销，提速30%+
    # -fomit-frame-pointer   # 省略帧指针，提升调用效率
    # -funroll-loops         # 循环展开，加速窗口计算
    # -falign-functions=32   # 函数32字节对齐，提升缓存命中率
    # -Wno-unused-result     # 禁用无关警告
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0 -DNDEBUG") # 禁用调试信息

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 可执行文件（无任何第三方依赖，直接编译）
add_executable(alpha101_pure_calc
    main.cpp
    alpha_vector.cpp
)

# 打印配置信息
message(STATUS "=== Alpha101 Pure C++ Config ===")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "No Third-Party Dependency: ON")