#pragma once

#include <string>
using std::string;

bool CheckFile(const string& sFileName);

// 显式导出公共符号（仅对库 A 的接口使用）
#define LIB_A_PUBLIC __attribute__((visibility("default")))

// 库 A 的公共接口（对外可见）
LIB_A_PUBLIC void public_function();

// 内部函数（自动隐藏，包括 Boost 相关调用）
void internal_function();