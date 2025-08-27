#include "file_checker.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <待检查路径>" << std::endl;
        return 1;
    }

    const char* path = argv[1];
    FileType type = check_path_type(path);

    std::cout << "路径: " << path << std::endl;
    switch (type) {
        case FileType::NOT_EXISTS:
            std::cout << "状态: 不存在" << std::endl;
            break;
        case FileType::REGULAR_FILE:
            std::cout << "状态: 普通文件" << std::endl;
            break;
        case FileType::DIRECTORY:
            std::cout << "状态: 目录" << std::endl;
            break;
    }

    return 0;
}
