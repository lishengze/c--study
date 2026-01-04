#include <cstring>
#include <array>
#include <functional>
#include <algorithm>
#include <string>
#include <chrono>
#include <memory>

#include <unordered_map>
#include <iostream>
#include <vector>
#include <mutex>
#include <set>


// 模板化通用别名，可复用
template <typename T>
using my_vector = std::vector<T>;

template <typename T>
using my_set = std::set<T>;

template <typename key, typename value>
using my_unorder_map = std::unordered_map<key, value>;

using my_mutex = std::mutex;

using my_string = std::string;
