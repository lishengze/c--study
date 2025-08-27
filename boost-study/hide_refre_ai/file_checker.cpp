#define FILE_CHECKER_EXPORTS  // 定义导出宏，必须在包含头文件前
#include "file_checker.h"
#include <boost/filesystem.hpp>

// 命名空间别名，简化代码
namespace fs = boost::filesystem;

/**
 * 实现路径检查功能，内部使用Boost.Filesystem
 * 但符号会被隐藏，不暴露到库外部
 */
FileType check_path_type(const char* path) {
    if (!path) {
        return FileType::NOT_EXISTS;
    }

    try {
        fs::path fs_path(path);
        
        if (!fs::exists(fs_path)) {
            return FileType::NOT_EXISTS;
        } else if (fs::is_regular_file(fs_path)) {
            return FileType::REGULAR_FILE;
        } else if (fs::is_directory(fs_path)) {
            return FileType::DIRECTORY;
        }
    } catch (const fs::filesystem_error&) {
        // 捕获文件系统错误，统一返回"不存在"
        return FileType::NOT_EXISTS;
    }

    // 其他类型（如管道、设备文件等）统一视为普通文件
    return FileType::REGULAR_FILE;
}
