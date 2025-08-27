#ifndef FILE_CHECKER_H
#define FILE_CHECKER_H

// 导出/导入宏定义（跨平台）
#ifdef _WIN32
    #ifdef FILE_CHECKER_EXPORTS  // 编译库时定义，用于导出符号
        #define FILE_CHECKER_API __declspec(dllexport)
    #else                        // 使用库时，用于导入符号
        #define FILE_CHECKER_API __declspec(dllimport)
    #endif
#else
    // Linux/macOS 下通过 visibility 属性控制符号导出
    #define FILE_CHECKER_API __attribute__((visibility("default")))
#endif


// 导出宏：编译库时标记为可见，使用库时标记为导入
#ifdef FILE_CHECKER_EXPORTS
#define FILE_CHECKER_PUBLIC __attribute__((visibility("default")))
#else
#define FILE_CHECKER_PUBLIC __attribute__((visibility("default"))) // 导入时也可见
#endif

// 文件类型枚举（公共接口）
enum class FileType {
    NOT_EXISTS,   // 路径不存在
    REGULAR_FILE, // 普通文件
    DIRECTORY     // 目录
};

/**
 * @brief 检查指定路径的类型
 * @param path 要检查的路径字符串
 * @return 路径类型（FileType枚举）
 */
extern "C" FILE_CHECKER_PUBLIC FileType check_path_type(const char* path);

#endif // FILE_CHECKER_H
