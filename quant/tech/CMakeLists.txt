cmake_minimum_required(VERSION 3.16)
project(cpp_dll_class_struct)

# C++ 标准（兼容 C++11 及以上，推荐 C++17）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# # 包含头文件目录
# include_directories(${PROJECT_SOURCE_DIR}/include)

# # ===================== 1. 编译动态库 =====================
# add_library(
#     class_struct_dll  # 动态库名称
#     SHARED            # 标识为动态库（Linux: .so; Windows: .dll）
#     lib_src/dll_class_impl.cpp
# )

# # 定义 LIB_BUILD 宏（动态库编译时使用）
# target_compile_definitions(class_struct_dll PRIVATE LIB_BUILD)

# # ===================== 2. 编译主程序 =====================
# add_executable(
#     class_struct_app  # 主程序名称
#     app_src/app_main.cpp
# )

# # 链接动态库
# target_link_libraries(class_struct_app PRIVATE class_struct_dll)

# # ===================== 跨平台优化 =====================
# if(WIN32)
#     # Windows：将 DLL 输出到主程序同一目录
#     set_target_properties(class_struct_dll PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     )
#     set_target_properties(class_struct_app PROPERTIES
#         RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#     )
# else()
#     # Linux：设置动态库版本
#     set_target_properties(class_struct_dll PROPERTIES
#         VERSION 1.0
#         SOVERSION 1
#     )
# endif()

add_subdirectory(lib_src)
add_subdirectory(app_src)
