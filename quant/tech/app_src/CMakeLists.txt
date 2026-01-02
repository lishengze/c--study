cmake_minimum_required(VERSION 3.16)
project(app_main)

# C++ 标准（兼容 C++11 及以上，推荐 C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

# 包含头文件目录
include_directories(${WORK_MAIN_DIR}/include)

# ===================== 2. 编译主程序 =====================
add_executable(
    ${PROJECT_NAME}  # 主程序名称
    import2.cpp
)

# # 链接动态库
# target_link_libraries(class_struct_app PRIVATE class_struct_dll)

message("install dir: ${WORK_MAIN_DIR}/build")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${WORK_MAIN_DIR}/build)