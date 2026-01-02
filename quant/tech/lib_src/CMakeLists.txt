cmake_minimum_required(VERSION 3.16)
project(app_dll)

# C++ 标准（兼容 C++11 及以上，推荐 C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(WORK_MAIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)

# 包含头文件目录
include_directories(${WORK_MAIN_DIR}/include)

# ===================== 1. 编译动态库 =====================
add_library(
    ${PROJECT_NAME}  # 动态库名称
    SHARED            # 标识为动态库（Linux: .so; Windows: .dll）
    dll_class_impl.cpp
)

# 定义 LIB_BUILD 宏（动态库编译时使用）
target_compile_definitions(${PROJECT_NAME} PRIVATE LIB_BUILD)


# # ===================== 跨平台优化 =====================
# if(WIN32)
#     # Windows：将 DLL 输出到主程序同一目录
#     set_target_properties(${PROJECT_NAME} PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     )
#     set_target_properties(class_struct_app PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     )
# else()
#     # Linux：设置动态库版本
#     set_target_properties(${PROJECT_NAME} PROPERTIES
#         VERSION 1.0
#         SOVERSION 1
#     )
# endif()

message("install dir: ${WORK_MAIN_DIR}/build")

install(TARGETS ${PROJECT_NAME}  DESTINATION ${WORK_MAIN_DIR}/build)
