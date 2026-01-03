cmake_minimum_required(VERSION 3.16)
project(alpha101_factor_calc)

# C++17标准（适配Eigen优化）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 量化低延迟编译优化（必开）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(BASE_CXX_FLAGS
    -O3                    # 最高优化
    # -march=native          # 适配CPU架构（AVX2/AVX512）
    # -ffast-math            # 浮点优化，不影响因子单调性
    # -fno-rtti -fno-exceptions # 禁用冗余开销
    # -falign-functions=32   # 函数对齐，提升缓存命中率
    # -funroll-loops         # 循环展开，加速窗口计算
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BASE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g0 -DNDEBUG")

# 查找Eigen3（必须，矩阵加速核心）
# find_package(Eigen3 3.4 REQUIRED NO_MODULE)

# include_directories(${EIGEN3_INCLUDE_DIR})

include_directories(/home/lsz/code/src/Eigen/3_4/include/eigen3)
include_directories(/home/lsz/code/src/open_blas_install/include)
link_directories(/home/lsz/code/src/open_blas_install/lib)

# # 查找OpenBLAS（可选，进一步加速大矩阵运算）
# find_library(OPENBLAS_LIB openblas PATHS /usr/local/lib /usr/lib NO_DEFAULT_PATH)
# if(OPENBLAS_LIB)
#     message(STATUS "OpenBLAS found, enable high performance mode")
#     add_definitions(-DEIGEN_USE_BLAS -DEIGEN_USE_LAPACKE)
# endif()

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 可执行文件
add_executable(alpha101_calc
    main.cpp
    alpha101_factors.cpp
)

# 链接依赖
# target_link_libraries(alpha101_calc PRIVATE Eigen3::Eigen)
target_link_libraries(alpha101_calc PRIVATE openblas pthread m)

