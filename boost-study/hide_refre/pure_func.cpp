#include "pure_func.h"

#include <string>
#include <iostream>
using std::string;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem; // 简化命名空间

bool CheckFile(const string& sFileName) {
    fs::path p = sFileName.c_str();
    if (fs::exists(p)) {
        std::cout << "文件存在" << std::endl;
        if (fs::is_regular_file(p)) {         // 是否为普通文件
            std::cout << "是普通文件，大小: " << fs::file_size(p) << " bytes" << std::endl;
        } else if (fs::is_directory(p)) {     // 是否为目录
            std::cout << "是目录" << std::endl;
        }
    } else {
        std::cout << "文件不存在" << std::endl;
    }
    return true;
}