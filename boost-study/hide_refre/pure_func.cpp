#include "pure_func.h"

#include <string>
#include <iostream>
using std::string;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem; // 简化命名空间

#define BOOST_SYMBOL_HIDDEN __attribute__((visibility("hidden")))

LIB_A_PUBLIC bool  CheckFileOut(const string& sFileName) {
    return CheckFile(sFileName);
}

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

#include <boost/algorithm/string.hpp> // 使用 Boost 库

// 公共接口（对外可见）
void public_function() {
    internal_function();
}

// 内部函数（自动隐藏）
void internal_function() {
    std::string s = "hello boost";
    boost::algorithm::to_upper(s); // Boost 函数调用（符号隐藏）
}